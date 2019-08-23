#include "builtins_hammerblade.h"
#include <string.h>

const std::string ucode_path =
        "/home/centos/bsg_bladerunner/bsg_manycore_86ef340"
        "/software/spmd/bsg_cuda_lite_runtime/graph_device_scalar/main.riscv";

using hammerblade::Device;
using hammerblade::Vector;
using hammerblade::GraphHB;
using hammerblade::GlobalScalar;

GraphHB edges;

using GlblHBPtr   = GlobalScalar<hb_mc_eva_t>;
using GlblHBFloat = GlobalScalar<float>;

GlblHBPtr parent_dev;
GlblHBFloat epsilon_dev;

int main(int argc, char * argv[]){
    hammerblade::builtin_loadMicroCodeFromFile(ucode_path);

    parent_dev = GlblHBPtr("parent");
    epsilon_dev = GlblHBFloat("epsilon");
    edges = hammerblade::builtin_loadEdgesFromFileToHB ( argv[(1) ]) ;
    Device::Ptr device = Device::GetInstance();

    std::cout << "setting global scalar values" << std::endl;
    parent_dev.set(device->malloc(parent_dev.scalar_size() * edges.num_nodes()));
    epsilon_dev.set(1e9);
    std::cout << "set values without error, launching kernel to modify global scalars on device" << std::endl;
    device->enqueueJob("kernel_graph_device_scalar",
                          {edges.num_nodes(),
                          edges.num_nodes()});
    device->runJobs();
    std::cout << "obtaining epsilon value" << std::endl;
    float temp = epsilon_dev.get();

    std::cout << "new epsilon value: " << temp << std::endl;
}
