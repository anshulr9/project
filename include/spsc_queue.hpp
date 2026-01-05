#pragma once

#include <atomic>
#include <cstddef>
#include <vector>

template <typename T>
class SPSCQueue {
public:
    explicit SPSCQueue(size_t capacity)
        : capacity_(capacity),
          buffer_(capacity),
          head_(0),
          tail_(0) {}

    bool enqueue(const T& item) {
        const size_t tail = tail_.load(std::memory_order_relaxed);
        const size_t next_tail = (tail + 1) % capacity_;

        if (next_tail == head_.load(std::memory_order_acquire)) {
            return false; // queue full
        }

        buffer_[tail] = item;
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }

    bool dequeue(T& item) {
        const size_t head = head_.load(std::memory_order_relaxed);

        if (head == tail_.load(std::memory_order_acquire)) {
            return false; // queue empty
        }

        item = buffer_[head];
        head_.store((head + 1) % capacity_, std::memory_order_release);
        return true;
    }

private:
    const size_t capacity_;
    std::vector<T> buffer_;

    std::atomic<size_t> head_;
    std::atomic<size_t> tail_;
};
