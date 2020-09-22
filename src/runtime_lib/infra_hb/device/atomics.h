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
