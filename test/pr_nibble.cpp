#include <iostream> 
#include <vector>
#include <algorithm>
#include "intrinsics.h"
Graph edges; 
float  * __restrict p;
float  * __restrict r;
float  * __restrict r_p;
int  * __restrict out_degree;
int  * __restrict generated_tmp_vector_3;
float alpha; 
float epsilon; 
template <typename APPLY_FUNC > void edgeset_apply_pull_serial_from_vertexset(Graph & g , VertexSubset<NodeID>* from_vertexset, APPLY_FUNC apply_func) 
{ 
    int64_t numVertices = g.num_nodes(), numEdges = g.num_edges();
  from_vertexset->toDense();
  for ( NodeID d=0; d < g.num_nodes(); d++) {
    for(NodeID s : g.in_neigh(d)){
      if (from_vertexset->bool_map_[s] ) { 
        apply_func ( s , d );
      }
    } //end of loop on in neighbors
  } //end of outer for loop
} //end of edgeset apply function 
struct generated_vector_op_apply_func_4
{
  void operator() (NodeID v) 
  {
    out_degree[v] = generated_tmp_vector_3[v];
  };
};
struct r_p_generated_vector_op_apply_func_2
{
  void operator() (NodeID v) 
  {
    r_p[v] = (0) ;
  };
};
struct r_generated_vector_op_apply_func_1
{
  void operator() (NodeID v) 
  {
    r[v] = (0) ;
  };
};
struct p_generated_vector_op_apply_func_0
{
  void operator() (NodeID v) 
  {
    p[v] = (0) ;
  };
};
struct updateNeigh
{
  void operator() (NodeID src, NodeID dst) 
  {
    r_p[dst] += (((((1)  - alpha) / ((1)  + alpha)) * r[src]) / out_degree[src]);
  };
};
struct updateSelf
{
  void operator() (NodeID v) 
  {
    p[v] = (p[v] + ((((2)  * alpha) / ((1)  + alpha)) * r[v]));
    r_p[v] = (0) ;
  };
};
struct addToFrontier
{
  bool operator() (NodeID v) 
  {
    bool output ;
    output = (r[v]) >= ((out_degree[v] * epsilon));
    return output;
  };
};
struct printRank
{
  void operator() (NodeID v) 
  {
    std::cout << p[v]<< std::endl;
  };
};
int main(int argc, char * argv[])
{
  edges = builtin_loadEdgesFromFile ( argv[(1) ]) ;
  p = new float [ builtin_getVertices(edges) ];
  r = new float [ builtin_getVertices(edges) ];
  r_p = new float [ builtin_getVertices(edges) ];
  out_degree = new int [ builtin_getVertices(edges) ];
  alpha = ((float) 0.85) ;
  epsilon = ((float) 0.1) ;
  parallel_for (int vertexsetapply_iter = 0; vertexsetapply_iter < builtin_getVertices(edges) ; vertexsetapply_iter++) {
    p_generated_vector_op_apply_func_0()(vertexsetapply_iter);
  };
  parallel_for (int vertexsetapply_iter = 0; vertexsetapply_iter < builtin_getVertices(edges) ; vertexsetapply_iter++) {
    r_generated_vector_op_apply_func_1()(vertexsetapply_iter);
  };
  parallel_for (int vertexsetapply_iter = 0; vertexsetapply_iter < builtin_getVertices(edges) ; vertexsetapply_iter++) {
    r_p_generated_vector_op_apply_func_2()(vertexsetapply_iter);
  };
  generated_tmp_vector_3 = builtin_getOutDegrees(edges) ;
  parallel_for (int vertexsetapply_iter = 0; vertexsetapply_iter < builtin_getVertices(edges) ; vertexsetapply_iter++) {
    generated_vector_op_apply_func_4()(vertexsetapply_iter);
  };
  VertexSubset<int> *  frontier = new VertexSubset<int> ( builtin_getVertices(edges)  , (0) );
  builtin_addVertex(frontier, (0) ) ;
  r[(0) ] = (1) ;
  while ( (builtin_getVertexSetSize(frontier) ) != ((0) ))
  {
    r_p = r;
     builtin_vertexset_apply ( frontier, updateSelf() ); 
;
    edgeset_apply_pull_serial_from_vertexset(edges, frontier, updateNeigh()); 
    r = r_p;
    VertexSubset<int> *  output ;
    auto ____graphit_tmp_out = new VertexSubset <NodeID> ( builtin_getVertices(edges)  , 0 );
bool * next5 = newA(bool, builtin_getVertices(edges) );
    parallel_for (int v = 0; v < builtin_getVertices(edges) ; v++) {
      next5[v] = 0;
if ( addToFrontier()( v ) )
        next5[v] = 1;
    } //end of loop
____graphit_tmp_out->num_vertices_ = sequence::sum( next5, builtin_getVertices(edges)  );
____graphit_tmp_out->bool_map_ = next5;

    output  = ____graphit_tmp_out; 
    deleteObject(frontier) ;
    frontier = output;
  }
  parallel_for (int vertexsetapply_iter = 0; vertexsetapply_iter < builtin_getVertices(edges) ; vertexsetapply_iter++) {
    printRank()(vertexsetapply_iter);
  };
  deleteObject(frontier) ;
};

