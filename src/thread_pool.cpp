#include "thread_pool.hpp"

ThreadPool::ThreadPool(size_t num_threads)
    : running_(true), next_queue_(0) {

    queues_.reserve(num_threads);
    workers_.reserve(num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
        queues_.emplace_back(1024);
    }

    for (size_t i = 0; i < num_threads; ++i) {
        workers_.emplace_back(
            static_cast<int>(i),
            [this, i]() { worker_loop(i); }
        );
    }
}

ThreadPool::~ThreadPool() {
    running_.store(false, std::memory_order_relaxed);
    for (auto& w : workers_) {
        w.join();
    }
}

bool ThreadPool::submit(Task task) {
    size_t idx = next_queue_.fetch_add(1, std::memory_order_relaxed);
    return queues_[idx % queues_.size()].enqueue(task);
}

void ThreadPool::worker_loop(size_t worker_id) {
    auto& queue = queues_[worker_id];
    Task task;

    while (running_.load(std::memory_order_relaxed)) {
        if (queue.dequeue(task)) {
            task();
        }
    }
}
