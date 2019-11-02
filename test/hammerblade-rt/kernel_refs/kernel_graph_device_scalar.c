//This kernel modifies 2 global scalar variables 

#include "bsg_manycore.h"
#include "bsg_set_tile_x_y.h"

#define BSG_TILE_GROUP_X_DIM bsg_tiles_X
#define BSG_TILE_GROUP_Y_DIM bsg_tiles_Y
#include "bsg_tile_group_barrier.h"
INIT_TILE_GROUP_BARRIER(r_barrier, c_barrier, 0, bsg_tiles_X-1, 0, bsg_tiles_Y-1);

__attribute__((section(".dram"))) int * parent;
__attribute__((section(".dram"))) float epsilon;

int  __attribute__ ((noinline)) kernel_graph_device_scalar(int V, int block_size_x) {

        int start_x = block_size_x * (__bsg_tile_group_id_y * __bsg_grid_dim_x + __bsg_tile_group_id_x);
        for (int iter_x = __bsg_id; iter_x < block_size_x; iter_x += bsg_tiles_X * bsg_tiles_Y) {
                if(start_x + iter_x < V){
                  parent[start_x + iter_x] = start_x + iter_x;
                }
                else{
                  break;
                }
        }
        epsilon = 1;

        bsg_tile_group_barrier(&r_barrier, &c_barrier);

  return 0;
}