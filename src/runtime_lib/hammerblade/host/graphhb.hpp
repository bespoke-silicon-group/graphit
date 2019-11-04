#pragma once
#include <infra_gapbs/benchmark.h>
#include <hammerblade/host/device.hpp>
#include <hammerblade/host/vector.hpp>

namespace hammerblade {
class GraphHB {
public:
	GraphHB() {}

        GraphHB(Graph &&g) :
                _host_g(std::move(g))
                { init(); }

	~GraphHB() {}

        /* no copying */
	GraphHB(const GraphHB &other) = delete;

        GraphHB(GraphHB &&other) {
                moveFrom(other);
        }

        GraphHB &operator=(GraphHB &&other) {
                moveFrom(other);
                return *this;
        }

        decltype(Graph().num_nodes()) num_nodes() const {
                return _host_g.num_nodes();
        }

        decltype(Graph().num_nodes()) num_edges() const {
                return _host_g.num_edges_;
        }

        using Vec = Vector<int32_t>;
        decltype(Vec().getBase()) getOutIndicesAddr() const {
                return _out_index.getBase();
        }
        decltype(Vec().getBase()) getOutNeighborsAddr() const {
                return _out_neighbors.getBase();
        }

				decltype(Vec().getBase()) getInIndicesAddr() const {
                return _in_index.getBase();
        }
        decltype(Vec().getBase()) getInNeighborsAddr() const {
                return _in_neighbors.getBase();
        }

        const Graph & getHostGraph() const {
                return _host_g;
        }

				int64_t out_degree(int v) const {
					return _host_g.out_degree(v);
				}

private:

	static const hb_mc_eva_t DEVICE_NULLPTR = 0;

	void init() { initGraphOnDevice(); }

	void initGraphOnDevice() {
                if (isTranspose()) {
                        throw hammerblade::runtime_error("transpose not supported");
                        // convert
                        std::vector<int32_t> index(num_nodes());

                        # pragma omp parallel for
                        for (int64_t i = 0; i < num_nodes(); i++)
                                index[i] = _host_g.in_index_[i] - _host_g.in_neighbors_;

                        // allocate
                        _in_index = Vec(num_nodes());
                        _in_neighbors = Vec(num_edges());
                        // copy
                        _in_index.copyToDevice(index.data(), index.size());
                        _in_neighbors.copyToDevice(_host_g.in_neighbors_, num_edges());
                }

                // out neighbors
                std::vector<int32_t> index(num_nodes());

                #pragma omp parallel for
                for (int64_t i = 0; i < num_nodes(); i++)
                        index[i] = _host_g.out_index_[i] - _host_g.out_neighbors_;

                //allocate
                _out_index = Vec(num_nodes());
                _out_neighbors = Vec(num_edges());
                //std::cerr << "index size = " << index.size() << std::endl;
                //std::cerr << "_out_index length = " << _out_index.getLength() << std::endl;
                //copy
                _out_index.copyToDevice(index.data(), index.size());
                _out_neighbors.copyToDevice(_host_g.out_neighbors_, num_edges());
        }

	void exit() { freeGraphOnDevice(); }

        void freeGraphOnDevice() {}

	bool isTranspose() const { return _host_g.is_transpose_; }

	Graph _host_g;
        Vec   _out_index;
        Vec   _out_neighbors;
        Vec   _in_index;
        Vec   _in_neighbors;

        void moveFrom(GraphHB & other) {
                _host_g = std::move(other._host_g);
                _out_index = std::move(other._out_index);
                _out_neighbors = std::move(other._out_neighbors);
                _in_index = std::move(other._in_index);
                _in_neighbors = std::move(other._in_neighbors);
        }
};
}
