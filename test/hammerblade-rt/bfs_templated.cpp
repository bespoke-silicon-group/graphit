#include "builtins_hammerblade.h"
#include <string.h>

const std::string ucode_path =
        "/home/centos/bsg_bladerunner/bsg_manycore"
        "/software/spmd/bsg_cuda_lite_runtime/graphit_bfs_templated/main.riscv";

using hammerblade::Device;
using hammerblade::Vector;
using hammerblade::GraphHB;
using hammerblade::GlobalScalar;
GraphHB edges;
//GlobalScalar<hb_mc_eva_t> parent_dev;

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
  parent_dev = GlobalScalar<hb_mc_eva_t>("parent");
  edges = hammerblade::builtin_loadEdgesFromFileToHB ( argv[(1) ]) ;
  Device::Ptr device = Device::GetInstance();

  Vector<int32_t> parent(edges.num_nodes());
  device->enqueueJob("parent_generated_vector_op_apply_func_0_kernel",{parent.getAddr(), edges.num_nodes(), edges.num_edges(),edges.num_nodes()});
  device->runJobs();

  Vector<int32_t> frontier(edges.num_nodes());
  Vector<int32_t> next_frontier(edges.num_nodes());
  int32_t host_frontier[edges.num_nodes()];

  for(auto i = 0; i < edges.num_nodes(); i++)
      host_frontier[i] = 0;
  //next_frontier.copyToDevice(host_frontier, edges.num_nodes());
  next_frontier.assign(0, edges.num_nodes(), 0);

  //host_frontier[0] = 1;
  //frontier.copyToDevice(host_frontier, edges.num_nodes());
  frontier.assign(0, edges.num_nodes(), 0);
  frontier.insert(0, 1);

  //hacky way to set one elem of parent. we probably want to add lib support for this
  int32_t host_parent[edges.num_nodes()];
  // parent.copyToHost(host_parent, edges.num_nodes());
  // host_parent[0 ] = 0 ;
  // parent.copyToDevice(host_parent, edges.num_nodes());
  parent.insert(0, 0);
  while ( (getFrontierSize(frontier) ) != ((0) ))
  {
      device->enqueueJob("edgeset_apply_push_serial_from_vertexset_to_filter_func_with_frontier_call",
                        {edges.getOutIndicesAddr(),
                         edges.getOutNeighborsAddr(),
                         frontier.getAddr(),
                         next_frontier.getAddr(),
                         parent.getAddr(),
                         edges.num_nodes(),
                         edges.num_edges(),
                         edges.num_nodes()});
      device->runJobs();

      next_frontier.copyToHost(host_frontier, edges.num_nodes());
      frontier.copyToDevice(host_frontier, edges.num_nodes());
      next_frontier.assign(0, edges.num_nodes(), 0);
  }
  parent.copyToHost(host_parent, edges.num_nodes());
  std::cerr << "Results of BFS" << std::endl;
  for(auto i : host_parent) {
      std::cerr << i << std::endl;
  }
}
