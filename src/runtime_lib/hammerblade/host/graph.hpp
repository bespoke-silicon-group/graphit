#pragma once
#include <memory>
#include <vector>
#include <iostream>
#include <string>
#include <bsg_manycore.h>
#include <bsg_manycore_loader.h>
#include <bsg_manycore_eva.h>
#include "intrinsics.h"

typedef hb_mc_manycore_t* manycore_ptr;

typedef std::vector<unsigned char> manycore_binary;
typedef std::vector<hb_mc_coordinate_t> tile_list;

template <class Graph>
class HammerBladeGraph {
        Graph g;
        manycore_ptr manycore;

        /* derive some types from our Graph type */
        typedef typename Graph::NodeID NodeID_;
        typedef typename Graph::DestID DestID_;

        manycore_binary object_code;
        tile_list tiles;

        hb_mc_eva_t edgeset_start;
        hb_mc_eva_t out_neighbors;
        hb_mc_eva_t in_neighbors;
        hb_mc_eva_t out_index;
        hb_mc_eva_t in_index;
        hb_mc_eva_t graph_data_end;
        hb_mc_eva_t edgeset_end;
        
        void build_tile_list(void) {
                const hb_mc_config_t *cfg = hb_mc_manycore_get_config(manycore);
                hb_mc_idx_t
                        base_x = hb_mc_config_get_vcore_base_x(cfg),
                        base_y = hb_mc_config_get_vcore_base_y(cfg),
                        ceil_x = hb_mc_coordinate_get_x(hb_mc_config_get_dimension(cfg)),
                        ceil_y = hb_mc_coordinate_get_y(hb_mc_config_get_dimension(cfg));

                tiles.clear();                
                for (hb_mc_idx_t y = base_y; y < ceil_y; y++)
                        for (hb_mc_idx_t x = base_x; x < ceil_x; x++)
                                tiles.push_back(hb_mc_coordinate(x, y));
        }
        
        void load_object_code(void) {
                int err;

                err = hb_mc_loader_load(object_code.data(), object_code.size(),
                                        manycore, &default_map,
                                        tiles.data(), tiles.size());
                if (err != HB_MC_SUCCESS) {
                        std::cerr << std::string(hb_mc_strerror(err)) << std::endl;
                        throw err;
                }
        }

        void load_graph_data(void) {
                /* map graph data onto the manycore DRAM */
                int err = hb_mc_loader_symbol_to_eva(object_code.data(), object_code.size(),
                                                 "_bsg_dram_d_end_addr",
                                                 &edgeset_start);
                if (err != HB_MC_SUCCESS) {
                        std::cerr << std::string(hb_mc_strerror(err)) << std::endl;
                        throw err;
                }

                /* this is where CUDA malloc would come in handy... */
                out_index     = edgeset_start;
                out_neighbors = out_index + (g.num_nodes() * sizeof(NodeID_));
                in_index      = out_neighbors + (g.num_edges() * sizeof(DestID_));
                in_neighbors  = in_index + (g.num_nodes() * sizeof(NodeID_));
                edgeset_end = in_neigbors + (g.num_edges() * sizeof(DestID_));

                /* write out indices */
                err = hb_mc_manycore_eva_write(manycore, &default_map, &tile_list[0], out_index,
                                               g.out_index_, g.num_nodes()*sizeof(NodeID_));
                if (!err != HB_MC_SUCCESS) {
                        std::cerr << std::string(hb_mc_strerror(err)) << std::endl;
                        throw err;
                }

                // blah, blah, blah...
        }
        
public:
        /* base constructors */
        HammerBladeGraph(Graph &g, manycore_ptr mc, manycore_binary &binary) :
                g(std::move(g)), manycore(mc), object_code(std::move(binary)) {
                build_tile_list();
                load_object_code();
                load_graph_data();
        }

        HammerBladeGraph() : manycore(nullptr) {}
        ~HammerBladeGraph() {}

        HammerBladeGraph& operator=(HammerBladeGraph &&other) {
                g = std::move(other.g);
                return *this;
        }

        /* derive all of Graph's helper functions */
        decltype(g.directed()) directed() const {
                return g.directed();
        }
        decltype(g.num_nodes()) num_nodes() const {
                return g.num_nodes();
        }
        decltype(g.num_edges()) num_edges() const {
                return g.num_edges();
        }
        decltype(g.num_edges_directed()) num_edges_directed() const {
                return g.num_edges_directed();
        }
        decltype(g.out_degree(0)) out_degree(NodeID_ v) const {
                return g.out_degree(v);
        }
        decltype(g.in_degree(0)) in_degree(NodeID_ v) const {
                return g.in_degree(v);
        }
        decltype(g.out_neigh(0)) out_neigh(NodeID_ v) const {
                return g.out_neigh(v);
        }
        decltype(g.in_neigh(0)) in_neigh(NodeID_ v) const {
                return g.in_neigh(v);
        }
        void PrintStats() const {
                g.PrintStats();
        }
        void PrintTopology() const {
                g.PrintTopology();
        }
        static DestID_ ** GenIndex(const pvector<SGOffset> &o, DestID_ *neighs) {
                return Graph::GenIndex(o, neighs);
        }
        decltype(g.VertexOffsets()) VertexOffsets(bool in_graph = false) const {
                return g.VertexOffsets(in_graph);
        }
        void SetUpOffsets(bool in_graph = false) {
                return g.in_graph;
        }
        Range<NodeID_> vertices() const {
                return g.vertices();
        }
        decltype(g.getSegmentedGraph("", 0)) getSegmentedGraph(std::string label, int id) {
                return g.getSegmentedGraph(label, id);
        }
        decltype(g.getNumSegments("")) getNumSegments(std::string label) {
                return g.getNumSegments(label);
        }
        void buildPullSegmentedGraphs(std::string label,
                                      int numSegments,
                                      bool numa_aware=false,
                                      std::string path = "") {
                return g.buildPullSegmentedGraphs(label, numSegments, numa_aware, path);
        }
};
