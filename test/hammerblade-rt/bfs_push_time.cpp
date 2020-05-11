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
int  * __restrict parent;

template <typename TO_FUNC , typename APPLY_FUNC> VertexSubset<NodeID>* edgeset_apply_pull_serial_from_vertexset_to_filter_func_with_frontier(Graph & g , VertexSubset<NodeID>* from_vertexset, TO_FUNC to_func, APPLY_FUNC apply_func)
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
    for(NodeID d : g.out_neigh(s)){
      traversed++;
      if (to_func(d)) {
        if( apply_func ( s , d  ) ) {
          outEdges[offset + j] = d;
        } else { outEdges[offset + j] = UINT_E_MAX; }
      } //end of to func
       else { outEdges[offset + j] = UINT_E_MAX;  }
      j++;
    } //end of for loop on neighbors
  }
  //std::cout << "traversed edges for this iteration: " << traversed << std::endl;
  uintE *nextIndices = newA(uintE, outEdgeCount);
  long nextM = sequence::filter(outEdges, nextIndices, outEdgeCount, nonMaxF());
  free(outEdges);
  free(degrees);
  next_frontier->num_vertices_ = nextM;
  next_frontier->dense_vertex_set_ = nextIndices;
  return next_frontier;
} //end of edgeset apply function
struct parent_generated_vector_op_apply_func_0
{
  void operator() (NodeID v)
  {
    parent[v] =  -(1) ;
  };
};
struct updateEdge
{
  bool operator() (NodeID src, NodeID dst)
  {
    bool output1 ;
    parent[dst] = src;
    output1 = (bool) 1;
    return output1;
  };
};
struct toFilter
{
  bool operator() (NodeID v)
  {
    bool output ;
    output = (parent[v]) == ( -(1) );
    return output;
  };
};
struct printParent
{
  void operator() (NodeID v)
  {
    std::cout << parent[v]<< std::endl;
  };
};
int main(int argc, char * argv[])
{
  InputParser parser(argc, argv);
  if(!parser.cmdOptionExists("-f")){
    std::cerr << "Usage: " << argv[0] << " -f [input graph edge list file] -r [root vertex]" << std::endl;
    return 0;
  }
  std::string input_graph = parser.getCmdOption("-f");
  int root = 0;
  if(parser.cmdOptionExists("-r")) root = std::stoi(parser.getCmdOption("-r"));
  //int root = 524287; //this is the max vertex for graph500.19.16.el
  //int root = 1048572; //this is the max vertex for graph500.20.16.el
  //int root = 65535; //this is the max vertex for graph500.16.16.el
  //int root = 262143; //this is the max vertex for graph500.18.16.el
  edges = builtin_loadEdgesFromFile ( input_graph.c_str()) ;
  parent = new int [ builtin_getVertices(edges) ];
  parallel_for (int vertexsetapply_iter = 0; vertexsetapply_iter < builtin_getVertices(edges) ; vertexsetapply_iter++) {
    parent_generated_vector_op_apply_func_0()(vertexsetapply_iter);
  };
  if(root == -1)
    root = rand() % builtin_getVertices(edges); //random seed node in range of vertices
  VertexSubset<int> *  frontier = new VertexSubset<int> ( builtin_getVertices(edges)  , (0) );
  builtin_addVertex(frontier, root ) ;
  parent[root ] = (root) ;
  std::cout << "root: " << root << std::endl;
  int iter_c = 0;
  bool written = false;
  while ( (builtin_getVertexSetSize(frontier) ) != ((0) ))
  {
    VertexSubset<int> *  output ;
    double percentage = builtin_getVertexSetSize(frontier) / ((double) builtin_getVertices(edges));
    auto start = std::chrono::steady_clock::now();
    output = edgeset_apply_pull_serial_from_vertexset_to_filter_func_with_frontier(edges, frontier, toFilter(), updateEdge());
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    deleteObject(frontier) ;
    frontier = output;
    iter_c++;
    //double percentage = builtin_getVertexSetSize(frontier) / ((double) builtin_getVertices(edges));
    std::cout << "size of frontier/total nodes: " << percentage << std::endl;
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n" << std::endl;
  }
  deleteObject(frontier) ;
};
