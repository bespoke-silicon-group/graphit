#pragma once
#include <hammerblade/host/device.hpp>
#include <hammerblade/host/vector.hpp>
#include <bsg_manycore_cuda.h>
#include <cstddef>
#include <sstream>

namespace hammerblade {
template <typename T>
class ParVector {

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

  ParVector () :
          _blocks(1),
          _length(0) {
          _vec = new Vector<T>();
  }

  ParVector (size_t length, size_t blocks) :
          _blocks(blocks),
          _length(length) {
            _vec = new Vector<T>(length * blocks);
          }

  ParVector (size_t length, size_t blocks, T val) :
          _length(length),
          _blocks(blocks) {
          _vec = new Vector<T>((length * blocks), val);
          }

  ParVector (ParVector &&other) {
    moveFrom(other);
  }

  ParVector(const ParVector &other) = delete;

  ParVector & operator=(ParVector && other) {
          moveFrom(other);
          return *this;
  }

  ~ParVector() { exit(); }

  hb_mc_eva_t getAddr()   const { return _vec->getBase(); }
  hb_mc_eva_t getBase()   const { return _vec->getBase(); }

  hb_mc_eva_t getLength() const { return _length; }

  void assign(size_t start, size_t end, const T &val) {
          if (start >= getLength())
                  throw ParVector::out_of_bounds(start, getLength());
          if (end > getLength())
                  throw ParVector::out_of_bounds(end, getLength());

          _vec->assign(start, end, val);
  }

  T at(size_t pos) const {
          if (pos >= _length)
                  throw ParVector::out_of_bounds(pos, _length);

          return(_vec->at(pos));
  }

  void insert(size_t pos, const T & val) {
          if(pos >= _length)
                  throw ParVector::out_of_bounds(pos, _length);
          //NOTE(Emily): do we want to insert for every thread's replication?
          _vec->insert(pos, val);
  }

  void copyToHost(T * host, size_t n) const {
          if(n > (_length * _blocks))
                  throw ParVector::out_of_bounds(n, (_length * _blocks));
          _vec->copyToHost(host, n);
  }

  void copyToDevice(const T * host, size_t n) {
          if(n > (_length * _blocks))
                  throw ParVector::out_of_bounds(n, (_length * _blocks));
          _vec->copyToDevice(host, n);
  }

private:

  void swap(const ParVector &other) {
    std::swap(other._blocks, _blocks);
    std::swap(other._length, _length);
    _vec->swap(other._vec);
  }

  void moveFrom(ParVector &other) {
    _blocks = other._blocks;
    _length = other._length;
    _vec->moveFrom(other._vec);
  }

  void init(void) {
    _vec->init();
  }

  void exit(void) {
    _vec->exit();
  }

  size_t      _blocks; //!< the number of threads accessing this vec
  size_t      _length; //!< the number of elements in the array (not * _blocks yet)
  Vector<T> *  _vec; //!< the actual Vector item storing the data
};
};
