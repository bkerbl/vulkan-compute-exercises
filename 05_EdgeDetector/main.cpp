#include <vulkan/vulkan.hpp>
#include <framework.h>
#include <iostream>
#include <filesystem>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

int main(int argc, char* argv[])
{
	try
	{
		if (argc < 2)
		{
			std::cout << "No image file name provided!" << std::endl;
			return -1;
		}

		FILE* file = std::fopen(argv[1], "rb"); // A FILE* in C++...
		if (file == nullptr)
		{
			std::cout << "Could not open given image!" << std::endl;
			return -2;
		}

		uint32_t width, height, numChannels; // Reading an input image from file
		stbi_uc* image_data = stbi_load_from_file(file, (int*)&width, (int*)&height, (int*)&numChannels, 4);
		std::vector<char> image(image_data, image_data + width * height * 4);
		stbi_image_free(image_data);
		std::fclose(file); // Let's pretend this never happened

		vk::UniqueInstance instance;
		vk::PhysicalDevice physicalDevice;
		vk::UniqueDevice device;
		vk::UniqueCommandPool commandPool;
		vk::Queue queue;

		Framework::setupBasicCompute("Task 5", VK_API_VERSION_1_3, {}, {}, instance, physicalDevice, device, queue, commandPool);

		VmaAllocator allocator = Framework::createAllocator(*instance, VK_API_VERSION_1_3, physicalDevice, *device);
		// Description for the resources that go into our pipeline (and descriptor set):
		// - One uniform buffer for image parameters (width, height)
		// - One storage buffer for the source image (width * height * 4 channels, RGBA)
		// - One storage buffer for the result image (width * height * 4 channels, RGBA)

		VkBufferCreateInfo infoCreate = vk::BufferCreateInfo({}, 2 * sizeof(uint32_t), vk::BufferUsageFlagBits::eUniformBuffer),
			srcCreate = vk::BufferCreateInfo({}, width * height * sizeof(uint32_t), vk::BufferUsageFlagBits::eStorageBuffer),
			dstCreate = vk::BufferCreateInfo({}, width * height * sizeof(uint32_t), vk::BufferUsageFlagBits::eStorageBuffer);
		VmaAllocationCreateInfo allocationInfo = { 0, VMA_MEMORY_USAGE_UNKNOWN, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT };

		VkBuffer infoBuffer, srcBuffer, dstBuffer;
		VmaAllocation infoAllocation, srcAllocation, dstAllocation;

		vmaCreateBuffer(allocator, &infoCreate, &allocationInfo, &infoBuffer, &infoAllocation, nullptr);
		vmaCreateBuffer(allocator, &srcCreate, &allocationInfo, &srcBuffer, &srcAllocation, nullptr);
		vmaCreateBuffer(allocator, &dstCreate, &allocationInfo, &dstBuffer, &dstAllocation, nullptr);

		// TODO: Prepare a descriptor pool that can provide a descriptor set,  
		// two storage buffer descriptors and one uniform buffer descriptor. 
		// 
		// TODO: Define bindings for the buffers in your descriptor set layout.
		// Make sure they match the bindings in the shader. Create a descriptor
		// set layout from them.
		vk::UniqueDescriptorSetLayout descriptorSetLayout;

		// TODO: Allocate a descriptor set with the created layout. 
		vk::UniqueDescriptorSet descriptorSet;

		// TODO: Update the descriptor set to reference your actual buffers in 
		// the corresponding bindings (use "updateDescriptorSets" with correct
		// WriteDescriptorSet structs filled in. 
	
		// TODO: Fill the created buffers with information. 
		// 
		// TODO: The info buffer is for meta information that we need, the width and the 
		// height of the image. Map its memory and then write the two integers in this order, 
		// 1) width 2) height.
		//
		// TODO: The src buffer is for storing the image color data. Map its memory
		// and then copy the contents of the image vector there. Ideally use std::memcpy.
		// Hint: at the end of this program, we already do something similar, just in the 
		// opposite direction, i.e., copying from mapped memory into a vector. 

		vk::UniqueShaderModule shaderModule;
		vk::UniquePipelineLayout layout;
		vk::UniquePipelineCache cache;
		vk::UniquePipeline pipeline;

		Framework::setupComputePipeline("sobel.comp.spv", { *descriptorSetLayout }, *device, shaderModule, layout, cache, pipeline);

		vk::CommandBufferAllocateInfo allocateInfo(*commandPool, vk::CommandBufferLevel::ePrimary, 1);
		auto cmdBuffers = device->allocateCommandBuffersUnique(allocateInfo);

		cmdBuffers[0]->begin(vk::CommandBufferBeginInfo{});
		cmdBuffers[0]->bindPipeline(vk::PipelineBindPoint::eCompute, *pipeline);
		cmdBuffers[0]->bindDescriptorSets(vk::PipelineBindPoint::eCompute, *layout, 0, *descriptorSet, {});

		// TODO: Set the proper grid dimensions for your dispatch that need to run for your image.
		// The 2D group size should be 16 in X and 16 in Y dimension. That leaves you with the task
		// of computing how many groups are needed to cover the image's width and height. Note: C++
		// by default rounds integers DOWN! E.g., for a 17x17 image, 17/16 = 1. If our image is 
		// 17x17 pixels, but we start a 1x1 grid with 16x16 groups, would you expect correct results?

		cmdBuffers[0]->dispatch(1, 1, 1);

		// TODO: We need a barrier to make sure the written data can be safely read by the CPU. 
		// By now you know the drill! Note that you can probably use code from the previous task.

		cmdBuffers[0]->end();

		Framework::initDebugging();
		Framework::beginCapture();
		queue.submit(vk::SubmitInfo({}, {}, * cmdBuffers[0]));
		Framework::endCapture();

		device->waitIdle();

		// Write the contents of the dst buffer out into an image. It will be named "output.png"
		// You should find it in the location where the built files for THIS TASK are located.
		uint32_t* image_out;
		vmaMapMemory(allocator, dstAllocation, (void**)&image_out);
		std::memcpy(image.data(), image_out, image.size());
		vmaUnmapMemory(allocator, dstAllocation);

		stbi_write_png("output.png", width, height, STBI_rgb_alpha, image.data(), 0);

		std::cout << "Program finished, please check the image "
			<< (std::filesystem::current_path() / std::filesystem::path("output.png")).string() << std::endl;

		vmaDestroyBuffer(allocator, infoBuffer, infoAllocation);
		vmaDestroyBuffer(allocator, srcBuffer, srcAllocation);
		vmaDestroyBuffer(allocator, dstBuffer, dstAllocation);
		vmaDestroyAllocator(allocator);
	}
	catch (Framework::NotImplemented e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

/*
==================================== Task 5 ====================================
0) Pass the path of an image file in the assets directory as a program argument.
1) Make use of the VMA to create the buffers you will need.
2) Prepare descriptor set pools, layouts and descriptor sets.
3) Fill the created buffers with useful information in main.cpp.
4) Compute and set the proper dispatch dimensions in main.cpp.
5) Add a barrier to safely read GPU-written data from the CPU in main.cpp.
6) Complete the shader file sobel.comp to perform edge detection on input images.
*/