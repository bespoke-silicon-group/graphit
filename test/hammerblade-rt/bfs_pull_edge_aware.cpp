#include <iostream> 
#include <vector>
#include <algorithm>
#include "intrinsics.h"
Graph edges; 
int  * __restrict parent;
template <typename TO_FUNC , typename APPLY_FUNC> VertexSubset<NodeID>* edgeset_apply_pull_parallel_from_vertexset_to_filter_func_with_frontier_pull_edge_based_load_balance(Graph & g , VertexSubset<NodeID>* from_vertexset, TO_FUNC to_func, APPLY_FUNC apply_func) 
{ 
    int64_t numVertices = g.num_nodes(), numEdges = g.num_edges();
  VertexSubset<NodeID> *next_frontier = new VertexSubset<NodeID>(g.num_nodes(), 0);
  bool * next = newA(bool, g.num_nodes());
  parallel_for (int i = 0; i < numVertices; i++)next[i] = 0;
  from_vertexset->toDense();
    if (g.offsets_ == nullptr) g.SetUpOffsets(true);
  SGOffset * edge_in_index = g.offsets_;
    std::function<void(int,int,int)> recursive_lambda = 
    [&to_func, &apply_func, &g,  &recursive_lambda, edge_in_index, &from_vertexset, &next   ]
    (NodeID start, NodeID end, int grain_size){
         if ((start == end-1) || ((edge_in_index[end] - edge_in_index[start]) < grain_size)){
  for (NodeID d = start; d < end; d++){
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
        } else { // end of if statement on grain size, recursive case next
                 cilk_spawn recursive_lambda(start, start + ((end-start) >> 1), grain_size);
                  recursive_lambda(start + ((end-start)>>1), end, grain_size);
        } 
    }; //end of lambda function
    recursive_lambda(0, numVertices, 4096);
    cilk_sync; 
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
  edges = builtin_loadEdgesFromFile ( argv[(1) ]) ;
  parent = new int [ builtin_getVertices(edges) ];
  parallel_for (int vertexsetapply_iter = 0; vertexsetapply_iter < builtin_getVertices(edges) ; vertexsetapply_iter++) {
    parent_generated_vector_op_apply_func_0()(vertexsetapply_iter);
  };
  VertexSubset<int> *  frontier = new VertexSubset<int> ( builtin_getVertices(edges)  , (0) );
  builtin_addVertex(frontier, (8) ) ;
  parent[(8) ] = (8) ;
  while ( (builtin_getVertexSetSize(frontier) ) != ((0) ))
  {
    VertexSubset<int> *  output ;
    output = edgeset_apply_pull_parallel_from_vertexset_to_filter_func_with_frontier_pull_edge_based_load_balance(edges, frontier, toFilter(), updateEdge()); 
    deleteObject(frontier) ;
    frontier = output;
  }
  deleteObject(frontier) ;
  for (int vertexsetapply_iter = 0; vertexsetapply_iter < builtin_getVertices(edges) ; vertexsetapply_iter++) {
    printParent()(vertexsetapply_iter);
  };
};

