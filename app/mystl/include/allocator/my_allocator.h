#ifndef MYSTL_MY_ALLOCATOR_H_
#define MYSTL_MY_ALLOCATOR_H_

#include "construct.h"

namespace mystl {
class Chunk {
public:
    Chunk(unsigned char blockNum, std::size_t blockSize) {
        if (blockNum != 0U && blockSize != 0U) {
            pBlocks_ = new unsigned char[blockNum * blockSize];
            Reset(blockNum, blockSize);
        }
    }
    ~Chunk() {
        if (pBlocks_ != nullptr) {
            delete pBlocks_;
        }
    }

    unsigned char* Allocate(std::size_t blockSize) {
        if (blocksAvailable_ == 0U) {
            return nullptr;
        }
        unsigned char* result = pBlocks_ + (firstAvailableBlock_ * blockSize);
        firstAvailableBlock_ = *result;
        --blocksAvailable_;
        return result;
    }

    void Deallocate(unsigned char* ptr, std::size_t blockSize) {
        *ptr = firstAvailableBlock_;
        firstAvailableBlock_ = (ptr - pBlocks_) / blockSize;
        ++blocksAvailable_;
    }

    bool IsAvailable() {
        return blocksAvailable_ > 0U;
    }

    bool IsInside(unsigned char* ptr, std::size_t chunkSize) {
        return (ptr >= pBlocks_) && (ptr < pBlocks_ + chunkSize);
    }

    bool IsAllBlockFree(unsigned char blockNum_) {
        return blocksAvailable_ == blockNum_;
    }

private:
    void Reset(unsigned char blockNum, std::size_t blockSize) {
        firstAvailableBlock_ = 0;
        blocksAvailable_ = blockNum;

        unsigned char* p = pBlocks_;
        for (unsigned char i = 0; i < blockNum; ++i) {
            *p = (i + 1);
            p += blockSize;
        }
    }

private:
    unsigned char firstAvailableBlock_{0U};
    unsigned char blocksAvailable_{0U};
    unsigned char* pBlocks_{nullptr};
};

// 双向链表保存Chunk
class ChunkList {
public:
    ChunkList(unsigned char blockNum = 0U, std::size_t blockSize = 0U) :
        next_(this), prev_(this), chunk_(blockNum, blockSize) {
    }

    unsigned char* Allocate(std::size_t blockSize) {
        return chunk_.Allocate(blockSize);
    }

    void Deallocate(unsigned char* ptr, std::size_t blockSize) {
        chunk_.Deallocate(ptr, blockSize);
    }

    bool IsAvailable() {
        return chunk_.IsAvailable();
    }

    bool IsAllBlockFree(unsigned char blockNum) {
        return chunk_.IsAllBlockFree(blockNum);
    }

    void InsertAtTail(ChunkList* p) {
        p->next_ = this;
        p->prev_ = prev_;
        prev_->next_ = p;
        prev_ = p;
    }

    void Remove(ChunkList* p) {
        p->prev_->next_ = p->next_;
        p->next_->prev_ = p->prev_;
    }

    ChunkList* FindAvailableChunk() {
        ChunkList* p = this->next_;
        while (p != this) {
            if (p->chunk_.IsAvailable()) {
                return this;
            }
            p = p->next_;
        }
        return nullptr;
    }

    ChunkList* FindLocatedChunk(unsigned char* ptr, std::size_t chunkSize) {
        ChunkList* p = this->next_;
        while (p != this) {
            if (p->chunk_.IsInside(ptr, chunkSize)) {
                return this;
            }
            p = p->next_;
        }
        return nullptr;
    }

private:
    ChunkList* next_{nullptr};
    ChunkList* prev_{nullptr};
    Chunk chunk_;
};

class Pool {
public:
    void Init(std::size_t pageSize, std::size_t blockSize) {
        blockSize_ = blockSize;
        std::size_t tmpNum = pageSize / blockSize;
        blockNum_ = tmpNum > MAX_BLOCK_NUM ? MAX_BLOCK_NUM : tmpNum;
        head = new ChunkList(); //
    }
    // operator new不能满足内存分配请求时，new-handler函数不只调用一次，而是不断重复，直至找到足够的内存。实现重复调用的代码在条款8里可以看到，这里我用描述性的的语言来说明：一个设计得好的new-handler函数必须实现下面功能中的一种。
    // * 产生更多的可用内存。这将使operator new下一次分配内存的尝试有可能获得成功。实施这一策略的一个方法是：在程序启动时分配一个大的内存块，然后在第一次调用new-handler时释放。释放时伴随着一些对用户的警告信息，如内存数量太少，下次请求可能会失败，除非又有更多的可用空间。
    // * 安装另一个不同的new-handler函数。如果当前的new-handler函数不能产生更多的可用内存，可能它会知道另一个new-handler函数可以提供更多的资源。这样的话，当前的new-handler可以安装另一个new-handler来取代它(通过调用set_new_handler)。下一次operator new调用new-handler时，会使用最近安装的那个。(这一策略的另一个变通办法是让new-handler可以改变它自己的运行行为，那么下次调用时，它将做不同的事。方法是使new-handler可以修改那些影响它自身行为的静态或全局数据。)
    // * 卸除new-handler。也就是传递空指针给set_new_handler。没有安装new-handler，operator new分配内存不成功时就会抛出一个标准的std::bad_alloc类型的异常。
    // * 抛出std::bad_alloc或从std::bad_alloc继承的其他类型的异常。这样的异常不会被operator new捕捉，所以它们会被送到最初进行内存请求的地方。(抛出别的不同类型的异常会违反operator new异常规范。规范中的缺省行为是调用abort，所以new-handler要抛出一个异常时，一定要确信它是从std::bad_alloc继承来的。想更多地了解异常规范)
    // * 没有返回。典型做法是调用abort或exit。abort/exit可以在标准c库中找到(还有标准c++库)。

    // ***
    // The sharing of knowledge, the spirit of encouragement.
    // By Joel Jiang (江东敏)
    unsigned char* Allocate() {
        if (allocChunk_ && allocChunk_->IsAvailable()) {
            return allocChunk_->Allocate(blockSize_);
        }

        if (deferChunk_ != nullptr) {
            allocChunk_ = deferChunk_;
            deferChunk_ = nullptr;
            return allocChunk_->Allocate(blockSize_);
        }

        ChunkList* p = head->FindAvailableChunk();
        if (p != nullptr) {
            allocChunk_ = p;
            return allocChunk_->Allocate(blockSize_);
        }
        p = new ChunkList(blockNum_, blockSize_);
        head->InsertAtTail(p);
        allocChunk_ = p;
        return allocChunk_->Allocate(blockSize_);
    }

    void Deallocate(unsigned char* ptr, ChunkList* chunk) {
        if (chunk == nullptr) {
            return;
        }
        deallocChunk_ = chunk;
        deallocChunk_->Deallocate(ptr, blockSize_);
        FreeChunk(ptr);
    }

    ChunkList* HasBlock(unsigned char* ptr) {
        const std::size_t chunkSize = blockNum_ * blockSize_;
        return head->FindLocatedChunk(ptr, chunkSize);
    }

private:
    void FreeChunk(unsigned char* ptr) {
        // 判断是否是全回收, 保证有两块全回收时才真正释放延迟一块的内存
        if (deallocChunk_->IsAllBlockFree(blockNum_)) {
            if (deferChunk_ != nullptr) {
                deferChunk_->Deallocate(ptr, blockSize_);
                head->Remove(deferChunk_);
                delete deferChunk_;
            }
            deferChunk_ = deallocChunk_;
            deallocChunk_ = nullptr;
        }
    }

private:
    unsigned char blockNum_{0};
    std::size_t blockSize_{0};

    ChunkList* head{nullptr};
    ChunkList* allocChunk_{nullptr};
    ChunkList* deallocChunk_{nullptr};
    ChunkList* deferChunk_{nullptr};
    static constexpr unsigned char MAX_BLOCK_NUM = static_cast<unsigned char>(-1);
};

class MyAllocator {
public:
    static MyAllocator* GetInstance() {
        static MyAllocator* alloc = new MyAllocator();
        return alloc;
    }
    unsigned char* Allocate(std::size_t bytes) {
        if (bytes > BLOCK_SIZE) {
            return static_cast<unsigned char*>(::operator new(bytes));
        }

        std::size_t loc = bytes - 1; // RoundUp(bytes) / ALIGN_SIZE - 1;
        Pool& pool = pools_[loc];
        return pool.Allocate();
    }

    void Deallocate(unsigned char* ptr) {
        if (ptr == nullptr) {
            return;
        }

        ChunkList* chunk = nullptr;
        std::size_t i = 0;
        for (; i < POOL_SIZE; ++i) {
            chunk = pools_[i].HasBlock(ptr);
            if (chunk != nullptr) {
                pools_[i].Deallocate(ptr, chunk);
                return;
            }
        }
        ::operator delete(ptr);
    }

private:
    MyAllocator() {
        for (std::size_t i = 0; i < POOL_SIZE; i++) {
            pools_[i].Init(CHUNK_SIZE, (i + 1) * ALIGN_SIZE);
        }
    }
    ~MyAllocator() = default;
    // 将任何小额区块的内存需求量上调至与区块大小对齐
    static std::size_t RoundUp(std::size_t bytes) {
        return (((bytes) + ALIGN_SIZE - 1) & ~(ALIGN_SIZE - 1));
    }

private:
    static constexpr std::size_t ALIGN_SIZE = 1;
    static constexpr std::size_t BLOCK_SIZE = 256;
    static constexpr std::size_t CHUNK_SIZE = 4096;
    static constexpr std::size_t POOL_SIZE = BLOCK_SIZE / ALIGN_SIZE;
    Pool pools_[POOL_SIZE];
};

} // namespace mystl

#endif // MYSTL_MY_ALLOCATOR_H_
