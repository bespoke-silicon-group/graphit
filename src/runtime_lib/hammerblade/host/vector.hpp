#pragma once
#include <hammerblade/host/device.hpp>
#include <bsg_manycore_cuda.h>
#include <cstddef>
#include <sstream>

namespace hammerblade {
template <typename T>
class Vector {
public:
        class out_of_bounds : public hammerblade::runtime_error {
        public:
                explicit out_of_bounds(size_t pos, size_t n) :
                        hammerblade::runtime_error(
                                //lambda -- because string formatting in C++ is not a one-liner
                                [=](){
                                        std::stringstream ss;
                                        ss << pos << " exceeds vector size " << n;
                                        return ss.str();
                                }()) {}
        };

        Vector () :
                _mem(0),
                _length(0),
                _device(Device::GetInstance()) {
                init();
        }
	Vector(size_t length) :
		Vector() {
                _length = length;
                init();
        }
        Vector(size_t length, T val) :
                Vector(length) {
                assign(0, length, val);
        }

        Vector(Vector &&other) :
                _device(Device::GetInstance())
                { moveFrom(other); }

        /* we don't allow copying (because it will either be confusing or slow) */
        Vector(const Vector &other) = delete;


        Vector & operator=(Vector && other) {
                moveFrom(other);
                return *this;
        }

	~Vector() { exit(); }

        /* accessors for getting the base address of the vector */
        hb_mc_eva_t getAddr()   const { return getBase(); }
	hb_mc_eva_t getBase()   const { return _mem; }

        /* accessors for getting the length of the vector */
	hb_mc_eva_t getLength() const { return _length; }

        /**/
        void assign(size_t start, size_t end, const T &val) {
                if (start >= getLength())
                        throw out_of_bounds(start, getLength());
                if (end > getLength())
                        throw out_of_bounds(end, getLength());

                for (size_t i = start; i < end; i++)
                        insert(i, val);
        }
        /**
         * Read value at #pos.
         */
        T at(size_t pos) const {
                if (pos >= _length)
                        throw Vector::out_of_bounds(pos, _length);

                T val;
                getDevice()->read((void*)&val, _mem + (pos * sizeof(T)), sizeof(T));
                return val;
        }

        /**
         * Write a value at #pos.
         */
        void insert(size_t pos, const T & val) {
                if (pos >= _length)
                        throw Vector::out_of_bounds(pos, _length);

                getDevice()->write(_mem + (pos * sizeof(T)), (const void*)&val, sizeof(T));
        }

        /* array copy from hammerblade to the host */
        void copyToHost(T * host, size_t n) const {
                if (n > _length)
                        throw Vector::out_of_bounds(n, _length);

                getDevice()->read((void*)host, _mem, n * sizeof(T));
        }

        /* array copy to hammerblade memory */
        void copyToDevice(const T *host, size_t n) {
                if (n > _length)
                        throw Vector::out_of_bounds(n, _length);

                getDevice()->write(_mem, (const void*)host, n * sizeof(T));
        }

        //making public to allow teardown of parvector type
        void exit(void) {
      		if (_length != 0) {
                              getDevice()->free(_mem);
      		}
      	}

private:
        /* swap the contents of two vectors */
        void swap(const Vector &other) {
                std::swap(other._mem, _mem);
                std::swap(other._length, _length);
                std::swap(other._device, _device);
        }

        /* common code for implementing move semantics in operator= and move constructor */
        void moveFrom(Vector &other) {
                _mem    = other._mem;
                _length = other._length;
                other._mem = 0;
                other._length = 0;
        }

        /* initialize the vector's memory on HammerBlade hardware */
	void init(void) {
		if (sizeof(T) != 4)
			throw hammerblade::runtime_error("Only Vectors of 4 byte words supported");

		if (_length != 0) {
			_mem = getDevice()->malloc(_length * sizeof(T));
		}
	}
        /* cleanup the vector's memory */


        /* accessors for the device */
        Device::Ptr & getDevice() { return _device; }
        const Device::Ptr & getDevice() const { return _device; }

        Device::Ptr _device; //!< HammerBlade hardware controller
	hb_mc_eva_t _mem;    //!< the base address of this vector (HammerBlade U-Processor virtual address)
	size_t      _length; //!< the length of this vector (number of T values)
};
};
