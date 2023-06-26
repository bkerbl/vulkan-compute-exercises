#include <vulkan/vulkan.hpp>
#include <framework.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <random>

int main(int argc, char* argv[])
{
	int N = 1024;
	std::default_random_engine eng(42);
	std::uniform_real_distribution<float> dist;
	std::vector<float> A(N * N), B(N * N), C(N * N);
	for (int i = 0; i < N * N; i++)
	{
		A[i] = dist(eng);
		B[i] = dist(eng);
		C[i] = 0;
	}

	try
	{
		vk::UniqueInstance instance;
		vk::PhysicalDevice physicalDevice;
		vk::UniqueDevice device;
		vk::UniqueCommandPool commandPool;
		vk::Queue queue;

		Framework::setupBasicCompute("Task 9", VK_API_VERSION_1_3, {}, {}, instance, physicalDevice, device, queue, commandPool);

		vk::UniqueShaderModule shaderModule;
		vk::UniquePipelineLayout layout;
		vk::UniquePipelineCache cache;
		vk::UniquePipeline pipeline;

		Framework::setupComputePipeline("matrixmult.comp.spv", {}, *device, shaderModule, layout, cache, pipeline);

		std::vector<float> C_computed_by_GPU(N * N, 0);

		// TODO: Set up all necessary resources and prepare them to do NxN matrix
		// mutliplication C = A * B on the GPU. Compare timing to CPU baseline. To 
		// (somewhat) accurately measure GPU time, use chrono to capture a timestamp 
		// before you submit the command buffer and then again AFTER you synchronized 
		// CPU and GPU, and compute the difference. Copy your result from the GPU into
		// C_computed_by_GPU. The error to the CPU result should be low (< 0.001).
		// Please note that he CPU computation is slooow, especially in Debug mode.

		auto beforeCPU = std::chrono::system_clock::now();
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
				for (int k = 0; k < N; k++)
					C[i * N + j] += A[i * N + k] * B[k * N + j];
		auto afterCPU = std::chrono::system_clock::now();
		std::cout << "CPU time: " << std::chrono::duration_cast<std::chrono::milliseconds>(afterCPU - beforeCPU).count() << " ms\n";
		
		float error = 0.0f;
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
				error += std::abs(C_computed_by_GPU[i * N + j] - C[i * N + j]);

		std::cout << "Avg. CPU-GPU Difference: " << error / (N * N) << std::endl;
	}
	catch (Framework::NotImplemented e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

/*
==================================== Task 9 ====================================
0) Read up on "efficient" matrix multiplication on the GPU in the CUDA guide:
https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#shared-memory
1) Prepare the environment and necessary GPU resources for matrix multiplication.
2) Implement the corresponding GLSL shader code in matrixmult.comp.
3) Optional: Compare against a version that does not exploit shared memory. Can
you see a clear difference? HINT: You might want to start with this simpler 
version anyway before trying matrix multiplication with shared memory. Also:
if you want fast performance, matrix memory should probably be device-local!
*/