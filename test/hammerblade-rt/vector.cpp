#include "builtins_hammerblade.h"
#define v1_size 1024
const std::string ucode_path =
        "/home/centos/bsg_bladerunner/bsg_manycore_86ef340"
        "/software/spmd/bsg_cuda_lite_runtime/vec_set/main.riscv";

using hammerblade::Device;
using hammerblade::Vector;
#define N 8

static
int copy_in_out()
{
        uint32_t hv1[N];
        uint32_t hv2[N];

        ///////////////////////////////////////////////
        // initialize hv1 with easy to verify values //
        ///////////////////////////////////////////////
        for (auto i = 0; i < N; i++)
                hv1[i] = i;

        Vector<uint32_t> dv(N);

        std::cerr << "dv: base: " << std::hex << dv.getBase()
                  << " length: " << std::dec << dv.getLength() << std::endl;
                
        // copy in
        dv.copyToDevice(hv1, N);
        // copy out
        dv.copyToHost(hv2, N);

        for (auto i = 0; i < N; i++)
                std::cerr << hv2[i] << std::endl;
}

static
void call_vec_set()
{
        Vector<uint32_t> dv(N);
        uint32_t hv[N];

        std::cerr << "dv: base: " << std::hex << dv.getBase()
                  << " length: " << std::dec << dv.getLength() << std::endl;
        
        Device::Ptr device = Device::GetInstance();
        device->enqueueJob("kernel_vec_set", {dv.getBase(),dv.getLength()});
        device->runJobs();
        dv.copyToHost(hv, N);

        for (auto i = 0; i < N; i++)
                std::cerr << hv[i] << std::endl;        
}

int main()
{
       
        hammerblade::builtin_loadMicroCodeFromFile(ucode_path);
        copy_in_out();
        call_vec_set();
}

