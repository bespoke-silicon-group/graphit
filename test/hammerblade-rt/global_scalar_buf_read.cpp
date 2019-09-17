#include "builtins_hammerblade.h"
#include <string.h>

const std::string ucode_path =
        "/home/centos/bsg_bladerunner/bsg_manycore"
        "/software/spmd/bsg_cuda_lite_runtime/graph_device_scalar/main.riscv";

using hammerblade::Device;
using hammerblade::Vector;
using hammerblade::GraphHB;
using hammerblade::GlobalScalar;

GraphHB edges;

using GlblHBPtr   = GlobalScalar<hb_mc_eva_t>;

GlblHBPtr parent_dev;

int main(int argc, char * argv[]){
    hammerblade::builtin_loadMicroCodeFromFile(ucode_path);

    // set parent_dev to its device counterpart
    parent_dev = GlblHBPtr("parent");

    // initialize the graph
    edges = hammerblade::builtin_loadEdgesFromFileToHB ( argv[(1) ]) ;
    Device::Ptr device = Device::GetInstance();

    // set parent on the device to an allocated buffer in DRAM
    std::cout << "setting global scalar values" << std::endl;
    parent_dev.set(device->malloc(parent_dev.scalar_size() * edges.num_nodes()));

    // run the kernel
    std::cout << "set values without error, launching kernel to modify global scalars on device" << std::endl;
    device->enqueueJob("kernel_graph_device_scalar",
                          {edges.num_nodes(),
                          edges.num_nodes()});
    device->runJobs();

    // parent[i] = i for all i ?
    uint32_t parents[edges.num_nodes()];
    hammerblade::read_global_buffer(parents, parent_dev, edges.num_nodes());
    for (auto i = 0; i < edges.num_nodes(); i++) {
            std::cout << "parents[" << i << "] = " << parents[i] << std::endl;
    }
}
