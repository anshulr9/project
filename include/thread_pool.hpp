#pragma once

#include "spsc_queue.hpp"
#include "pinned_thread.hpp"

#include <functional>
#include <vector>
#include <atomic>
#include <cstddef>

class ThreadPool {
public:
    using Task = std::function<void()>;

    explicit ThreadPool(size_t num_threads);
    ~ThreadPool();

    bool submit(Task task);

private:
    void worker_loop(size_t worker_id);

    std::vector<PinnedThread> workers_;
    std::vector<SPSCQueue<Task>> queues_;

    std::atomic<bool> running_;
    std::atomic<size_t> next_queue_;
};
