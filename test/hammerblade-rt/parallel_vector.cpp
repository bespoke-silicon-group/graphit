#include "builtins_hammerblade.h"
#include <string.h>

const std::string ucode_path =
        "/home/centos/bsg_bladerunner/bsg_manycore"
        "/software/spmd/bsg_cuda_lite_runtime/kernel_parallel_vector/main.riscv";

using hammerblade::Device;
using hammerblade::Vector;
using hammerblade::GraphHB;
using hammerblade::GlobalScalar;

int main(int argc, char * argv[]){
  hammerblade::builtin_loadMicroCodeFromFile(ucode_path);

  Device::Ptr device = Device::GetInstance();

  size_t cores = 2;
  size_t len = 24;

  Vector<int32_t> test = new Vector<int32_t>(len, cores, 0);


  test.insert(8, 8);


  device->enqueueJob("test_vector_kernel",
                    {test.getAddr(),
                     len,
                     (len / cores)});
  device->runJobs();
  int * test_host
  test.copyToHost(test_host, len * cores);

  std::cout << "Results of Test" << std::endl;
  for(auto i = 0 ; i < len * cores; i++) {
      std::cerr << test_host[i] << std::endl;
  }
}
