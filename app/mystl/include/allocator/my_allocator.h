#ifndef MYSTL_MY_ALLOCATOR_H_
#define MYSTL_MY_ALLOCATOR_H_

#include "construct.h"

namespace MyStl {
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
    unsigned char firstAvailableBlock_;
    unsigned char blocksAvailable_;
    unsigned char* pBlocks_;
};

// 双向链表保存Chunk
class ChunkList {
public:
    ChunkList(unsigned char blockNum = 0U, std::size_t blockSize = 0U) :
        next(this), prev(this), chunk(blockNum, blockSize) {
    }

    unsigned char* Allocate(std::size_t blockSize) {
        return chunk.Allocate(blockSize);
    }

    void Deallocate(unsigned char* ptr, std::size_t blockSize) {
        chunk.Deallocate(ptr, blockSize);
    }

    bool IsAvailable() {
        return chunk.IsAvailable();
    }

    bool IsAllBlockFree(unsigned char blockNum) {
        return chunk.IsAllBlockFree(blockNum);
    }

    void InsertAtTail(ChunkList* p) {
        p->next = this;
        p->prev = prev;
        prev->next = p;
        prev = p;
    }

    void Remove(ChunkList* p) {
        p->prev->next = p->next;
        p->next->prev = p->prev;
    }

    ChunkList* FindAvailableChunk() {
        ChunkList* p = this->next;
        while (p != this) {
            if (p->chunk.IsAvailable()) {
                return this;
            }
            p = p->next;
        }
        return nullptr;
    }

    ChunkList* FindLocatedChunk(unsigned char* ptr, std::size_t chunkSize) {
        ChunkList* p = this->next;
        while (p != this) {
            if (p->chunk.IsInside(ptr, chunkSize)) {
                return this;
            }
            p = p->next;
        }
        return nullptr;
    }

private:
    ChunkList* next{nullptr};
    ChunkList* prev{nullptr};
    Chunk chunk;
};

class Pool {
public:
    void Init(std::size_t pageSize, std::size_t blockSize) {
        blockSize_ = blockSize;
        std::size_t tmpNum = pageSize / blockSize;
        blockNum_ = tmpNum > MAX_BLOCK_NUM ? MAX_BLOCK_NUM : tmpNum;
        head = new ChunkList();
    }

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

        std::size_t loc = RoundUp(bytes) / ALIGN_SIZE - 1;
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

} // namespace MyStl

#endif // MYSTL_MY_ALLOCATOR_H_
