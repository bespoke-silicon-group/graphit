#include "builtins_hammerblade.h"
#include <string.h>

const std::string ucode_path =
        "/home/centos/bsg_bladerunner/bsg_manycore_86ef340"
        "/software/spmd/bsg_cuda_lite_runtime/graphit_pr_nibble/main.riscv";

using hammerblade::Device;
using hammerblade::Vector;
using hammerblade::GraphHB;
GraphHB edges;

//int  * __restrict generated_tmp_vector_3;
float alpha;
float epsilon;

int getFrontierSize(Vector<int32_t> &frontier)
{
   int size = 0;
   int32_t temp[edges.num_nodes()];
   frontier.copyToHost(temp, edges.num_nodes());
   for(auto i : temp) {
      if(i == 1) {
          size++;
      }
    }
    return size;
}

int main(int argc, char * argv[]){
  hammerblade::builtin_loadMicroCodeFromFile(ucode_path);
  edges = hammerblade::builtin_loadEdgesFromFileToHB ( argv[(1) ]) ;
  Device::Ptr device = Device::GetInstance();

  Vector<float> p(edges.num_nodes());
  Vector<float> r(edges.num_nodes());
  Vector<float> r_p(edges.num_nodes());
  Vector<int32_t> out_degree(edges.num_nodes());

  alpha = ((float) 0.15) ;
  epsilon = ((float) 1e-06) ;

  device->enqueueJob("kernel_init_p",
                   {p.getAddr(),
                    edges.num_nodes(),
                    edges.num_edges(),
                    edges.num_nodes()});

  device->enqueueJob("kernel_init_r",
                   {r.getAddr(),
                    edges.num_nodes(),
                    edges.num_edges(),
                    edges.num_nodes()});

  device->enqueueJob("kernel_init_r_p",
                   {r_p.getAddr(),
                    edges.num_nodes(),
                    edges.num_edges(),
                    edges.num_nodes()});

  device->enqueueJob("kernel_init_out_degree",
                    {out_degree.getAddr(),
                    edges.getOutIndicesAddr(),
                    edges.num_nodes(),
                    edges.num_edges(),
                    edges.num_nodes()});
  device->runJobs();

  Vector<int32_t> frontier(edges.num_nodes());
  Vector<int32_t> next_frontier(edges.num_nodes());
  int32_t host_frontier[edges.num_nodes()];

  next_frontier.assign(0, edges.num_nodes(), 0);
  frontier.assign(0, edges.num_nodes(), 0);
  frontier.insert(0, 1);

  r.insert(0, 1);

  while ( (getFrontierSize(frontier) ) != ((0) ))
  {
    device->enqueueJob("kernel_copy_r_to_rp",
                      {r.getAddr(),
                      r_p.getAddr(),
                      edges.num_nodes(),
                      edges.num_edges(),
                      edges.num_nodes()});
    device->runJobs();
    device->enqueueJob("kernel_update_self",
                      {p.getAddr(),
                      r.getAddr(),
                      r_p.getAddr(),
                      alpha,
                      frontier.getAddr(),
                      edges.getOutIndicesAddr(),
                      edges.num_nodes(),
                      edges.num_edges(),
                      edges.num_nodes()});
    device->runJobs();

    device->enqueueJob("kernel_update_neigh",
                      {frontier.getAddr(),
                      edges.getOutIndicesAddr(),
                      edges.getOutNeighborsAddr(),
                      alpha,
                      r.getAddr(),
                      r_p.getAddr(),
                      out_degree.getAddr(),
                      edges.num_nodes(),
                      edges.num_edges(),
                      edges.num_nodes()});
    device->runJobs();

    device->enqueueJob("kernel_copy_rp_to_r",
                      {r.getAddr(),
                      r_p.getAddr(),
                      edges.num_nodes(),
                      edges.num_edges(),
                      edges.num_nodes()});
    device->runJobs();

    device->enqueueJob("kernel_build_next_frontier",
                      {next_frontier.getAddr(),
                      r.getAddr(),
                      out_degree.getAddr(),
                      epsilon,
                      edges.num_nodes(),
                      edges.num_edges(),
                      edges.num_nodes()});
    device->runJobs();

    next_frontier.copyToHost(host_frontier, edges.num_nodes());
    frontier.copyToDevice(host_frontier, edges.num_nodes());
    next_frontier.assign(0, edges.num_nodes(), 0);
  }

}
