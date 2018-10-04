#include <iostream>
#include <vector>
#include <algorithm>
#include "intrinsics.h"
#include "hammerblade/host/arg_parser.hpp"
#include <stdlib.h>
#include <fstream>
#include <string>
#include <chrono>
Graph edges;
float  * __restrict old_rank;
float  * __restrict new_rank;
int  * __restrict out_degree;
float  * __restrict error;
int  * __restrict generated_tmp_vector_2;
float damp;
float beta_score;
template <typename APPLY_FUNC > int edgeset_apply_push_serial(Graph & g , APPLY_FUNC apply_func)
{
    int64_t numVertices = g.num_nodes(), numEdges = g.num_edges();
    long m = numVertices;
    int traversed = 0;
  for (NodeID s=0; s < g.num_nodes(); s++) {
    for(NodeID d : g.out_neigh(s)){
      traversed++;
      apply_func ( s , d  );
    } //end of for loop on neighbors
  }
  return traversed;
} //end of edgeset apply function
struct error_generated_vector_op_apply_func_4
{
  void operator() (NodeID v)
  {
    error[v] = ((float) 0) ;
  };
};
struct generated_vector_op_apply_func_3
{
  void operator() (NodeID v)
  {
    out_degree[v] = generated_tmp_vector_2[v];
  };
};
struct new_rank_generated_vector_op_apply_func_1
{
  void operator() (NodeID v)
  {
    new_rank[v] = ((float) 0) ;
  };
};
struct old_rank_generated_vector_op_apply_func_0
{
  void operator() (NodeID v)
  {
    old_rank[v] = (((float) 1)  / builtin_getVertices(edges) );
  };
};
struct updateEdge
{
  void operator() (NodeID src, NodeID dst)
  {
    new_rank[dst] += (old_rank[src] / out_degree[src]);
  };
};
struct updateVertex
{
  void operator() (NodeID v)
  {
    float old_score = old_rank[v];
    new_rank[v] = (beta_score + (damp * new_rank[v]));
    error[v] = fabs((new_rank[v] - old_rank[v])) ;
    old_rank[v] = new_rank[v];
    new_rank[v] = ((float) 0) ;
  };
};
struct printRank
{
  void operator() (NodeID v)
  {
    std::cout << old_rank[v]<< std::endl;
  };
};
int main(int argc, char * argv[])
{
  InputParser parser(argc, argv);
  if(!parser.cmdOptionExists("-f")){
    std::cerr << "Usage: " << argv[0] << " -f [input graph edge list file]" << std::endl;
    return 0;
  }
  std::string input_graph = parser.getCmdOption("-f");
  edges = builtin_loadEdgesFromFile ( input_graph.c_str()) ;
  old_rank = new float [ builtin_getVertices(edges) ];
  new_rank = new float [ builtin_getVertices(edges) ];
  out_degree = new int [ builtin_getVertices(edges) ];
  error = new float [ builtin_getVertices(edges) ];
  damp = ((float) 0.85) ;
  beta_score = ((((float) 1)  - damp) / builtin_getVertices(edges) );
  parallel_for (int vertexsetapply_iter = 0; vertexsetapply_iter < builtin_getVertices(edges) ; vertexsetapply_iter++) {
    old_rank_generated_vector_op_apply_func_0()(vertexsetapply_iter);
  };
  parallel_for (int vertexsetapply_iter = 0; vertexsetapply_iter < builtin_getVertices(edges) ; vertexsetapply_iter++) {
    new_rank_generated_vector_op_apply_func_1()(vertexsetapply_iter);
  };
  generated_tmp_vector_2 = builtin_getOutDegrees(edges) ;
  parallel_for (int vertexsetapply_iter = 0; vertexsetapply_iter < builtin_getVertices(edges) ; vertexsetapply_iter++) {
    generated_vector_op_apply_func_3()(vertexsetapply_iter);
  };
  parallel_for (int vertexsetapply_iter = 0; vertexsetapply_iter < builtin_getVertices(edges) ; vertexsetapply_iter++) {
    error_generated_vector_op_apply_func_4()(vertexsetapply_iter);
  };
  startTimer() ;
  for ( int i = (1) ; i < (10) ; i++ )
  {
    auto start = std::chrono::steady_clock::now();
    int traversed = edgeset_apply_push_serial(edges, updateEdge());
    parallel_for (int vertexsetapply_iter = 0; vertexsetapply_iter < builtin_getVertices(edges) ; vertexsetapply_iter++) {
      updateVertex()(vertexsetapply_iter);
    };
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "traversed edges " << traversed << " total edges " << edges.num_edges() << std::endl;
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
  }
  float sum = (0) ;
  for ( int i = (0) ; i < builtin_getVertices(edges) ; i++ )
  {
    sum += error[i];
  }
  std::cout << sum<< std::endl;
  float elapsed_time = stopTimer() ;
  std::cout << "elapsed time: "<< std::endl;
  std::cout << elapsed_time<< std::endl;
};
