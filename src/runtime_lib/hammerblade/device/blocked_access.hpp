#ifndef _BLOCKED_ACCESS_HPP_
#define _BLOCKED_ACCESS_HPP_
#include <cstdint>

int number_of_blocks(int len, int block_sz) {
  return (len % block_sz) == 0 ? (len / block_sz) : ((len / block_sz) + 1);
}

int block_of(int offset, int block_sz) {
  return offset / block_sz;
}

bool check_access(int offset, int block_idx, int block_sz) {
  if(block_of(offset, block_sz) != block_idx) {
    return false;
  }
  return true;
}

#endif
