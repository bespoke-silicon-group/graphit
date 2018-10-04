#include <iostream>
#include <vector>
#include <algorithm>
#include "intrinsics.h"
#include "hammerblade/host/arg_parser.hpp"
#include <stdlib.h>
#include <fstream>
#include <string>
#include <chrono>

WGraph edges;
int  * __restrict SP;
template <typename APPLY_FUNC > VertexSubset<NodeID>* edgeset_apply_pull_serial_weighted_deduplicatied_from_vertexset_with_frontier(WGraph & g , VertexSubset<NodeID>* from_vertexset, APPLY_FUNC apply_func)
{
  int64_t numVertices = g.num_nodes(), numEdges = g.num_edges();
  from_vertexset->toSparse();
  long m = from_vertexset->size();
  // used to generate nonzero indices to get degrees
  uintT *degrees = newA(uintT, m);
  // We probably need this when we get something that doesn't have a dense set, not sure
  // We can also write our own, the eixsting one doesn't quite work for bitvectors
  //from_vertexset->toSparse();
  {
      parallel_for (long i = 0; i < m; i++) {
          NodeID v = from_vertexset->dense_vertex_set_[i];
          degrees[i] = g.out_degree(v);
      }
  }
  uintT outDegrees = sequence::plusReduce(degrees, m);
  if (g.flags_ == nullptr){
    g.flags_ = new int[numVertices]();
    parallel_for(int i = 0; i < numVertices; i++) g.flags_[i]=0;
  }
  VertexSubset<NodeID> *next_frontier = new VertexSubset<NodeID>(g.num_nodes(), 0);
  if (numVertices != from_vertexset->getVerticesRange()) {
      cout << "edgeMap: Sizes Don't match" << endl;
      abort();
  }
  if (outDegrees == 0) return next_frontier;
  uintT *offsets = degrees;
  long outEdgeCount = sequence::plusScan(offsets, degrees, m);
  uintE *outEdges = newA(uintE, outEdgeCount);
  int traversed = 0;
for (long i=0; i < m; i++) {
  NodeID s = from_vertexset->dense_vertex_set_[i];
  int j = 0;
  uintT offset = offsets[i];
  for(WNode d : g.out_neigh(s)){
    traversed++;
    if( apply_func ( s , d.v, d.w )) {
      //std::cout << "adding to frontier\n";
      outEdges[offset + j] = d.v;
    } else { outEdges[offset + j] = UINT_E_MAX; }
    j++;
  } //end of for loop on neighbors
}
//std::cout << "traversed edges for this iteration: " << traversed << std::endl;
uintE *nextIndices = newA(uintE, outEdgeCount);
long nextM = sequence::filter(outEdges, nextIndices, outEdgeCount, nonMaxF());
free(outEdges);
free(degrees);
//std::cout << "next frontier size: " << nextM << std::endl;
next_frontier->num_vertices_ = nextM;
next_frontier->dense_vertex_set_ = nextIndices;
parallel_for(int i = 0; i < nextM; i++){
   g.flags_[nextIndices[i]] = 0;
}
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
      //std::cout << "passed update edge cond" <<std::endl;
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
     std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n" << std::endl;
  }
  deleteObject(frontier) ;
};
