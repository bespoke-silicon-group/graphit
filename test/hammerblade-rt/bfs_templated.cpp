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
GlobalScalar<hb_mc_eva_t> parent_dev;

int main(int argc, char * argv[]){
  hammerblade::builtin_loadMicroCodeFromFile(ucode_path);
  //parent_dev = GlobalScalar<hb_mc_eva_t>("parent");
  edges = hammerblade::builtin_loadEdgesFromFileToHB ( argv[(1) ]) ;
  Device::Ptr device = Device::GetInstance();

  device->enqueueJob("parent_generated_vector_op_apply_func_0_kernel",{edges.num_nodes(), edges.num_edges(),edges.num_nodes()});
  device->runJobs();

  Vector<int32_t> frontier(hammerblade::builtin_getVerticesHB(edges), 0);
  Vector<int32_t> next_frontier(hammerblade::builtin_getVerticesHB(edges), 0);
  Vector<int32_t> parent(hammerblade::builtin_getVerticesHB(edges), -1);

  frontier.insert(8, 1);

  parent.insert(8, 8);

  int host_frontier[edges.num_nodes()];

  while ( (hammerblade::builtin_getVertexSetSizeHB(frontier, edges.num_nodes()) ) != ((0) ))
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

      //frontier = next_frontier;
      next_frontier.copyToHost(host_frontier, edges.num_nodes());
      frontier.copyToDevice(host_frontier, edges.num_nodes());
      next_frontier.assign(0, edges.num_nodes(), 0);
  }
  int host_parent[edges.num_nodes()];
  //hammerblade::read_global_buffer(host_parent, parent_dev, edges.num_edges());
  parent.copyToHost(host_parent, edges.num_nodes());
  std::cerr << "Results of BFS" << std::endl;
  for(auto i : host_parent) {
      std::cerr << i << std::endl;
  }
}