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

		Framework::setupBasicCompute("Task 2", VK_API_VERSION_1_3, {}, {}, instance, physicalDevice, device, queue, commandPool);

		// Variables for compute pipeline to run shader code and related objects 
		vk::UniqueShaderModule shaderModule;
		vk::UniquePipelineLayout layout;
		vk::UniquePipelineCache cache;
		vk::UniquePipeline pipeline;
		std::vector<vk::DescriptorSetLayout> descLayouts;

		// TODO: Create a buffer binding for the buffer you want to read from in the shader. 
		// You can use any binding index you want, just make sure that it matches your 
		// definitions in the shader (you can just use 0 in both places). The buffer should 
		// be a uniform buffer. We just need one, not an array of buffers. Also, we want to 
		// use it in the compute stage.
		//
		// TODO: Create a unique descriptor set layout to organize your bindings (you only have one).
		// Add it to descLayouts (use * on unique descriptor set to get a regular descriptor set).
		//
		// The layouts are sent to the pipeline so it knows how the descriptor sets bound to it look.

		Framework::setupComputePipeline("print.comp.spv", descLayouts, *device, shaderModule, layout, cache, pipeline);

		// TODO: We will want a resource to put in our descriptor set. A single uniform buffer is needed.
		// The buffer should have enough room to store 3 integers. The memory we use for the buffer 
		// should be device-local, but also be visible to the host and host-coherent so we can write to it 
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

		// TODO: Fill your memory with some numbers: 7458410, 3542145 and 1647875.
		// Map it with no offset and for its entire size (VK_WHOLE_SIZE works) and cast it 
		// to an integer pointer (necessary, because you cannot write to a void*).
		//
		// TODO: We will want a descriptor pool that can provide 1 descriptor set 
		// and has room for 1 uniform buffer descriptor, nothing else.
		//
		// TODO: Allocate a single unique descriptor set from your descriptor pool, with the 
		// descriptor set layout you defined above.
		//
		// TODO: Enter the buffer into your descriptor set. First, prepare a descriptor 
		// buffer info that describes which buffer will be used, and how. Select your 
		// buffer from above, with no offset, and using its full size (VK_WHOLE_SIZE).
		//
		// TODO: Prepare a WriteDescriptorSet struct that specifies what entries
		// should be overwritten in which descriptor set. You want to update your 
		// descriptor set from above. You want to update the binding index you chose
		// above. We don't have an array of buffers, so just set array element to 0. 
		// The descriptor type is a uniform buffer. We are not updating image infos,
		// so leave those empty. We are updating descriptor buffer infos, so use 
		// the one you just prepared.
		//
		// TODO: Execute the updates by calling updateDescriptorSets on the device.
		// We are only doing writes, so no (0) copies should be passed.

		vk::CommandBufferAllocateInfo allocateInfo{ *commandPool, vk::CommandBufferLevel::ePrimary, 1 };
		auto cmdBuffers = device->allocateCommandBuffersUnique(allocateInfo);

		cmdBuffers[0]->begin(vk::CommandBufferBeginInfo{});
		cmdBuffers[0]->bindPipeline(vk::PipelineBindPoint::eCompute, *pipeline);

		// TODO: Bind your descriptor set before dispatching a compute job that needs it.
		// It should be bound to the "compute" pipeline bind point. Use your pipeline's layout.
		// You can define an offset for the set IDs, but best leave it at 0. Bind the first
		// (and only) entry from your vector of descriptor sets. We don't use dynamic offsets.

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
==================================== Task 2 ====================================
1) Create a descriptor set layout with a single uniform buffer in main.cpp.
2) Fill the uniform buffer's memory with the magic numbers in main.cpp
3) Create a descriptor pool that is big enough for our use.
4) Create and fill a descriptor set for your pipeline in main.cpp.
5) Bind your descriptor set before dispatching the compute job in main.cpp.
6) Complete the shader file print.comp to print the completed message!
7) Optional: Try the debugging functionality of RenderDoc:
- Enable DEBUG_SHADERS in CMake
- Start RenderDoc
- Launch your Vulkan executable from within RenerDoc (make sure the working
 directory is pointing to where your compiled shaders are)
- Explore and see how you can step through compute shader code
*/