#pragma once
#include <hammerblade/host/device.hpp>
#include <bsg_manycore_cuda.h>
#include <cstddef>

namespace hammerblade {
template <typename T>
class Vector {
public:
	Vector(size_t length = 0) :
		_mem(0), _length(length), _device(nullptr)
		{ init(); }

        Vector(Vector &&other)  {
                moveFrom(other);
        }

        Vector(const Vector &other) = delete;

        Vector & operator=(Vector && other) {
                moveFrom(other);
                return *this;
        }

	~Vector() { exit(); }


	hb_mc_eva_t getBase()   const { return _mem; }
	hb_mc_eva_t getLength() const { return _length; }

        void copyToHost(T * host, size_t n) const {
                if (n > _length)
                        throw hammerblade::runtime_error("Device buffer overflow");

                getDevice()->read((void*)host, _mem, n * sizeof(T));
        }

        void copyToDevice(const T *host, size_t n) {
                if (n > _length)
                        throw hammerblade::runtime_error("Device buffer overflow");

                getDevice()->write(_mem, (const void*)host, n * sizeof(T));
        }

private:
        void swap(const Vector &other) {
                std::swap(other._mem, _mem);
                std::swap(other._length, _length);
                std::swap(other._device, _device);
        }

        void moveFrom(Vector &other) {
                _mem    = other._mem;
                _length = other._length;
                _device = other._device;
                other._mem = 0;
                other._length = 0;
                other._device = nullptr;
        }

	void init(void) {
		if (sizeof(T) != 4)
			throw hammerblade::runtime_error("Only Vectors of 4 byte words supported");

		if (_length != 0) {
			_mem = getDevice()->malloc(_length * sizeof(T));
		}
	}
	void exit(void) {
		if (_length != 0) {
                        getDevice()->free(_mem);
		}
	}

        Device::Ptr & getDevice() {
                if (_device == nullptr)
                        _device = Device::GetInstance();
                return _device;
        }

        Device::Ptr _device;
	hb_mc_eva_t _mem;
	size_t _length;
};
};
