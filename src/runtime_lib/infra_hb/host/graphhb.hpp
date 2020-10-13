#pragma once
#include <infra_gapbs/benchmark.h>
#include <infra_hb/host/device.hpp>
#include <infra_hb/host/vector.hpp>


namespace hammerblade {
class GraphHB {
public:

	struct vertexdata {
		int32_t offset;
		int32_t degree;
	};

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

				decltype(Vector<vertexdata>.getBase()) getOutVertexlistAddr() const {
								return _out_vertexlist.getBase();
				}

				decltype(Vec().getBase()) getInIndicesAddr() const {
                return _in_index.getBase();
        }
        decltype(Vec().getBase()) getInNeighborsAddr() const {
                return _in_neighbors.getBase();
        }

				decltype(Vector<vertexdata>.getBase()) getInVertexlistAddr() const {
								return _in_vertexlist.getBase();
				}

				std::vector<int32_t> get_out_degrees() const {
					std::vector<int32_t> out_degrees (_host_g.num_nodes(), 0);
    			for (NodeID n=0; n < _host_g.num_nodes(); n++){
        		out_degrees[n] = _host_g.out_degree(n);
    			}
    			return out_degrees;
        }

				std::vector<int32_t> get_in_degrees() const {
					std::vector<int32_t> in_degrees (_host_g.num_nodes(), 0);
    			for (NodeID n=0; n < _host_g.num_nodes(); n++){
        		in_degrees[n] = _host_g.in_degree(n);
    			}
    			return in_degrees;
        }

        Graph & getHostGraph() {
                return _host_g;
        }

				int64_t out_degree(int v) const {
					return _host_g.out_degree(v);
				}

				int64_t in_degree(int v) const {
					return _host_g.in_degree(v);
				}
private:

	static const hb_mc_eva_t DEVICE_NULLPTR = 0;

	void init() { initGraphOnDevice(); }

	void initGraphOnDevice() {
	  if (true) {
	    //throw hammerblade::runtime_error("transpose not supported");
	    // convert
	    std::vector<int32_t> index(num_nodes() + 1);
			std::vector<int32_t> tmp_deg = this->get_in_degrees();
			std::vector<vertexdata> tmp_vertexlist(num_nodes());
	    # pragma omp parallel for
	    for (int64_t i = 0; i < num_nodes(); i++) {
	      index[i] = _host_g.in_index_[i] - _host_g.in_neighbors_;
				vertexdata tmp_elem = {.offset = index[i], .degree = tmp_deg[i]};
				tmp_vertexlist[i] = tmp_elem;
			}
			index[num_nodes()] = num_edges();
	    // allocate
	    _in_index = Vec(num_nodes() + 1);
	    _in_neighbors = Vec(num_edges());
			_in_vertexlist = Vector<vertexdata>(num_nodes());
	    // copy
	    _in_index.copyToDevice(index.data(), index.size());
	    _in_neighbors.copyToDevice(_host_g.in_neighbors_, num_edges());
			_in_vertexlist.copyToDevice(tmp_vertexlist.data(), tmp_vertexlist.size());
	  }

	  // out neighbors
	  std::vector<int32_t> index(num_nodes() + 1);
		std::vector<int32_t> tmp_deg = this->get_out_degrees();
		std::vector<vertexdata> tmp_vertexlist(num_nodes());
	  #pragma omp parallel for
	  for (int64_t i = 0; i < num_nodes(); i++) {
	  	index[i] = _host_g.out_index_[i] - _host_g.out_neighbors_;
			vertexdata tmp_elem = {.offset = index[i], .degree = tmp_deg[i]};
			tmp_vertexlist[i] = tmp_elem;
		}
		index[num_nodes()] = num_edges();
	  //allocate
	  _out_index = Vec(num_nodes() + 1);
	  _out_neighbors = Vec(num_edges());
		_out_vertexlist = Vector<vertexdata>(num_nodes());

	  //copy
	  _out_index.copyToDevice(index.data(), index.size());
	  _out_neighbors.copyToDevice(_host_g.out_neighbors_, num_edges());
		_out_vertexlist.copyToDevice(tmp_vertexlist.data(), tmp_vertexlist.size());
	}

	void exit() { freeGraphOnDevice(); }

        void freeGraphOnDevice() {}

	bool isTranspose() const { return _host_g.is_transpose_; }

	Graph _host_g;
        Vec   _out_index;
        Vec   _out_neighbors;
        Vec   _in_index;
        Vec   _in_neighbors;
				Vector<vertexdata> _out_vertexlist;
				Vector<vertexdata> _in_vertexlist;

        void moveFrom(GraphHB & other) {
                _host_g = std::move(other._host_g);
                _out_index = std::move(other._out_index);
                _out_neighbors = std::move(other._out_neighbors);
                _in_index = std::move(other._in_index);
                _in_neighbors = std::move(other._in_neighbors);
                _in_vertexlist = std::move(other._in_vertexlist);
                _out_vertexlist = std::move(other._out_vertexlist);
        }
};
}
