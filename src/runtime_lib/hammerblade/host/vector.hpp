#pragma once
#include <hammerblade/host/device.hpp>
#include <bsg_manycore_cuda.h>
#include <cstddef>

namespace hammerblade {
template <typename T>
class Vector {
	Vector(size_t length) :
		_mem(0), _length(length)
		{ init(); }
	
	Vector() :
		_mem(0), _length(0)
		{ init(); }
	
	~Vector() { exit(); }

	hb_mc_eva_t getBase()   const { return _mem; }
	hb_mc_eva_t getLength() const { return _length; }
	
private:
	void init(void) {
		if (sizeof(T) != 4)
			throw hammerblade::runtime_error("Only Vectors of 4 byte words supported");

		if (_length != 0) {
			Device::Ptr device = Device::GetInstance();
			_mem = device->malloc(_length * sizeof(T));
		}
	}
	void exit(void) {
		if (_length != 0) {
			Device::Ptr device = Device::GetInstance();
			device->free(_mem);
		}
	}
	hb_mc_eva_t _mem;
	size_t _length;
};
};
