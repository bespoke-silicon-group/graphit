#include "builtins_hammerblade.h"
#include <string.h>

const std::string ucode_path =
        "/home/centos/bsg_bladerunner/bsg_manycore"
        "/software/spmd/bsg_cuda_lite_runtime/graphit_bfs_parallel/main.riscv";

using hammerblade::Device;
using hammerblade::Vector;
using hammerblade::GraphHB;
using hammerblade::GlobalScalar;
GraphHB edges;
GlobalScalar<hb_mc_eva_t> parent_dev;

//hacking host side sync of parallel vector
int * syncArray(int * input, size_t length, size_t cores)
{
  int * base_result = new int[length];
  for(auto i = 0; i < length; i++) {
    for(auto c = 1; c < cores; c++) {
      base_result[i] = (input[i] == 1 || input[c * length + i] == 1 ? 1 : 0);
    }
  }

  for(auto i = 0; i < length; i++) {
    for(auto c = 0; c < cores; c++) {
      input[c * length + i] = base_result[i];
    }
  }

  return input;
}

//even hackier sync of a result array
int * syncResultArray(int * input, size_t length, size_t cores)
{
  for(auto i = 0; i < length; i++) {
    for(auto c = 1; c < cores; c++) {
      if(input[c * length + 1] != -1)
        input[i] = input[c * length + i];
    }
  }

  for(auto i = 0; i < length; i++) {
    for(auto c = 1; c < cores; c++) {
      input[c * length + i] = input[i];
    }
  }

  return input;
}

int main(int argc, char * argv[]){
  hammerblade::builtin_loadMicroCodeFromFile(ucode_path);
  //parent_dev = GlobalScalar<hb_mc_eva_t>("parent");
  edges = hammerblade::builtin_loadEdgesFromFileToHB ( argv[(1) ]) ;
  Device::Ptr device = Device::GetInstance();

  size_t cores = 2;

  Vector<int32_t> parent = new Vector<int32_t>(hammerblade::builtin_getVerticesHB(edges), cores, -1);

  //device->enqueueJob("parent_generated_vector_op_apply_func_0_kernel",{edges.num_nodes(), edges.num_edges(),edges.num_nodes()});
  //device->runJobs();

  Vector<int32_t> frontier = new Vector<int32_t>(hammerblade::builtin_getVerticesHB(edges), cores, 0);
  Vector<int32_t> next_frontier = new Vector<int32_t>(hammerblade::builtin_getVerticesHB(edges), cores, 0);
  int * temp_frontier;
  int * host_parent;

  frontier.insert(8, 8);

  parent.insert(8, 8);
  while ( (hammerblade::builtin_getVertexSetSizeHB(frontier, edges.num_edges()) ) != ((0) ))
  {
      device->enqueueJob("edgeset_apply_push_serial_from_vertexset_to_filter_func_with_frontier_call",
                        {edges.getOutIndicesAddr(),
                         edges.getOutNeighborsAddr(),
                         frontier.getAddr(),
                         next_frontier.getAddr(),
                         parent.getAddr(),
                         edges.num_nodes(),
                         edges.num_edges(),
                         edges.num_nodes() / cores});
      device->runJobs();

      //frontier = next_frontier;
      next_frontier.copyToHost(temp_frontier, edges.num_nodes() * cores);
      frontier.copyToDevice(syncArray(temp_frontier, edges.num_nodes(), cores), edges.num_nodes() * cores);
      next_frontier.assign(0, edges.num_nodes(), 0);

      parent.copyToHost(host_parent, edges.num_nodes() * cores);
      parent.copyToDevice(syncResultArray(host_parent, edges.num_nodes(), cores), edges.num_nodes() * cores);
  }

  //hammerblade::read_global_buffer(host_parent, parent_dev, edges.num_edges());
  parent.copyToDevice(host_parent, edges.num_nodes() * cores);
  int * result_parent = syncResultArray(host_parent, edges.num_nodes(), cores);
  std::cerr << "Results of BFS" << std::endl;
  for(auto i = 0 ; i < edges.num_nodes(); i++) {
      std::cerr << result_parent[i] << std::endl;
  }
}
