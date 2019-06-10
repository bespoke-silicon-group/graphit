#pragma once
#include "intrinsics.h"

template <class Graph>
class HammerBladeGraph {
        Graph g;
        
        /* derive some types from our Graph type */
        typedef typename Graph::NodeID NodeID_;
        typedef typename Graph::DestID DestID_;        
        
public:        
        /* base constructors */
        HammerBladeGraph()  {}
        HammerBladeGraph(Graph &&g) : g(std::move(g)) {}
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
