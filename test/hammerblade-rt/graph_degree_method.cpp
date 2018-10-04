#include "builtins_hammerblade.h"
#define v1_size 1024
const std::string ucode_path =
        "/home/centos/bsg_bladerunner/bsg_manycore_86ef340"
        "/software/spmd/bsg_cuda_lite_runtime/graph_degree_method/main.riscv";

using hammerblade::Device;
using hammerblade::Vector;
using hammerblade::GraphHB;

GraphHB edges;

int main(int argc, const char *argv[])
{
        if (argc != 2) {
                std::cerr << "not enough arguments" << std::endl;
                return 1;
        }

        hammerblade::builtin_loadMicroCodeFromFile(ucode_path);
        edges = hammerblade::builtin_loadEdgesFromFileToHB(argv[1]);;
	int out_4 = edges.out_degree(4);
	std::cout << "out degree of vertex 4: " << out_4 << std::endl;
	
	int out_13 = edges.out_degree(13);
	std::cout << "out degree of vertex 13: " << out_13 << std::endl;
}
