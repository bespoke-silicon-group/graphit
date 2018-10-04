#include "builtins_hammerblade.h"
#include <string.h>

const std::string ucode_path =
        "/home/centos/bsg_bladerunner/bsg_manycore_86ef340"
        "/software/spmd/bsg_cuda_lite_runtime/parallel_vector/main.riscv";

using hammerblade::Device;
using hammerblade::Vector;
using hammerblade::GraphHB;
using hammerblade::GlobalScalar;
using hammerblade::ParallelVector;

GraphHB edges;
const char * graph_path =  "/mnt/users/ssd0/homes/eafurst/graphit/test/graphs/4.el";

int launch(int argc, char * argv[]){
  hammerblade::builtin_loadMicroCodeFromFile(ucode_path);
  //parent_dev = GlobalScalar<hb_mc_eva_t>("parent");
  edges = hammerblade::builtin_loadEdgesFromFileToHB ( graph_path) ;
  Device::Ptr device = Device::GetInstance();

  ParallelVector<int32_t> test(hammerblade::builtin_getVerticesHB(edges), 2, 0);
  int block_size = edges.num_nodes();
  if(edges.num_nodes() % 2 == 0) {
    std::cerr << "even number of nodes, running in 2 blocks" << std::endl;
    block_size = edges.num_nodes()/2;
  }
  device->enqueueJob("kernel_parallel_vector", {test.getAddr(), edges.num_nodes(), block_size});
  device->runJobs();

  int host_test[edges.num_nodes() * 2];
  test.copyToHost(host_test, edges.num_nodes() * 2);

  for(int i = 0; i < edges.num_nodes() * 2; i++)
  {
    std::cerr << i << " : " << host_test[i] << std::endl;
  }
  return 0;
}

#ifdef COSIM
void cosim_main(uint32_t *exit_code, char * args) {
    int argc = get_argc(args);
    char *argv[argc];
    get_argv(args, argc, argv);
#ifdef VCS
    svScope scope;
    scope = svGetScopeFromName("tb");
    svSetScope(scope);
#endif
    bsg_pr_test_info("Unified Main Regression Test (COSIMULATION)\n");
    int rc = launch(argc,argv);
    *exit_code = rc;
    bsg_pr_test_pass_fail(rc == HB_MC_SUCCESS);
    return;
}
#else
int main(int argc, char ** argv) {
    bsg_pr_test_info("Unified Main CUDA Regression Test (F1)\n");
    int rc = launch(argc,argv);
    bsg_pr_test_pass_fail(rc == HB_MC_SUCCESS);
    return rc;
}
#endif
