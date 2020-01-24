#include <iostream>
#include <vector>
#include <algorithm>
#include "intrinsics.h"
#include <stdlib.h>

Graph edges;
int  * __restrict parent;
template <typename TO_FUNC , typename APPLY_FUNC> VertexSubset<NodeID>* edgeset_apply_pull_serial_from_vertexset_to_filter_func_with_frontier(Graph & g , VertexSubset<NodeID>* from_vertexset, TO_FUNC to_func, APPLY_FUNC apply_func)
{
  int64_t numVertices = g.num_nodes(), numEdges = g.num_edges();
  VertexSubset<NodeID> *next_frontier = new VertexSubset<NodeID>(g.num_nodes(), 0);
  bool * next = newA(bool, g.num_nodes());
  parallel_for (int i = 0; i < numVertices; i++)next[i] = 0;
  from_vertexset->toDense();
  for ( NodeID d=0; d < g.num_nodes(); d++) {
    if (to_func(d)){
      for(NodeID s : g.in_neigh(d)){
        if (from_vertexset->bool_map_[s] ) {
          if( apply_func ( s , d ) ) {
            next[d] = 1;
            if (!to_func(d)) break;
          }
        }
      } //end of loop on in neighbors
    } //end of to filtering
  } //end of outer for loop
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
  //int root = (int) argv[2];
  int root = 524287; //this is the max vertex for graph500.19.16.el
  edges = builtin_loadEdgesFromFile ( argv[(1) ]) ;
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
  while ( (builtin_getVertexSetSize(frontier) ) != ((0) ))
  {
    VertexSubset<int> *  output ;
    output = edgeset_apply_pull_serial_from_vertexset_to_filter_func_with_frontier(edges, frontier, toFilter(), updateEdge());
    deleteObject(frontier) ;
    frontier = output;
    iter_c++;
    if(builtin_getVertexSetSize(frontier) > (builtin_getVertices(edges)/3)){
      std::cout << "dumping frontier now after " << iter_c << " iterations" << std::endl;
      break;
    }
  }
  //need to dump frontier here
  deleteObject(frontier) ;
  // parallel_for (int vertexsetapply_iter = 0; vertexsetapply_iter < builtin_getVertices(edges) ; vertexsetapply_iter++) {
  //   printParent()(vertexsetapply_iter);
  // };
};
