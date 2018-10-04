#include "builtins_hammerblade.h"

// note that this example is running a kernel that may not be available on GitHub
const std::string ucode_path =
        "/home/centos/bsg_bladerunner/bsg_manycore_86ef340"
        "/software/spmd/bsg_cuda_lite_runtime/graph_degree_count/main.riscv";

using hammerblade::Device;
using hammerblade::Vector;
using hammerblade::GraphHB;

GraphHB edges;

static
void run_graph_degree()
{
        Device::Ptr device = Device::GetInstance();
        Vector<int32_t> degree_count(edges.num_nodes());

        device->enqueueJob("kernel_graph_degree_count",
                           {edges.getOutIndicesAddr(),
                            edges.getOutNeighborsAddr(),
                            degree_count.getAddr(),
                            edges.num_nodes(),
                            edges.num_edges(),
                            edges.num_nodes()});
        device->runJobs();

        int32_t degrees[edges.num_nodes()];
        degree_count.copyToHost(degrees, edges.num_nodes());

        std::cerr << "kernel results" << std::endl;
        for (auto i : degrees)
                std::cerr << i << std::endl;

        std::cerr << "host check" << std::endl;
        const Graph &g = edges.getHostGraph();
        for (auto i = 0; i < g.num_nodes(); i++)
                std::cerr << g.out_degree(i) << std::endl;
}

int main(int argc, const char *argv[])
{
        if (argc != 2) {
                std::cerr << "not enough arguments" << std::endl;
                return 1;
        }

        hammerblade::builtin_loadMicroCodeFromFile(ucode_path);
        edges = hammerblade::builtin_loadEdgesFromFileToHB(argv[1]);;

        run_graph_degree();
}
