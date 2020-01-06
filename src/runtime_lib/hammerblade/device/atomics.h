#pragma once
#include <bsg_manycore.h>
#include <bsg_set_tile_x_y.h>
#include <bsg_manycore_atomic.h>
#ifdef __cplusplus
extern "C" {
#endif
//method to atomically update a value in global mem
static inline void atomic_update_val(int * ptr, int pos, int val, int * lock)
{
  int result = bsg_amoswap_aq(lock, bsg_id); //set lock to bsg_id
  ptr[pos] = val;
  result = bsg_amoswap_rl(lock, bsg_id);
}
#ifdef __cplusplus
}
#endif
