#pragma once
#include <bsg_manycore_errno.h>
#include <bsg_manycore.h>
#include <bsg_manycore_cuda.h>
#include <hammerblade/host/error.hpp>
#include <memory>

/*
 * grid_init() -> after program_init_binary()
 * initialize a grid of tile groups.
 * I only want one, so make it 1x1 and make each tg 3x3.
 */
namespace hammerblade {
class Device {
public:
	/* Singleton object with automatic reference counting */
	typedef std::shared_ptr<Device>       Ptr;
	typedef std::shared_ptr<const Device> ConstPtr;
	/* no copy; no move */
	Device(Device && d)      = delete;
	Device(const Device & d) = delete;
	/* singleton object */
	static Device::Ptr GetInstance() {
		static Device::Ptr instance = nullptr;
		if (!instance)
			instance = Device::Ptr(new Device);
		return instance;
	}

	/* set the micro code data in vector */
	void setMicroCode(std::vector<unsigned char> && ucode) {
		_ucode = ucode;
		updateMicroCode();
	}

	
	/* void enqueue a CUDA task */
	/* should only be called after the micro code has been set */
	void enqueueJob(const std::string &kernel_name,
			std::vector<uint32_t> argv) {
		int err;

		if (_ucode.empty())
			throw noUCodeError();
		/* 
		 * this is necessary because of a bug in CUDA-lite
		 * where argv is not saved as a deep copy but as 
		 * a pointer
		 */
		_argv_saves.push_back(std::move(argv));
		
		err = hb_mc_grid_init(_device,
				      hb_mc_dimension(1,1), /* grid of tile groups  */
				      hb_mc_dimension(3,3), /* tile group dimension */
				       // TODO: cast is required because of bug in CUDA-lite
				      (char*)kernel_name.c_str(),
				      _argv_saves.back().size(),
				      _argv_saves.back().data());
		
		if (err != HB_MC_SUCCESS)
			throw hammerblade::manycore_runtime_error(err);
		
		
	}

	/*
	 * run enqueued jobs
	 */
	void runJobs() {
		int err;

		if (_ucode.empty())
			throw noUCodeError();
		
		err = hb_mc_device_tile_groups_execute(_device);
		if (err != HB_MC_SUCCESS)
			throw hammerblade::manycore_runtime_error(err);
		/*
		 * argv saves can now be released
		 */
		_argv_saves.clear();
	}

	/*
	 * allocate some application memory on the device
	 */
	hb_mc_eva_t malloc(hb_mc_eva_t sz) {
		if (_ucode.empty())
			throw noUCodeError();
		
		return 0;
	}

	/*
	 * free application memory on the device
	 */
	void free(hb_mc_eva_t) {
		if (_ucode.empty())
			throw noUCodeError();
	}

	/*
	 * write data to application memory
	 */
	void write(hb_mc_eva_t dst, const void *src, hb_mc_eva_t sz) {
		if (_ucode.empty())
			throw noUCodeError();
	}
	
	/*
	 * read data from application memory
	 */
	void read(void *dst, hb_mc_eva_t src, hb_mc_eva_t sz) {
		if (_ucode.empty())
			throw noUCodeError();
	}
	
private:
	hammerblade::runtime_error noUCodeError() const {
		return hammerblade::runtime_error("Device not initialized with microcode");
	}
	static char * CUDA_DEVICE_NAME() {
		static char cuda_device_name [] = "GraphIt";
		return cuda_device_name;
	}
	static char * CUDA_PROGRAM_NAME() {
		static char cuda_program_name [] = "GraphIt Micro Code";
		return cuda_program_name;
	}
	static char *CUDA_ALLOC_NAME() {
		static char cuda_alloc_name [] = "GraphIt Allocator";
		return cuda_alloc_name;
	}

	/* update the micro code on the device */
	void updateMicroCode() {
		int err;

		err = hb_mc_device_program_init_binary(_device,
						       CUDA_PROGRAM_NAME(),
						       (char*)_ucode.data(),
						       _ucode.size(),
						       CUDA_ALLOC_NAME(),
						       0);
		if (err != HB_MC_SUCCESS)
			throw hammerblade::manycore_runtime_error(err);		
	}
	
protected:
	/* singleton; constructor is protected */
	Device() : _device(nullptr) {
		int err;

		/* allocate and initialize CUDA-lite handles */
		_device = new hb_mc_device_t;

		err = hb_mc_device_init(_device, CUDA_DEVICE_NAME(), 0, hb_mc_coordinate(4,4));
		if (err != HB_MC_SUCCESS)
			throw hammerblade::manycore_runtime_error(err);

		/* make sure ucode is cleared */
		_ucode.clear(); // probably not necessary
	}
public:
	~Device() {
		int err;

		/* cleanup and free CUDA-lite handles */
		err = hb_mc_device_finish(_device);
		if (err != HB_MC_SUCCESS)
			throw hammerblade::manycore_runtime_error(err);
		
		delete _device;
	}
private:
	std::vector< std::vector <uint32_t> > _argv_saves;
	std::vector<unsigned char> _ucode;
	hb_mc_device_t * _device;
};
}
