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
	int N = 200'000'000;
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

		Framework::setupBasicCompute("Task 11", VK_API_VERSION_1_3, {}, { VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME }, instance, physicalDevice, device, queue, commandPool);

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
		//
		// TODO: There is a twist this time - the data is probalby too large for your 
		// your GPU to fit it into host-visible, host-coherent AND device-local memory.
		// You should use a staging buffer! Remember that copying requires synchronization.

		vk::UniqueDescriptorSetLayout descriptorSetLayout;
		vk::UniqueDescriptorPool descriptorPool;
		vk::UniqueDescriptorSet descriptorSet;

		Framework::setupComputePipeline("reduce_sequential.comp.spv", { *descriptorSetLayout }, *device, shaderModule, layout, cache, pipeline);

		float input_sum = 0.0f;
		for (int i = 0; i < N; i++)
			input_sum += input[i];
		std::cout << "CPU reduction (float): " << input_sum << std::endl;

		// TODO: Set up your submission and submit your command buffer! The result
		// will be printed by the shader this time, it suffices to launch a single
		// work group.

		vk::CommandBufferAllocateInfo allocateInfo(*commandPool, vk::CommandBufferLevel::ePrimary, 1);
		auto cmdBuffers = device->allocateCommandBuffersUnique(allocateInfo);

		Framework::initDebugging();
		Framework::beginCapture();
		queue.submit(vk::SubmitInfo({}, {}, *cmdBuffers[0]));
		Framework::endCapture();

		device->waitIdle();
	}
	catch (Framework::NotImplemented e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

/*
==================================== Task 11 ====================================
0) You probably don't want to run this task in Debug mode (slow)!
1) Prepare the environment and necessary GPU resources for reduction (addition).
2) Make sure to use a staging buffer so that the large data can move to the GPU.
*/