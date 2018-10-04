#include "builtins_hammerblade.h"
#include <string.h> 

const std::string ucode_path =
        "/home/centos/bsg_bladerunner/bsg_manycore_86ef340"
        "/software/spmd/bsg_cuda_lite_runtime/graphit_bfs/main.riscv";

using hammerblade::Device;
using hammerblade::Vector;
using hammerblade::GraphHB;
GraphHB edges; 

int getFrontierSize(Vector<int32_t> &frontier)
{
   int size = 0;
   int32_t temp[edges.num_nodes()];
   frontier.copyToHost(temp, edges.num_nodes());
   for(auto i : temp) {
      if(i == 1) {
          size++;
      }
    }
    return size;
}

int main(int argc, char * argv[]){
    hammerblade::builtin_loadMicroCodeFromFile(ucode_path);
    edges = hammerblade::builtin_loadEdgesFromFileToHB ( argv[(1) ]) ;
    Device::Ptr device = Device::GetInstance();    

    Vector<int32_t> parent(edges.num_nodes()); 
    device->enqueueJob("kernel_init_parent",
		     {parent.getAddr(),
		      edges.num_nodes(),
		      edges.num_edges(),
		      edges.num_nodes()});
    device->runJobs();  
   
    //hacky way of initializing and adding one elem to frontier
    Vector<int32_t> frontier(edges.num_nodes());
    Vector<int32_t> next_frontier(edges.num_nodes());
    int32_t host_frontier[edges.num_nodes()];
    for(auto i = 0; i < edges.num_nodes(); i++)
        host_frontier[i] = 0;
    next_frontier.copyToDevice(host_frontier, edges.num_nodes());
    host_frontier[0] = 1;
    frontier.copyToDevice(host_frontier, edges.num_nodes());
    
    //hacky way to set one elem of parent. we probably want to add lib support for this
    int32_t host_parent[edges.num_nodes()];
    parent.copyToHost(host_parent, edges.num_nodes()); 
    host_parent[0 ] = 0 ;
    parent.copyToDevice(host_parent, edges.num_nodes());
    while ( (getFrontierSize(frontier) ) != ((0) ))
    {
      device->enqueueJob("kernel_edgeset_apply_push_serial_from_vertexset_to_filter_func_with_frontier",
                         {edges.getOutIndicesAddr(),
                          edges.getOutNeighborsAddr(),
                          frontier.getAddr(),
                          next_frontier.getAddr(),
                          parent.getAddr(),
                          edges.num_nodes(),
                          edges.num_edges(),
                          edges.num_nodes()});
      device->runJobs();
      //frontier = next_frontier; //copy next to current frontier
      next_frontier.copyToHost(host_frontier, edges.num_nodes());
      frontier.copyToDevice(host_frontier, edges.num_nodes());
      for(auto i = 0; i < edges.num_nodes(); i++) {
         host_frontier[i] = 0;
      }
      next_frontier.copyToDevice(host_frontier, edges.num_nodes());
    }
    parent.copyToHost(host_parent, edges.num_nodes());
    std::cerr << "Results of BFS" << std::endl;
    for(auto i : host_parent) {
        std::cerr << i << std::endl;
    }
}

