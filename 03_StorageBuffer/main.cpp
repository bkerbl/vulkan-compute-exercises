#include <vulkan/vulkan.hpp>
#include <framework.h>
#include <iostream>

int main()
{
	try
	{
		// Variables that will hold the objects we need to run Vulkan applications
		vk::UniqueInstance instance;
		vk::PhysicalDevice physicalDevice;
		vk::UniqueDevice device;
		vk::UniqueCommandPool commandPool;
		vk::Queue queue;

		Framework::setupBasicCompute("Task 3", VK_API_VERSION_1_3, {}, {}, instance, physicalDevice, device, queue, commandPool);

		// Variables for compute pipeline to run shader code and related objects 
		vk::UniqueShaderModule shaderModule;
		vk::UniquePipelineLayout layout;
		vk::UniquePipelineCache cache;
		vk::UniquePipeline pipeline;
		std::vector<vk::DescriptorSetLayout> descLayouts;

		// TODO: Create a unique descriptor set layout. It should have a single storage
		// buffer at some binding index. Push the descriptor set layout into descLayouts.

		Framework::setupComputePipeline("fibonacci.comp.spv", descLayouts, *device, shaderModule, layout, cache, pipeline);

		// TODO: We will want a resource to put in our descriptor set. A single storage buffer is needed.
		// The buffer should have enough room to store 64 integers. The memory we use for the buffer 
		// should device-local, but also be visible to the host and coherent so we can write to it 
		// from the CPU. Once the buffer is created, you will have to (in addition):
		// 1) Get the buffer's memory requirements (a struct)
		// 2) Find a memory type that fulfills the requirements (you can use the code below)
		// 3) Allocate a large enough chunk of it to store the data
		// 4) Bind the memory to the buffer
		vk::UniqueBuffer buffer;
		vk::MemoryRequirements req;
		vk::MemoryPropertyFlagBits props;

		uint32_t memoryIndex;
		vk::PhysicalDeviceMemoryProperties memProps = physicalDevice.getMemoryProperties();
		for (memoryIndex = 0; memoryIndex < memProps.memoryTypes.size(); memoryIndex++)
		{
			vk::MemoryPropertyFlags flags = memProps.memoryTypes[memoryIndex].propertyFlags;
			if (req.memoryTypeBits & (1 << memoryIndex) && (flags & props) == props)
				break;
		}
		if (memoryIndex == memProps.memoryTypes.size())
			throw std::runtime_error("No suitable memory found!");

		// TODO: We will want a pool that can provide 1 descriptor set and 1 storage buffer, nothing else.
		//
		// TODO: Allocate a single unique descriptor set from your descriptor pool, with the 
		// descriptor set layout you defined above. Connect your buffer to your descriptor set
		// by preparing the necessary resource info, write struct and executing the update,
		// exactly as in Task 3, just that this time it is a storage buffer resource you write.

		vk::CommandBufferAllocateInfo allocateInfo{ *commandPool, vk::CommandBufferLevel::ePrimary, 1 };
		auto cmdBuffers = device->allocateCommandBuffersUnique(allocateInfo);

		cmdBuffers[0]->begin(vk::CommandBufferBeginInfo{});
		cmdBuffers[0]->bindPipeline(vk::PipelineBindPoint::eCompute, *pipeline);

		// TODO: Bind your descriptor set before dispatching a compute job that needs it.

		cmdBuffers[0]->dispatch(1, 1, 1);

		// TODO: Some data was written by the GPU to the buffer. Make sure that it becomes 
		// available to the CPU safely. Enforce a memory barrier that makes shader writes (source) 
		// available to host reads (destination). The corresponding pipeline barrier should 
		// synchronize the computer shader stage with the host stage.

		cmdBuffers[0]->end();

		Framework::initDebugging();
		Framework::beginCapture();
		queue.submit(vk::SubmitInfo({}, {}, *cmdBuffers[0]));
		Framework::endCapture();

		device->waitIdle();

		int* fibonacci;
		// TODO: Map the memory of your storage buffer with no offset and for its 
		// entire size (VK_WHOLE_SIZE works) and cast it to the integer pointer;

		std::cout << "The first 32 Fibonacci numbers are: " << std::endl;
		for (int i = 0; i < 32; i++)
			std::cout << i << ": " << fibonacci[i] << std::endl;
	}
	catch (Framework::NotImplemented e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

/*
==================================== Task 3 ====================================
1) Create a descriptor set layout with a single uniform buffer in main.cpp.
2) Create a storage buffer and memory to hold the GPU-produced data.
3) Create and fill a descriptor set for your pipeline in main.cpp.
4) Bind your descriptor set before dispatching the compute job in main.cpp.
5) Enforce an adequate barrier to protect reading back the storage in main.cpp
6) Complete the shader fibonacci.comp to compute Fibonacic numbers in parallel!
*/