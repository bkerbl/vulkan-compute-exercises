#include <vulkan/vulkan.hpp>
#include <framework.h>
#include <iostream>
#include <vector>

int main(int argc, char* argv[])
{
	try
	{
		vk::UniqueInstance instance;
		vk::PhysicalDevice physicalDevice;
		vk::UniqueDevice device;
		vk::UniqueCommandPool commandPool;
		vk::Queue queue;

		Framework::setupBasicCompute("Task 7", VK_API_VERSION_1_3, {}, {}, instance, physicalDevice, device, queue, commandPool);

		const uint32_t numWorkGroups = 25;   // How many work groups we will start
		const uint32_t sizeWorkGroup = 128;  // How many threads we expect in each work group
		const uint32_t numsTested = numWorkGroups * sizeWorkGroup; // Total number of threads

		// TODO: The description for the resources that go into our pipeline (and descriptor set):
		// - One storage buffer for the counter (size of a single int)
		// - One storage buffer for the output array with primes (size of numTested * size of (int)).
		// - Both of them should be host-visible, host-coherent AND device-local.

		vk::UniqueDescriptorSetLayout descriptorSetLayout;
		vk::UniqueDescriptorPool descriptorPool;
		vk::UniqueDescriptorSet descriptorSet;
		std::vector<vk::UniqueBuffer> buffers;
		std::vector<vk::UniqueDeviceMemory> bufferMemories;

		// TODO: As before, take all the necessary steps to have a descriptor set that references
		// the above created buffers. Make sure the bindings match the shader "primes.comp".

		uint32_t* outputMapped = nullptr;

		// TODO: Map and initialize buffer memories. Set the counter, a single integer, to 0. Next,
		// map the output array to the above variable "outputMapped". Initialize the entries in the 
		// array all to UINT_MAX, or 0xFFFFFFFF (equivalent). Hint: for this, you can actually just 
		// use std::memset to do it in one line. 

		vk::UniqueShaderModule shaderModule;
		vk::UniquePipelineLayout layout;
		vk::UniquePipelineCache cache;
		vk::UniquePipeline pipeline;

		Framework::setupComputePipeline("primes.comp.spv", { *descriptorSetLayout }, *device, shaderModule, layout, cache, pipeline);

		vk::CommandBufferAllocateInfo allocateInfo(*commandPool, vk::CommandBufferLevel::ePrimary, 1);
		auto cmdBuffers = device->allocateCommandBuffersUnique(allocateInfo);

		vk::MemoryBarrier memoryBarrier(vk::AccessFlagBits::eShaderWrite, vk::AccessFlagBits::eHostRead);
		cmdBuffers[0]->begin(vk::CommandBufferBeginInfo{});
		cmdBuffers[0]->bindPipeline(vk::PipelineBindPoint::eCompute, *pipeline);
		cmdBuffers[0]->bindDescriptorSets(vk::PipelineBindPoint::eCompute, *layout, 0, *descriptorSet, {});
		cmdBuffers[0]->dispatch(numWorkGroups, 1, 1);
		cmdBuffers[0]->pipelineBarrier(vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eHost, {}, memoryBarrier, {}, {});
		cmdBuffers[0]->end();

		Framework::initDebugging();
		Framework::beginCapture();
		queue.submit(vk::SubmitInfo({}, {}, *cmdBuffers[0]));
		Framework::endCapture();

		device->waitIdle();

		std::cout << "All prime numbers from 0 to " << numsTested << ": ";
		for (int i = 0; i < numsTested; i++)
			if (outputMapped && outputMapped[i] != 0xffffffff)
				std::cout << (i == 0 ? "" : ", ") << outputMapped[i];
	}
	catch (Framework::NotImplemented e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

/*
==================================== Task 6 ====================================
1) Create all the resources you need to fill a descriptor set for the shader.
2) Map and initialize the buffer memories in main.cpp.
3) Complete the shader file primes.comp to compute and store prime numbers.
4) Optional: Why are the numbers not in order? Sort them on the CPU and print.
*/