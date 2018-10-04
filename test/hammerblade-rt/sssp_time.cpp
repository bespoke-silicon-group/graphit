#include <iostream>
#include <vector>
#include <algorithm>
#include "intrinsics.h"
#include "hammerblade/host/arg_parser.hpp"
#include <stdlib.h>
#include <fstream>
#include <string>

WGraph edges;
int  * __restrict SP;
template <typename APPLY_FUNC > VertexSubset<NodeID>* edgeset_apply_pull_serial_weighted_deduplicatied_from_vertexset_with_frontier(WGraph & g , VertexSubset<NodeID>* from_vertexset, APPLY_FUNC apply_func)
{
    int64_t numVertices = g.num_nodes(), numEdges = g.num_edges();
  VertexSubset<NodeID> *next_frontier = new VertexSubset<NodeID>(g.num_nodes(), 0);
  bool * next = newA(bool, g.num_nodes());
  parallel_for (int i = 0; i < numVertices; i++)next[i] = 0;
  from_vertexset->toDense();
  int traversed = 0;
  for ( NodeID d=0; d < g.num_nodes(); d++) {
    for(WNode s : g.in_neigh(d)){
      traversed++;
      if (from_vertexset->bool_map_[s.v] ) {
        if( apply_func ( s.v , d, s.w ) ) {
          next[d] = 1;
        }
      }
    } //end of loop on in neighbors
  } //end of outer for loop
  next_frontier->num_vertices_ = sequence::sum(next, numVertices);
  next_frontier->bool_map_ = next;
  next_frontier->is_dense = true;
  return next_frontier;
} //end of edgeset apply function
struct SP_generated_vector_op_apply_func_0
{
  void operator() (NodeID v)
  {
    SP[v] = (2147483647) ;
  };
};
struct updateEdge
{
  bool operator() (NodeID src, NodeID dst, int weight)
  {
    bool output2 ;
    bool SP_trackving_var_1 = (bool) 0;
    if ( ( SP[dst]) > ( (SP[src] + weight)) ) {
      SP[dst]= (SP[src] + weight);
      SP_trackving_var_1 = true ;
    }
    output2 = SP_trackving_var_1;
    return output2;
  };
};
struct printSP
{
  void operator() (NodeID v)
  {
    std::cout << SP[v]<< std::endl;
  };
};
int main(int argc, char * argv[])
{
  InputParser parser(argc, argv);
  if(!parser.cmdOptionExists("-f")){
    std::cerr << "Usage: " << argv[0] << " -f [input graph weighted edge list file] -r [root vertex]" << std::endl;
    return 0;
  }
  std::string input_graph = parser.getCmdOption("-f");
  int root = 0;
  if(parser.cmdOptionExists("-r")) root = std::stoi(parser.getCmdOption("-r"));

  edges = builtin_loadWeightedEdgesFromFile (input_graph.c_str()) ;

  SP = new int [ builtin_getVertices(edges) ];
  parallel_for (int vertexsetapply_iter = 0; vertexsetapply_iter < builtin_getVertices(edges) ; vertexsetapply_iter++) {
    SP_generated_vector_op_apply_func_0()(vertexsetapply_iter);
  };
  startTimer() ;
  int n = builtin_getVertices(edges) ;
  VertexSubset<int> *  frontier = new VertexSubset<int> ( builtin_getVertices(edges)  , (0) );
  builtin_addVertex(frontier, root ) ;
  SP[root ] = root ;
  std::cout << "root: " << root << std::endl;
  int rounds = (0) ;

  bool written = false;
  while ( (builtin_getVertexSetSize(frontier) ) != ((0) ))
  {
    double percentage = builtin_getVertexSetSize(frontier) / ((double) builtin_getVertices(edges));
    auto start = std::chrono::steady_clock::now();
    VertexSubset<int> *  output = edgeset_apply_pull_serial_weighted_deduplicatied_from_vertexset_with_frontier(edges, frontier, updateEdge());
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    deleteObject(frontier) ;
    frontier = output;
    rounds = (rounds + (1) );
    if ((rounds) == (n))
     {
      std::cout << "negative cycle"<< std::endl;
      break;
     }

     std::cout << "size of frontier/total nodes: " << percentage << std::endl;
     std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n\n";

  }
  deleteObject(frontier) ;
};
