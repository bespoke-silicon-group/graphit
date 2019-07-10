#pragma once
#include <hammerblade/host/graphhb.hpp>
#include <hammerblade/host/device.hpp>
#include <hammerblade/host/error.hpp>
#include <fstream>
#include <vector>
#include <sys/stat.h>
#include "intrinsics.h"
namespace hammerblade {

static
GraphHB builtin_loadEdgesFromFileToHB(const char *graph_file)
{
	return GraphHB(builtin_loadEdgesFromFile(graph_file));
}

static
void builtin_loadMicroCodeFromSTDVectorRValue(std::vector<unsigned char> &&ucode)
{
	Device::Ptr device = Device::GetInstance();
	device->setMicroCode(std::move(ucode));
}

static
void builtin_loadMicroCodeFromSTDVectorCopy(const std::vector<unsigned char> &ucode)
{
	Device::Ptr device = Device::GetInstance();
	std::vector<unsigned char> ucode_cpy = ucode;
	device->setMicroCode(std::move(ucode_cpy));
}

static
void builtin_loadMicroCodeFromSTDVector(std::vector<unsigned char> &ucode)
{
	Device::Ptr device = Device::GetInstance();
	device->setMicroCode(std::move(ucode));
}

static
void builtin_loadMicroCodeFromFile(const std::string &ucode_fname)
{
	/* read in the micro code */
	struct stat st;
	int err = stat(ucode_fname.c_str(), &st);
	if (err != 0) {
		stringstream error;
		error << "Failed to stat micro-code file '" << ucode_fname << "': "
		      << std::string(strerror(errno)) << std::endl;
		throw hammerblade::runtime_error(error.str());
	}

	std::vector<unsigned char> ucode(st.st_size, 0);
	std::fstream ucode_file (ucode_fname);
	ucode_file.read((char*)ucode.data(), ucode.size());

	/* load micro code to device */
	builtin_loadMicroCodeFromSTDVectorRValue(std::move(ucode));
}
}
