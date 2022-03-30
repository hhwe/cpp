#ifndef MYSTL_MY_ALLOCATOR_H_
#define MYSTL_MY_ALLOCATOR_H_

#include "construct.h"

namespace MyStl {
class Chunk {
public:
    Chunk(unsigned char blockNum, std::size_t blockSize) :
        next(nullptr), prev(nullptr) {
        pBlocks_ = new unsigned char[blockNum * blockSize];
        Reset(blockNum, blockSize);
    }
    ~Chunk() {
        delete pBlocks_;
    }

    Chunk* StepNext() {
        return next;
    }

    void AddToTail(Chunk* tail) {
        prev = tail;
        tail->next = this;
        next = nullptr;
    }

    void DeleteFromList() {
        prev->next = next;
        next->prev = prev;
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

    void Deallocate(unsigned char* p, std::size_t blockSize) {
        *p = firstAvailableBlock_;
        firstAvailableBlock_ = (p - pBlocks_) / blockSize;
        ++blocksAvailable_;
    }

    bool IsAvailable() {
        return blocksAvailable_ > 0U;
    }

    bool IsInside(unsigned char* p, std::size_t chunkSize) {
        return p >= pBlocks_ && p < pBlocks_ + chunkSize;
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
    Chunk* next;
    Chunk* prev;
};

class Pool {
public:
    void Init(std::size_t pageSize, std::size_t blockSize) {
        blockSize_ = blockSize;
        std::size_t tmpNum = pageSize / blockSize;
        blockNum_ = tmpNum > MAX_BLOCK_NUM ? MAX_BLOCK_NUM : tmpNum;
    }

    unsigned char* Allocate() {
        if (allocChunk_ && allocChunk_->IsAvailable()) {
            return allocChunk_->Allocate(blockSize_);
        }

        Chunk* p = head.next;
        while (p != nullptr) {
            if (p->IsAvailable()) {
                allocChunk_ = p;
                return allocChunk_->Allocate(blockSize_);
            }
            p = p->StepNext();
        }
        p = new Chunk(blockNum_, blockSize_);
        if (head.prev != nullptr) {
            p->AddToTail(head.prev);
        } else {
            head.next = p;
            head.prev = p;
        }
        allocChunk_ = p;
        return allocChunk_->Allocate(blockSize_);
    }

    void Deallocate(unsigned char* ptr, Chunk* chunk) {
        if (chunk == nullptr) {
            return;
        }
        deallocChunk_ = chunk;
        deallocChunk_->Deallocate(ptr, blockSize_);
        FreeChunk(ptr);
    }

    Chunk* HasBlock(unsigned char* ptr) {
        const std::size_t chunkSize = blockNum_ * blockSize_;
        Chunk* p = head.next;
        while (p != nullptr) {
            if (p->IsInside(ptr, chunkSize)) {
                break;
            }
            p = p->StepNext();
        }
        return p;
    }

private:
    void FreeChunk(unsigned char* ptr) {
        // 判断是否是全回收, 保证有两块全回收时才真正释放延迟一块的内存
        if (deallocChunk_->IsAllBlockFree(blockNum_)) {
            if (deferChunk_ != nullptr) {
                deferChunk_->Deallocate(ptr, blockSize_);
                deferChunk_->DeleteFromList();
                delete deferChunk_;
            }
            deferChunk_ = deallocChunk_;
            deallocChunk_ = nullptr;
        }
    }

private:
    unsigned char blockNum_{0};
    std::size_t blockSize_{0};

    Chunk* allocChunk_{nullptr};
    Chunk* deallocChunk_{nullptr};
    Chunk* deferChunk_{nullptr};
    struct ChunkHead {
        Chunk* next;
        Chunk* prev;
    };
    ChunkHead head{nullptr, nullptr};

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

        Chunk* chunk = nullptr;
        std::size_t i = 0;
        for (; i < POOL_SIZE; ++i) {
            chunk = pools_[i].HasBlock(ptr);
            if (chunk == nullptr) {
                break;
            }
        }
        if (i == POOL_SIZE) {
            ::operator delete(ptr);
            return;
        }
        pools_[i].Deallocate(ptr, chunk);
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
