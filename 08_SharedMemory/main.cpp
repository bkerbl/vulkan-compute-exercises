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

		Framework::setupBasicCompute("Task 8", VK_API_VERSION_1_3, {}, {}, instance, physicalDevice, device, queue, commandPool);

		vk::UniqueShaderModule shaderModule;
		vk::UniquePipelineLayout layout;
		vk::UniquePipelineCache cache;
		vk::UniquePipeline pipeline;

		Framework::setupComputePipeline("communicate.comp.spv", {}, *device, shaderModule, layout, cache, pipeline);

		vk::CommandBufferAllocateInfo allocateInfo(*commandPool, vk::CommandBufferLevel::ePrimary, 1);
		auto cmdBuffers = device->allocateCommandBuffersUnique(allocateInfo);

		cmdBuffers[0]->begin(vk::CommandBufferBeginInfo{});
		cmdBuffers[0]->bindPipeline(vk::PipelineBindPoint::eCompute, *pipeline);
		cmdBuffers[0]->dispatch(1, 1, 1);
		cmdBuffers[0]->end();

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
	std::cin.ignore();

	return 0;
}

/*
==================================== Task 8 ====================================
1) Complete the shader file communicate.comp to use shared memory.
2) Optional: Experiment! What happens if you skip synchronization barriers? Why?
Can you find a reproducible setup where you get wrong results if you skip them?
*/