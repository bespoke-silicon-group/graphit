//NOTE: this file added here as reference, but can't be compiled from this directory

#include "bsg_manycore.h"
#include "bsg_set_tile_x_y.h"
#define BSG_TILE_GROUP_X_DIM bsg_tiles_X
#define BSG_TILE_GROUP_Y_DIM bsg_tiles_Y
#include "bsg_tile_group_barrier.h"
INIT_TILE_GROUP_BARRIER(r_barrier, c_barrier, 0, bsg_tiles_X-1, 0, bsg_tiles_Y-1);


void __attribute__ ((noinline)) parent_generated_vector_op_apply_func_0(int v, int *parent) 
{
  parent[v] =  -(1) ;
}

int __attribute__ ((noinline)) 
kernel_init_parent(int *parent, int V, int E, int block_size_x) {
    int start_x = block_size_x * (__bsg_tile_group_id_y * __bsg_grid_dim_x + __bsg_tile_group_id_x); 
    for (int iter_x = __bsg_id; iter_x < block_size_x; iter_x += bsg_tiles_X * bsg_tiles_Y) {
        if ((start_x + iter_x) < V) {
	    parent_generated_vector_op_apply_func_0(start_x + iter_x, parent);
        }
	else {
            break;
        }
    }
    bsg_tile_group_barrier(&r_barrier, &c_barrier);
    return 0;
}


int __attribute__ ((noinline)) updateEdge(int src, int dst, int *parent) 
{
  int output1 ;
  parent[dst] = src;
  output1 = 1;
  return output1;
}

int __attribute__ ((noinline)) toFilter(int v, int *parent) 
{
  int output ;
  output = (parent[v]) == ( -(1) );
  return output;
}


int __attribute__ ((noinline)) 
kernel_edgeset_apply_push_serial_from_vertexset_to_filter_func_with_frontier(int *out_indices, int *out_neighbors, int *frontier, int *next_frontier, int *parent, int V, int E, int block_size_x) {
    int start_x = block_size_x * (__bsg_tile_group_id_y * __bsg_grid_dim_x + __bsg_tile_group_id_x); 
	for (int iter_x = __bsg_id; iter_x < block_size_x; iter_x += bsg_tiles_X * bsg_tiles_Y) {
                if ((start_x + iter_x) < V-1) {  //in bounds (and not last)
                   if(frontier[start_x + iter_x]) { //in frontier
                      for(int iter_n = out_indices[start_x + iter_x]; iter_n < out_indices[start_x + iter_x + 1]; iter_n++) {
                          if(toFilter(out_neighbors[iter_n], parent)) {
                              if(updateEdge((start_x + iter_x), out_neighbors[iter_n], parent)) {
                                 next_frontier[out_neighbors[iter_n]] = 1;
                              }
                          }
                      }
                   }
                }
                else if ((start_x + iter_x) == V-1) { 
                   if(frontier[start_x + iter_x]) { //in frontier
                      for(int iter_n = out_indices[start_x + iter_x]; iter_n < E; iter_n++) {
                          if(toFilter(out_neighbors[iter_n], parent)) {
                              if(updateEdge((start_x + iter_x), out_neighbors[iter_n], parent)) {
                                 next_frontier[out_neighbors[iter_n]] = 1;
                              }
                          }
                      }
                   }
                }
                else {
                   break;
                }
          }
          bsg_tile_group_barrier(&r_barrier, &c_barrier); 

          return 0;
}


