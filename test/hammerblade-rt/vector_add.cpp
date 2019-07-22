#include "builtins_hammerblade.h"
#define v1_size 1024
const std::string ucode_path =
        "/home/centos/bsg_bladerunner/bsg_manycore_86ef340"
        "/software/spmd/bsg_cuda_lite_runtime/vec_add/main.riscv";

using hammerblade::Device;
using hammerblade::Vector;
#define N 2

static
void call_vec_add()
{
        Vector<uint32_t> dA(N), dB(N), dC(N);
        uint32_t hA[N] = {1,0},
                 hB[N] = {0,1},
                 hC[N];

        dA.copyToDevice(hA, N);
        dB.copyToDevice(hB, N);
        
        Device::Ptr device = Device::GetInstance();
        device->enqueueJob("kernel_vec_add",
                           {dA.getBase(), dB.getBase(), dC.getBase(), N, N});
        device->runJobs();

        dC.copyToHost(hC, N);

        for (auto i = 0; i < N; i++)
                std::cerr << hC[i] << std::endl;
}

int main()
{
       
        hammerblade::builtin_loadMicroCodeFromFile(ucode_path);
        call_vec_add();
}

