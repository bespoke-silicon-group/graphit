import unittest
import subprocess
import os
import shutil
import sys

GRAPHIT_BUILD_DIRECTORY="${GRAPHIT_BUILD_DIRECTORY}".strip().rstrip("/")
GRAPHIT_SOURCE_DIRECTORY="${GRAPHIT_SOURCE_DIRECTORY}".strip().rstrip("/")
CXX_COMPILER="${CXX_COMPILER}"

NVCC_COMPILER="${NVCC_COMPILER}"

class TestGPURuntimeLibrary(unittest.TestCase):
	@classmethod
	def get_command_output_class(self, command):
		output = ""
		if isinstance(command, list):
			proc = subprocess.Popen(command, stdout=subprocess.PIPE)
		else:
			proc = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
		exitcode = proc.wait()
		for line in proc.stdout.readlines():
			if isinstance(line, bytes):
				line = line.decode()
			output += line.rstrip() + "\n"

		proc.stdout.close()
		return exitcode, output

	def get_command_output(self, command):
		(exitcode, output) = self.get_command_output_class(command)
		self.assertEqual(exitcode, 0)
		return output

	@classmethod	
	def setUpClass(cls):
		if NVCC_COMPILER == "":
			print ("Cannot find CUDA compiler")
			exit(-1)	

		cls.build_directory = GRAPHIT_BUILD_DIRECTORY
		cls.scratch_directory = GRAPHIT_BUILD_DIRECTORY + "/scratch"
		if os.path.isdir(cls.scratch_directory):
			shutil.rmtree(cls.scratch_directory)
		os.mkdir(cls.scratch_directory)
		
		cls.nvcc_command = NVCC_COMPILER + " -ccbin " + CXX_COMPILER + " "
		cls.test_input_directory = GRAPHIT_SOURCE_DIRECTORY + "/test/gpu_tests/test_input"
		
		cls.get_command_output_class(cls.nvcc_command + cls.test_input_directory + "/obtain_gpu_cc.cu -o " + cls.scratch_directory + "/obtain_gpu_cc")
		output = cls.get_command_output_class(cls.scratch_directory + "/obtain_gpu_cc")[1].split()

		if len(output) != 2:
			print ("Cannot obtain GPU information")
			exit(-1)
		compute_capability = output[0]
		num_of_sm = output[1]
		
		cls.nvcc_command += " -DNUM_CTA=" + num_of_sm + " -DCTA_SIZE=1024 -gencode arch=compute_" + compute_capability + ",code=sm_" + compute_capability
		cls.nvcc_command += " -std=c++11 -O3 -I " + GRAPHIT_SOURCE_DIRECTORY + "/src/runtime_lib/ -Xcompiler \"-w\" -Wno-deprecated-gpu-targets "
		
		shutil.copytree(GRAPHIT_SOURCE_DIRECTORY + "/test/graphs", cls.scratch_directory + "/graphs")
		cls.graph_directory = cls.scratch_directory + "/graphs"
		
		cls.executable_name = cls.scratch_directory + "/test_exectuable"
	
	def cpp_compile_test(self, input_file_name, extra_cpp_args=[]):
		compile_command = self.nvcc_command + self.test_input_directory + "/" + input_file_name + " -o " + self.executable_name + " " + " ".join(extra_cpp_args)
		self.get_command_output(compile_command)
	
	def cpp_exec_test(self, input_file_name, extra_cpp_args=[], extra_exec_args=[]):
		self.cpp_compile_test(input_file_name, extra_cpp_args)
		return self.get_command_output(self.executable_name + " " + " ".join(extra_exec_args))
			
	def test_basic_compile(self):
		self.cpp_compile_test("basic_compile.cu")
	def test_basic_load_graph(self):
		output = self.cpp_exec_test("basic_load_graph.cu", [], [self.graph_directory + "/4.mtx"])
		output = output.split("\n")
		self.assertEqual(len(output), 3)
		self.assertEqual(output[1], "14, 106")

if __name__ == '__main__':
	unittest.main()