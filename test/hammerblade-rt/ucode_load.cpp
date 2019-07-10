#include "builtins_hammerblade.h"
#define v1_size 1024
const std::string ucode_path =
        "/home/centos/bsg_bladerunner/bsg_manycore_86ef340"
        "/software/spmd/bsg_cuda_lite_runtime/empty/main.riscv";

using hammerblade::Device;

int main()
{
        hammerblade::builtin_loadMicroCodeFromFile(ucode_path);
}
