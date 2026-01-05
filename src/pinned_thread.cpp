#include "pinned_thread.hpp"

#include <pthread.h>
#include <sched.h>
#include <cassert>

void pin_thread_to_cpu(std::thread& t, int cpu_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);

    int rc = pthread_setaffinity_np(
        t.native_handle(),
        sizeof(cpu_set_t),
        &cpuset
    );

    assert(rc == 0 && "pthread_setaffinity_np failed");
}

template <typename Fn>
PinnedThread::PinnedThread(int cpu_id, Fn&& fn)
    : thread_(std::forward<Fn>(fn)) {
    pin_thread_to_cpu(thread_, cpu_id);
}

// Explicit instantiation is NOT needed because constructor is used inline
