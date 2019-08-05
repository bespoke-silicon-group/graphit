#include "bsg_manycore.h"
#include "bsg_set_tile_x_y.h"
#define BSG_TILE_GROUP_X_DIM bsg_tiles_X
#define BSG_TILE_GROUP_Y_DIM bsg_tiles_Y
#include "bsg_tile_group_barrier.h"
INIT_TILE_GROUP_BARRIER(r_barrier, c_barrier, 0, bsg_tiles_X-1, 0, bsg_tiles_Y-1);

void __attribute__ ((noinline)) r_p_generated_vector_op_apply_func_2(int v, float * r_p)
{
  r_p[v] = 0;
}

void __attribute__ ((noinline)) r_generated_vector_op_apply_func_1(int v, float * r)
{
  r[v] = 0;
}

void __attribute__ ((noinline)) p_generated_vector_op_apply_func_0(int v, float * p)
{
  p[v] = 0;
}

int __attribute__ ((noinline))
kernel_init_p(float *p, int V, int E, int block_size_x) {
    int start_x = block_size_x * (__bsg_tile_group_id_y * __bsg_grid_dim_x + __bsg_tile_group_id_x);
    for (int iter_x = __bsg_id; iter_x < block_size_x; iter_x += bsg_tiles_X * bsg_tiles_Y) {
        if ((start_x + iter_x) < V) {
	    p_generated_vector_op_apply_func_0(start_x + iter_x, p);
        }
	else {
            break;
        }
    }
    bsg_tile_group_barrier(&r_barrier, &c_barrier);
    return 0;
}

int __attribute__ ((noinline))
kernel_init_r(float *r, int V, int E, int block_size_x) {
    int start_x = block_size_x * (__bsg_tile_group_id_y * __bsg_grid_dim_x + __bsg_tile_group_id_x);
    for (int iter_x = __bsg_id; iter_x < block_size_x; iter_x += bsg_tiles_X * bsg_tiles_Y) {
        if ((start_x + iter_x) < V) {
	    r_generated_vector_op_apply_func_0(start_x + iter_x, r);
        }
	else {
            break;
        }
    }
    bsg_tile_group_barrier(&r_barrier, &c_barrier);
    return 0;
}

int __attribute__ ((noinline))
kernel_init_r_p(float *r_p, int V, int E, int block_size_x) {
    int start_x = block_size_x * (__bsg_tile_group_id_y * __bsg_grid_dim_x + __bsg_tile_group_id_x);
    for (int iter_x = __bsg_id; iter_x < block_size_x; iter_x += bsg_tiles_X * bsg_tiles_Y) {
        if ((start_x + iter_x) < V) {
	    r_p_generated_vector_op_apply_func_2(start_x + iter_x, r_p);
        }
	else {
            break;
        }
    }
    bsg_tile_group_barrier(&r_barrier, &c_barrier);
    return 0;
}

int __attribute__ ((noinline))
kernel_init_out_degree(int *out, int * out_indices, int V, int E, int block_size_x) {
    int start_x = block_size_x * (__bsg_tile_group_id_y * __bsg_grid_dim_x + __bsg_tile_group_id_x);
    for (int iter_x = __bsg_id; iter_x < block_size_x; iter_x += bsg_tiles_X * bsg_tiles_Y) {
        if ((start_x + iter_x) < V - 1) {
	        out[start_x + iter_x] = out_indices[start_x + iter_x + 1] - out_indices[start_x + iter_x];
        }
        else if ((start_x + iter_x) == V - 1) {
          out[start_x + iter_x] = E - out_indices[start_x + iter_x];
        }
	      else {
            break;
        }
    }
    bsg_tile_group_barrier(&r_barrier, &c_barrier);
    return 0;
}

void __attribute__ ((noinline)) updateNeigh(int src, int dst, float alpha,
                                            float * r, float * r_p, int * out_degree)
{
  r_p[dst] += (((((1)  - alpha) / ((1)  + alpha)) * r[src]) / out_degree[src]);
}

int __attribute__ ((noinline)) kernel_update_neigh(int * frontier, int *out_indices, int *out_neighbors,
                                            float alpha, float * r, float * r_p, int * out_degree
                                            int V, int E, int block_size_x)
{
  int start_x = block_size_x * (__bsg_tile_group_id_y * __bsg_grid_dim_x + __bsg_tile_group_id_x);
  for (int iter_x = __bsg_id; iter_x < block_size_x; iter_x += bsg_tiles_X * bsg_tiles_Y) {
    if ((start_x + iter_x) < V-1) {
      if(frontier[start_x + iter_x]) { //in frontier
         for(int iter_n = out_indices[start_x + iter_x]; iter_n < out_indices[start_x + iter_x + 1]; iter_n++) {
           updateNeigh((start_x + iter_x), out_neighbors[iter_n], alpha, r, r_p, out_degree);
         }
       }
     }
     else if((start_x + iter_x) == V-1) {
        if(frontier[start_x + iter_x]) { //in frontier
           for(int iter_n = out_indices[start_x + iter_x]; iter_n < E; iter_n++) {
             updateNeigh((start_x + iter_x), out_neighbors[iter_n], alpha, r, r_p, out_degree);
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

void __attribute__ ((noinline)) updateSelf(int v, float alpha, float * p,
                                          float * r, float * r_p)
{
  p[v] = (p[v] + ((((2)  * alpha) / ((1)  + alpha)) * r[v]));
  r_p[v] = ((float) 0) ;
}

int __attribute__ ((noinline))
kernel_update_self(float *p, float *r, float * r_p, float alpha, int * frontier, int V, int E, int block_size_x) {
    int start_x = block_size_x * (__bsg_tile_group_id_y * __bsg_grid_dim_x + __bsg_tile_group_id_x);
    for (int iter_x = __bsg_id; iter_x < block_size_x; iter_x += bsg_tiles_X * bsg_tiles_Y) {
        if ((start_x + iter_x) < V) {
          if(frontier[start_x + iter_x]){ //in frontier
	           updateSelf(start_x + iter_x, alpha, p, r, r_p);
          }
        }
	else {
            break;
        }
    }
    bsg_tile_group_barrier(&r_barrier, &c_barrier);
    return 0;
}

int __attribute__ ((noinline)) addToFrontier(int v, float epsilon, float * r, int * out_degree)
{
  int output ;
  output = ((r[v]) >= ((out_degree[v] * epsilon)) && (out_degree[v]) > ((0) ));
  return output;
}

int __attribute__ ((noinline)) kernel_build_next_frontier(int * next_frontier, float *r, int * out_degree, float epsilon, int V, int E, int block_size_x)
{
  int start_x = block_size_x * (__bsg_tile_group_id_y * __bsg_grid_dim_x + __bsg_tile_group_id_x);
  for (int iter_x = __bsg_id; iter_x < block_size_x; iter_x += bsg_tiles_X * bsg_tiles_Y) {
      if ((start_x + iter_x) < V) {
    next_frontier[start_x + iter_x] = addToFrontier(start_x + iter_x, epsilon, r, out_degree);
      }
else {
          break;
      }
  }
  bsg_tile_group_barrier(&r_barrier, &c_barrier);
  return 0;
}

void __attribute__ ((noinline)) copyRtoRP(int v, float * r, float * r_p)
{
  float old_score = r[v];
  r_p[v] = old_score;
}

int __attribute__ ((noinline))
kernel_copy_r_to_rp(float *r, float * r_p, int V, int E, int block_size_x) {
    int start_x = block_size_x * (__bsg_tile_group_id_y * __bsg_grid_dim_x + __bsg_tile_group_id_x);
    for (int iter_x = __bsg_id; iter_x < block_size_x; iter_x += bsg_tiles_X * bsg_tiles_Y) {
        if ((start_x + iter_x) < V) {
	    copyRtoRP(start_x + iter_x, r, r_p);
        }
	else {
            break;
        }
    }
    bsg_tile_group_barrier(&r_barrier, &c_barrier);
    return 0;
}

void __attribute__ ((noinline)) copyRPtoR(int v, float * r, float * r_p)
{
  float old_score = r_p[v];
  r[v] = old_score;
}

int __attribute__ ((noinline))
kernel_copy_rp_to_r(float *r, float * r_p, int V, int E, int block_size_x) {
    int start_x = block_size_x * (__bsg_tile_group_id_y * __bsg_grid_dim_x + __bsg_tile_group_id_x);
    for (int iter_x = __bsg_id; iter_x < block_size_x; iter_x += bsg_tiles_X * bsg_tiles_Y) {
        if ((start_x + iter_x) < V) {
	    copyRPtoR(start_x + iter_x, r, r_p);
        }
	else {
            break;
        }
    }
    bsg_tile_group_barrier(&r_barrier, &c_barrier);
    return 0;
}
