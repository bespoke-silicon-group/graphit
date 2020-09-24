#pragma once
#include <bsg_manycore.h>
#include <bsg_set_tile_x_y.h>
#include <bsg_manycore_atomic.h>
#include <atomic>

template <typename T>
class lockable {
private:
    atomic<int> lock;
    T value;
void acquire_lock() {
    int l = 0;
    do { l = lock.exchange(0, std::memory_order_acquire); } while (l == 0);
};
void release_lock() {
    lock.store(1, std::memory_order_release);
}
bool cas(T t, T v) {
    acquire_lock();
    T x = value;
    bool r = false;
    if (x == t) {
        r = true;
        value = v;
    }
    release_lock();
    return r;
}
};

template <typename T>
bool compare_and_swap(lockable<T> &x, const T &old_val, const T &new_val) {
    return x.cas(old_val, new_val);
}

//TODO(Emily): implement & test
template <typename T>
int fetch_and_add(lockable<T> &x, T inc) {
    bsg_fail(); //not yet implemented, so fails if called
}
