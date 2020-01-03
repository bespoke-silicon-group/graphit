#pragma once
#include <bsg_manycore.h>
#include <bsg_set_tile_x_y.h>
#ifdef __cplusplus
extern "C" {
#endif
static inline void local_range(int n, int *start, int *end)
{
        int sz = n / (bsg_tiles_X * bsg_tiles_Y);
        *start = bsg_id * sz;
        *end = *start + sz;
        *end = *end < n ? *end : n;
}
#ifdef __cplusplus
}
#endif
