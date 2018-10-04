#include "builtins_hammerblade.h"
#include <string.h>

const std::string ucode_path =
        "/home/centos/bsg_bladerunner/bsg_manycore"
        "/software/spmd/bsg_cuda_lite_runtime/template_func/main.riscv";

using hammerblade::Device;
using hammerblade::Vector;
using hammerblade::GraphHB;
using hammerblade::GlobalScalar;

GraphHB edges;

using GlblHBInt = GlobalScalar<int>;

GlblHBInt test_val_dev;

int main(int argc, char * argv[]){
    hammerblade::builtin_loadMicroCodeFromFile(ucode_path);

    test_val_dev = GlblHBInt("test_val");
    edges = hammerblade::builtin_loadEdgesFromFileToHB ( argv[(1) ]) ;
    Device::Ptr device = Device::GetInstance();

    std::cout << "setting global scalar values" << std::endl;
    test_val_dev.set(1);
    std::cout << "launching kernel using templated funcs" << std::endl;
    device->enqueueJob("kernel_template_func",
                          {edges.num_nodes(),
                           edges.num_nodes(),
                           edges.num_edges(),
                           edges.num_nodes()});
    device->runJobs();
    std::cout << "kernel finished running" << std::endl;
    int temp = test_val_dev.get();

    std::cout << "new test value: " << temp << std::endl;
}
