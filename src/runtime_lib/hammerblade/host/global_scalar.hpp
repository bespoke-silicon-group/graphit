#pragma once
#include <hammerblade/host/device.hpp>
#include <bsg_manycore_cuda.h>
#include <string>
#include <type_traits>

namespace hammerblade {
/**
 * A wrapper class for accessing global variables statically allocated on the device.
 */
template <typename T>
class GlobalScalar {
        static_assert(std::is_scalar<T>::value && sizeof(T) == sizeof(hb_mc_eva_t),
                      "type template for GlobalScalar must be built-in "
                      "scalar type and the size must match HammerBlade machine word size");
public:
        /**
         * Empty constructor.
        */
        GlobalScalar() : GlobalScalar("") {}
        /**
         * Constructor from std::string.
         * param[in] varname is the name of the global on the device.
         */
        explicit GlobalScalar(const std::string & varname) :
                _device(Device::GetInstance()),
                _varname(varname) {}

        /**
         * Set the value of a global.
         * @param[in] the new value of the global.
         */
        void set(const T &v) {
                Device::Ptr device = Device::GetInstance();
                hb_mc_eva_t addr = device->findSymbolAddress(_varname);
                device->write(addr, &v, sizeof(T));

        }
        /**
         * Get the value of a global.
         * @return the value of the global.
         */
        T get () const {
                Device::Ptr device = Device::GetInstance();
                hb_mc_eva_t addr = device->findSymbolAddress(_varname);
                T v;
                device->read(&v, addr, sizeof(T));
                return v;
        }
        /**
         * Get the size of a global.
         * @return the size in bytes of the global.
         */
        size_t scalar_size() const { return sizeof(T); }
private:
        std::string _varname;
        Device::Ptr _device;
};
}