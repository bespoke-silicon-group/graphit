#pragma once
#include <bsg_manycore.h>
#include <bsg_set_tile_x_y.h>
#include <bsg_tile_group_barrier.h>
INIT_TILE_GROUP_BARRIER(r_barrier, c_barrier, 0, bsg_tiles_X-1, 0, bsg_tiles_Y-1);

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

void recursive_range(int n, int e, int grain size, int * start_idx, int * end_idx, int * edge_index)
{
  //TODO(thread idx = 0 does the work to assign all the ranges)


}

#ifdef __cplusplus
extern "C" {
#endif
static inline void edge_aware_local_range(int n, int e, int *start, int *end, int *edge_index)
{
        //TODO(Emily): need to implement this function to distribute work based on grain size
        int grain_size = e / (bsg_tiles_X * bsg_tiles_Y);
        int start_idx[bsg_tiles_X * bsg_tiles_Y] = {0};
        int end_idx[bsg_tiles_X * bsg_tiles_Y] = {0};
        if(bsg_id == 0) {
          recursive_range(n, e, grain_size, start_idx, end_idx, edge_index);
        }
        //barrier here?
        bsg_tile_group_barrier(&r_barrier, &c_barrier);
        *start = start_idx[bsg_id];
        *end = end_idx[bsg_id];
}
#ifdef __cplusplus
}
#endif
