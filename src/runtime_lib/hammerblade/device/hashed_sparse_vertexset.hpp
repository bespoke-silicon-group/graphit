#pragma once
#include <bsg_manycore_errno.h>
#include <bsg_manycore.h>
#include <bsg_manycore_loader.h>
#include <bsg_manycore_cuda.h>
#include <hammerblade/host/error.hpp>

class HashedSparseVertexset {
public:
  //n = #cores, m = #vertices in simplest implementation
  HashedSparseVertexset(int n, int m)
  {
    //init the index array to 0s
    int temp_ind[n] = {0};
    indices = temp_ind;
    //init the sparse datastructure w/ -1s
    int temp_data[n][m];
    for(int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        temp_data[i][j] = -1;
      }
    }
    //sparse_vertexset = temp_data;
    for(int i = 0; i < n; i++) {
      sparse_vertexset[i] = temp_data[i];
    }

  }

  //TODO(Emily): eventually we will want a more sophisticated hashing
  int hash_idx(int idx) {
    return idx;
  }

  //TODO(Emily): need to implement locking mechanisms for safe writes to data
  bool aquire_lock(int array_idx) {
    return true;
  }

  void release_lock(int array_idx) {

  }

  void insert_element(int idx, int val) {
    int array_idx = hash_idx(idx);

    if(aquire_lock(array_idx))
    {
      sparse_vertexset[array_idx][indices[array_idx]] = val;
      indices[array_idx]++;
    }
    release_lock(array_idx);
  }

  //NOTE(Emily): because -1 is our indicator of end of array, do we need to return the index array as well?
  //or can we assume that the core will read until sees -1 flag?
  int ** get_data()
  {
    return sparse_vertexset;
  }

  HashedSparseVertexset &operator=(HashedSparseVertexset &&other) {
          moveFrom(other);
          return *this;
  }


private:

  int * indices;
  int ** sparse_vertexset;

  void moveFrom(HashedSparseVertexset & other) {
          //TODO(Emily): we can't use std::move on device, so need to implement swap
          //indices = std::move(other.indices);
          //sparse_vertexset = std::move(other.sparse_vertexset);
  }

};
