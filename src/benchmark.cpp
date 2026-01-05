#include "thread_pool.hpp"

#include <chrono>
#include <iostream>
#include <atomic>
#include <vector>

using Clock = std::chrono::steady_clock;

int main() {
    constexpr size_t num_threads = 4;
    constexpr size_t num_tasks = 1'000'000;

    ThreadPool pool(num_threads);

    std::atomic<size_t> completed{0};
    std::vector<std::chrono::nanoseconds> latencies;
    latencies.reserve(num_tasks);

    auto start = Clock::now();

    for (size_t i = 0; i < num_tasks; ++i) {
        auto enqueue_time = Clock::now();

        while (!pool.submit([&, enqueue_time]() {
            auto exec_time = Clock::now();
            latencies.push_back(exec_time - enqueue_time);
            completed.fetch_add(1, std::memory_order_relaxed);
        })) {
            // retry
        }
    }

    while (completed.load(std::memory_order_relaxed) < num_tasks) {
        // spin
    }

    auto end = Clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);

    double throughput = num_tasks / duration.count();

    std::chrono::nanoseconds total_latency{0};
    for (auto& l : latencies) {
        total_latency += l;
    }

    auto avg_latency = total_latency / latencies.size();

    std::cout << "Threads: " << num_threads << "\n";
    std::cout << "Tasks: " << num_tasks << "\n";
    std::cout << "Total time (s): " << duration.count() << "\n";
    std::cout << "Throughput (tasks/s): " << throughput << "\n";
    std::cout << "Average latency (ns): " << avg_latency.count() << "\n";
}
