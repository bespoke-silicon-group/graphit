#include "builtins_hammerblade.h"
#include <string.h>

const std::string ucode_path =
        "/home/centos/bsg_bladerunner/bsg_manycore_86ef340"
        "/software/spmd/bsg_cuda_lite_runtime/graphit_bfs/main.riscv";

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

    parent_dev.set(device->malloc(parent_dev.scalar_size() * edges.num_nodes()));
    epsilon_dev.set(1e9);
}

