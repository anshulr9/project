#pragma once

#include <thread>

class PinnedThread {
public:
    template <typename Fn>
    PinnedThread(int cpu_id, Fn&& fn);

    void join();

private:
    std::thread thread_;
};
