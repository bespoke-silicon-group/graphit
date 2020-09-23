#pragma once
#include <bsg_manycore.h>
#include <bsg_set_tile_x_y.h>
#include <bsg_manycore_atomic.h>

#ifdef __cplusplus
extern "C" {
#endif
bool compare_and_swap(int &x, const int &old_val, const int &new_val) {
    if(x != old_val) {
        return false;
    }
    int result = bsg_amoswap(&x, new_val);
    if(result != old_val) {
        return false;
    }
    return true;
}
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

int fetch_and_add(int &x, int inc) {
    int orig_val = x;
    int result = bsg_amoswap(&x, (orig_val + inc));
    //TODO(Emily): trying a recursive call instead
    //to try to acquire the lock again if it fails
    if(result != orig_val){
        return fetch_and_add(&x, inc);
    }
    return orig_val;
}

#ifdef __cplusplus
}
#endif
