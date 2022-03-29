#ifndef MYSTL_MY_ALLOCATOR_H_
#define MYSTL_MY_ALLOCATOR_H_

#include "alloc.h"
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

        auto p = pBlocks_;
        for (unsigned char i = 1; i <= blockNum; ++i) {
            *p = i;
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

class ChunkPool {
public:
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

    void Deallocate(unsigned char* ptr) {
        const std::size_t chunkSize = blockNum_ * blockSize_;
        Chunk* p = head.next;
        while (p != nullptr) {
            if (p->IsInside(ptr, chunkSize)) {
                deallocChunk_ = p;
                break;
            }
            p = p->StepNext();
        }
        if (p == nullptr) {
            return;
        }
        deallocChunk_->Deallocate(ptr, blockSize_);
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
    unsigned char blockNum_;
    std::size_t blockSize_;

    Chunk* allocChunk_;
    Chunk* deallocChunk_;
    Chunk* deferChunk_;
    struct ChunkHead {
        Chunk* next;
        Chunk* prev;
    };
    ChunkHead head;
};

template <typename T>
class MyAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    static pointer Allocate(size_type n) {
        return static_cast<pointer>(::operator new(n * sizeof(T)));
    }

    static void deallocate(pointer ptr) {
        ::operator delete(ptr);
    }

    static void deallocate(pointer ptr, size_type /*size*/) {
        ::operator delete(ptr);
    }

    static void construct(pointer ptr, const T& value) {
        MyStl::construct<T>(ptr, value);
    }

    static void destroy(pointer ptr) {
        MyStl::destroy<T>(ptr);
    }

    static void destroy(pointer first, pointer last) {
        MyStl::destroy<T>(first, last);
    }
};
} // namespace MyStl

#endif // MYSTL_MY_ALLOCATOR_H_
