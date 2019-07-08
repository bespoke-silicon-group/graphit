#pragma once
#include <infra_gapbs/benchmark.h>
#include <hammerblade/host/device.hpp>
namespace hammerblade {
class GraphHB {
public:
	GraphHB() {}
	GraphHB(Graph &&g)
		: _host_g(std::move(g))
		{ init(); }
	GraphHB(GraphHB && other)
		: _host_g(std::move(other._host_g))
		{ exit(); }

	~GraphHB() {}
        /* no copying */
	GraphHB(const GraphHB &other) = delete;

	GraphHB operator=(GraphHB &&other) {
		return std::move(other);
	}
private:
	static const hb_mc_eva_t DEVICE_NULLPTR = 0;

	void init() { initGraphOnDevice(); }

	void initGraphOnDevice() {
		Device::Ptr device = Device::GetInstance();
		_device_out_neighbors = DEVICE_NULLPTR;
		_device_out_offsets   = DEVICE_NULLPTR;
		_device_in_neighbors  = DEVICE_NULLPTR;
		_device_in_offsets    = DEVICE_NULLPTR;
		_device_out_neighbors_sz = 0;
		_device_out_offsets_sz   = 0;
		_device_in_neighbors_sz  = 0;
		_device_in_offsets_sz    = 0;

		/* allocate memory on device */
		if (isTranspose()) {
			_device_in_neighbors  = device->malloc(_device_in_neighbors_sz);
			_device_in_offsets    = device->malloc(_device_in_offsets_sz);
		} else {
			_device_out_neighbors = device->malloc(_device_out_neighbors_sz);
			_device_out_offsets   = device->malloc(_device_out_offsets_sz);
		}

		/* copy data to device */
		if (isTranspose()) {
			device->write(_device_in_neighbors,
				      _host_g.in_neighbors_,
				      _device_in_neighbors_sz);

			device->write(_device_in_offsets,
				      _host_g.in_index_,
				      _device_in_offsets_sz);
		} else {
			device->write(_device_out_neighbors,
				      _host_g.out_neighbors_,
				      _device_out_neighbors_sz);

			device->write(_device_out_offsets,
				      _host_g.out_index_,
				      _device_out_offsets_sz);
		}
	}

	void exit() { freeGraphOnDevice(); }

	void freeGraphOnDevice() {
		Device::Ptr device = Device::GetInstance();

		/* free allocated data on the device */
		if (isTranspose()) {
			device->free(_device_in_neighbors);
			device->free(_device_in_offsets);
		} else {
			device->free(_device_out_neighbors);
			device->free(_device_out_offsets);
		}
	}

	bool isTranspose() const { return _host_g.is_transpose_; }

	Graph _host_g;
	hb_mc_eva_t _device_out_neighbors;
	hb_mc_eva_t _device_out_neighbors_sz;
	hb_mc_eva_t _device_out_offsets;
	hb_mc_eva_t _device_out_offsets_sz;
	hb_mc_eva_t _device_in_neighbors;
	hb_mc_eva_t _device_in_neighbors_sz;
	hb_mc_eva_t _device_in_offsets;
	hb_mc_eva_t _device_in_offsets_sz;

};
}
