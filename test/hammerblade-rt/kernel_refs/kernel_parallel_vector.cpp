#include "bsg_manycore.h"
#include "bsg_set_tile_x_y.h"


#define BSG_TILE_GROUP_X_DIM bsg_tiles_X
#define BSG_TILE_GROUP_Y_DIM bsg_tiles_Y
#include "bsg_tile_group_barrier.h"
INIT_TILE_GROUP_BARRIER(r_barrier, c_barrier, 0, bsg_tiles_X-1, 0, bsg_tiles_Y-1);


extern "C" int __attribute__ ((noinline)) kernel_template_func(int * test_vec, int len, int block_size_x) {

  int start_x = block_size_x * (__bsg_tile_group_id_y * __bsg_grid_dim_x + __bsg_tile_group_id_x);
  for (int iter_x = __bsg_id; iter_x < block_size_x; iter_x += bsg_tiles_X * bsg_tiles_Y) {
    if ((start_x + iter_x) < len) {
      test_vec[__bsg_id * threads + iter_x] = iter_x;
    }
  }
  bsg_tile_group_barrier(&r_barrier, &c_barrier);
  return 0;
}
