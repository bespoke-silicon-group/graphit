#include <iostream>
#include <vector>
#include <algorithm>
#include "intrinsics.h"
#include "hammerblade/host/arg_parser.hpp"
#include <stdlib.h>
#include <fstream>
#include <string>

Graph edges;
int  * __restrict parent;

template <typename TO_FUNC , typename APPLY_FUNC> VertexSubset<NodeID>* edgeset_apply_pull_serial_from_vertexset_to_filter_func_with_frontier(Graph & g , VertexSubset<NodeID>* from_vertexset, TO_FUNC to_func, APPLY_FUNC apply_func)
{
  int64_t numVertices = g.num_nodes(), numEdges = g.num_edges();
  VertexSubset<NodeID> *next_frontier = new VertexSubset<NodeID>(g.num_nodes(), 0);
  bool * next = newA(bool, g.num_nodes());
  parallel_for (int i = 0; i < numVertices; i++)next[i] = 0;
  from_vertexset->toDense();
  int traversed = 0;
  for ( NodeID d=0; d < g.num_nodes(); d++) {
    if (to_func(d)){
      for(NodeID s : g.in_neigh(d)){
        traversed++;
        if (from_vertexset->bool_map_[s] ) {
          if( apply_func ( s , d ) ) {
            next[d] = 1;
            if (!to_func(d)) break;
          }
        }
      } //end of loop on in neighbors
    } //end of to filtering
  } //end of outer for loop
  std::cout << "traversed edges for this iteration: " << traversed << std::endl;
  std::cout << std::endl;
  next_frontier->num_vertices_ = sequence::sum(next, numVertices);
  next_frontier->bool_map_ = next;
  next_frontier->is_dense = true;
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
    output = edgeset_apply_pull_serial_from_vertexset_to_filter_func_with_frontier(edges, frontier, toFilter(), updateEdge());
    deleteObject(frontier) ;
    frontier = output;
    iter_c++;
    double percentage = builtin_getVertexSetSize(frontier) / ((double) builtin_getVertices(edges));
    std::cout << "size of frontier/total nodes: " << percentage << std::endl;
    if(builtin_getVertexSetSize(frontier) > (builtin_getVertices(edges)/10) && !written){
      std::cout << "writing frontier to file" << std::endl;
      ofstream file("frontier.txt");
      ofstream par_file("parent.txt");
      if(file.is_open()){
        for ( NodeID d=0; d < edges.num_nodes(); d++) {
          if (frontier->bool_map_[d] ) {
            file << "1\n";
          }
          else {
            file << "0\n";
          }
          file2 << parent[d] << std::endl;
        }
        file.close();
        file2.close();
        written = true;
      }
//      break;
    }
  }
  deleteObject(frontier) ;
};
