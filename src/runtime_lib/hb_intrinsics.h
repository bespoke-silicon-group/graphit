#pragma once
#define IGNORE_JULIENNE_TYPES
#include <infra_hb/host/graphhb.hpp>
#include <infra_hb/host/wgraphhb.hpp>
#include <infra_hb/host/device.hpp>
#include <infra_hb/host/error.hpp>
#include <infra_hb/host/global_scalar.hpp>
#include <infra_hb/host/priority_queue.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include "intrinsics.h"
#include <string>
namespace hammerblade {

static
GraphHB builtin_loadEdgesFromFileToHB(const char *graph_file, bool large = false, bool pull = false)
{
        if (!graph_file)
                throw std::runtime_error("bad graph file: did you pass a graph argument?");

        return GraphHB(builtin_loadEdgesFromFile(graph_file), large, pull);
}

static
WGraphHB builtin_loadWeightedEdgesFromFileToHB(const char *graph_file)
{
        if (!graph_file)
                throw std::runtime_error("bad graph file: did you pass a graph argument?");

        return WGraphHB(builtin_loadWeightedEdgesFromFile(graph_file));
}

static
int * builtin_loadFrontierFromFile(const char *frontier_file)
{
        std::vector<int> file_vals;
        std::string   line;
        ifstream file (frontier_file);
        while(std::getline(file, line))
        {
          file_vals.push_back(std::stoi(line));
        }
        int * arr = new int[file_vals.size()];
        std::copy(file_vals.begin(), file_vals.end(), arr);
        return arr;

}

static
GraphHB builtin_transposeHB(GraphHB & graph)
{
        return GraphHB(builtin_transpose(graph.getHostGraph()));
}

static
void builtin_loadMicroCodeFromSTDVectorRValue(std::vector<unsigned char> &&ucode)
{
        Device::Ptr device = Device::GetInstance();
        device->setMicroCode(std::move(ucode));
}

static
void builtin_loadMicroCodeFromSTDVectorCopy(const std::vector<unsigned char> &ucode)
{
        Device::Ptr device = Device::GetInstance();
        std::vector<unsigned char> ucode_cpy = ucode;
        device->setMicroCode(std::move(ucode_cpy));
}

static
void builtin_loadMicroCodeFromSTDVector(std::vector<unsigned char> &ucode)
{
        Device::Ptr device = Device::GetInstance();
        device->setMicroCode(std::move(ucode));
}

static
void builtin_loadMicroCodeFromFile(const std::string &ucode_fname)
{
        /* read in the micro code */
        struct stat st;
        int err = stat(ucode_fname.c_str(), &st);
        if (err != 0) {
                stringstream error;
                error << "Failed to stat micro-code file '" << ucode_fname << "': "
                      << std::string(strerror(errno)) << std::endl;
                throw hammerblade::runtime_error(error.str());
        }

        std::vector<unsigned char> ucode(st.st_size, 0);
        std::fstream ucode_file (ucode_fname);
        ucode_file.read((char*)ucode.data(), ucode.size());

        /* load micro code to device */
        builtin_loadMicroCodeFromSTDVectorRValue(std::move(ucode));
}

//TODO(Emily): ideally this computation would happen on the device
//             so that we can avoid this unnecessary copy
static
int builtin_getVertexSetSizeHB(Vector<int32_t> &frontier, int len){
    Device::Ptr device = Device::GetInstance();
    int size = 0;
    int32_t temp[len];
    frontier.copyToHost(temp, len);
    device->read_dma();
    for(auto i : temp) {
      if(i == 1) {
        size++;
      }
    }
    return size;
}

static
void builtin_addVertexHB(Vector<int32_t> &frontier, int pos)
{
  frontier.insert(pos, 1);
}

static
int builtin_getVerticesHB(GraphHB &g)
{
  return g.num_nodes();
}

static
int builtin_getVerticesHB(WGraphHB &g)
{
  return g.num_nodes();
}

template <typename T>
static
void builtin_swapVectors(Vector<T> &a, Vector<T> &b)
{
  Device::Ptr device = Device::GetInstance();
  int n = a.getLength();
  if(n != b.getLength())
    throw std::runtime_error("vectors are not equal in length.");
  std::vector<T> hosta(n);
  std::vector<T> hostb(n);
  a.copyToHost(hosta.data(), n);
  b.copyToHost(hostb.data(), n);
  std::cout << "copy to host\n";
  device->freeze_cores();
  device->read_dma();
  device->unfreeze_cores();
  a.copyToDevice(hostb.data(), n);
  b.copyToDevice(hosta.data(), n);
  std::cout << "copy from host\n";
  device->freeze_cores();
  device->write_dma();
  device->unfreeze_cores();
}

template<typename T>
Vector<T> getBucketWithGraphItVertexSubset(BucketPriorityQueue<T> &pq){
    return pq.popDenseReadyVertexSet();
}

template<typename T>
void updateBucketWithGraphItVertexSubset(Vector<T> &vset, BucketPriorityQueue<T> &pq)
{
  pq.updateWithDenseVertexSet(vset);
}

static
void deleteObject(Vector<int32_t> &a) {
  a.exit();
}

static void builtin_transpose(GraphHB &graphhb) {
  std::cout << "attempt to transpose\n";
  //builtin_swapVectors<int>(graphhb._in_neighbors, graphhb._out_neighbors);
  //builtin_swapVectors<int>(graphhb._in_index, graphhb._out_index);
  //builtin_swapVectors<vertexdata>(graphhb._in_vertexlist, graphhb._out_vertexlist);
  graphhb.transpose();
}

template <typename T> static void builtin_appendHB(std::vector<std::vector<T>>* vec, Vector<T> &element){
  int n = element.getLength();
  T host_el[n];
  element.copyToHost(host_el, element.getLength());
  vector<T> vect(host_el, host_el + n);
  vec->push_back(vect);
}

template <typename T> static Vector<T> builtin_popHB(std::vector<std::vector<T>> *vec) {
  std::vector<T> host_el = vec->back();
  vec->pop_back();
  Vector<T> temp = Vector<T>(host_el.size());
  temp.copyToDevice(host_el.data(), host_el.size());
  return temp;
}

int calculate_direction(int sz, std::vector<int> frontier, GraphHB & e, int V, int E)
{
  int pull = 0;
  int n = sz;
  int outEdges = 0;
  for(int i = 0; i < V; i++) {
    if(frontier[i] == 1) {
      outEdges += e.out_degree(i); 
    }
  }
  if(n + outEdges > E / 20) pull = 1;
  return pull;
}
int calculate_direction(int sz, std::vector<int> frontier, WGraphHB & e, int V, int E)
{
  int pull = 0;
  int n = sz;
  int outEdges = 0;
  for(int i = 0; i < V; i++) {
    if(frontier[i] == 1) {
      outEdges += e.out_degree(i); 
    }
  }
  if(n + outEdges > E / 20) pull = 1;
  return pull;
}

}

