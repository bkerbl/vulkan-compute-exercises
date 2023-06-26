#include <vulkan/vulkan.hpp>
#include <framework.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <sstream>
#include <iomanip>

int main(int argc, char* argv[])
{
	int N = 20'000'000;
	std::default_random_engine eng(42);
	std::uniform_real_distribution<float> dist;
	std::vector<float> input(N);
	double full_sum = 0.0;
	std::cout << "Computing reference value..." << std::endl;
	for (int i = 0; i < N; i++)
	{
		input[i] = dist(eng);
		full_sum += input[i];
	}
	double reference_sum = 0;
	for (int i = 0; i < N; i++)
	{
		input[i] = (float)((input[i] * 42.0) / full_sum);
		reference_sum += input[i];
	}
	std::cout << "CPU reference (double): " << reference_sum << std::endl;

	try
	{
		vk::UniqueInstance instance;
		vk::PhysicalDevice physicalDevice;
		vk::UniqueDevice device;
		vk::UniqueCommandPool commandPool;
		vk::Queue queue;

		Framework::setupBasicCompute("Task 10", VK_API_VERSION_1_3, {}, { VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME }, instance, physicalDevice, device, queue, commandPool);

		vk::UniqueShaderModule shaderModule;
		vk::UniquePipelineLayout layout;
		vk::UniquePipelineCache cache;
		vk::UniquePipeline pipeline;

		// TODO: Set up all necessary resources and prepare them to do a reduction
		// of the input array as fast as you can manage! Pick the proper resources
		// and memory and initialize their contents. Prepare a descriptor set layout,
		// descriptor set and record a command buffer.
		//
		// TODO: Make sure to copy all the data from the CPU input vector to your
		// GPU-side buffers, as well as necessary parameters (check the shader source).

		vk::UniqueDescriptorSetLayout descriptorSetLayout;
		vk::UniqueDescriptorPool descriptorPool;
		vk::UniqueDescriptorSet descriptorSet;

		Framework::setupComputePipeline("reduce.comp.spv", { *descriptorSetLayout }, * device, shaderModule, layout, cache, pipeline);

		auto beforeCPU = std::chrono::system_clock::now();
		float input_sum = 0.0f;
		for (int i = 0; i < N; i++)
			input_sum += input[i];
		auto afterCPU = std::chrono::system_clock::now();
		std::cout << "CPU time: " << std::chrono::duration_cast<std::chrono::milliseconds>(afterCPU - beforeCPU).count() << " ms\n";
		std::cout << "CPU reduction (float): " << input_sum << std::endl;

		auto beforeGPU = std::chrono::system_clock::now();

		// TODO: Set up your submission and submit your command buffer! 

		vk::CommandBufferAllocateInfo allocateInfo(*commandPool, vk::CommandBufferLevel::ePrimary, 1);
		auto cmdBuffers = device->allocateCommandBuffersUnique(allocateInfo);

		Framework::initDebugging();
		Framework::beginCapture();
		queue.submit(vk::SubmitInfo({}, {}, *cmdBuffers[0]));
		Framework::endCapture();

		device->waitIdle();

		// TODO: read back the result of the reduction from the GPU.
		// Store it in "input_sum_gpu".

		float input_sum_gpu = 0;

		auto afterGPU = std::chrono::system_clock::now();
		std::cout << "GPU time: " << std::chrono::duration_cast<std::chrono::milliseconds>(afterGPU - beforeGPU).count() << " ms\n";
		std::cout << "GPU reduction (float): " << input_sum_gpu << std::endl;
	}
	catch (Framework::NotImplemented e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

/*
==================================== Task 10 ====================================
1) Prepare the environment and necessary GPU resources for reduction (addition).
2) Implement the corresponding GLSL shader code in reduce.comp.
3) Make sure that your solution is as fast as you can manage. Be sure to pick the
proper memory types for your buffers and that your shader uses fast techniques
such as optimal algorithms, shared memory and perhaps even subgroups! As you 
prepare your solution, take note whether your optimizations are actually making
things faster, and by how much!
4) Optional: Compare the accuracy of the CPU (float) reduction to your GPU (float)
reduction. Which one comes closer to the CPU (double) reference? Why?
*/