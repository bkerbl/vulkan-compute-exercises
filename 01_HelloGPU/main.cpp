#include <vulkan/vulkan.hpp>
#include <framework.h>
#include <iostream>

int main()
{
	try
	{
		// Variables that will hold the objects we need to run Vulkan applications
		vk::UniqueInstance instance;
		vk::UniqueDevice device;
		vk::UniqueCommandPool commandPool;
		vk::PhysicalDevice physicalDevice;
		vk::Queue queue;

		// We will need this functionality several times. Implement it once, reuse code in future!
		Framework::setupBasicCompute("Task 1", VK_API_VERSION_1_3, {}, {},
			instance, physicalDevice, device, queue, commandPool);

		// Variables for compute pipeline to run shader code and related objects 
		vk::UniqueShaderModule shaderModule;
		vk::UniquePipelineLayout layout;
		vk::UniquePipelineCache cache;
		vk::UniquePipeline pipeline;

		Framework::beginCapture();

		// We will need this functionality several times. Implement it once, reuse code in future!
		Framework::setupComputePipeline("hello.comp.spv", {}, *device, shaderModule, layout, cache, pipeline);

		vk::CommandBufferAllocateInfo allocateInfo{ *commandPool, vk::CommandBufferLevel::ePrimary, 1 };
		auto cmdBuffers = device->allocateCommandBuffersUnique(allocateInfo);

		cmdBuffers[0]->begin(vk::CommandBufferBeginInfo{});
		cmdBuffers[0]->bindPipeline(vk::PipelineBindPoint::eCompute, *pipeline);
		cmdBuffers[0]->dispatch(8, 1, 1);
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

	return 0;
}

/*
==================================== Task 1 ====================================
1) Implement the function Framework::setupBasicCompute in framework.cpp.
2) Implement the function Framework::setupCompuePipeline in framework.cpp.
3) Complete the shader file hello.comp to print a basic message.
4) Optional: Experiment with different dispatch/work group configurations. There
is a way to actually set the work group size from the C++ side! You can try to
do this, but you will probably need your own pipeline creation function for this.
*/