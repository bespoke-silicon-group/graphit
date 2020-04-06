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

#ifdef __cplusplus
extern "C" {
#endif
static inline void edge_aware_local_range(int n, int e, int grain_size, int *start, int *end, int *edge_index)
{
        //TODO(Emily): need to implement this function to distribute work based on grain size
        int sz = n / (bsg_tiles_X * bsg_tiles_Y);
        *start = bsg_id * sz;
        *end = *start + sz;
        *end = *end < n ? *end : n;
}
#ifdef __cplusplus
}
#endif
