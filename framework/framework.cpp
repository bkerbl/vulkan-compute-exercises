#include "framework.h"
#include <iostream>
#include <fstream>
#define VMA_IMPLEMENTATION
#define VMA_VULKAN_VERSION 1003000 // Vulkan 1.3
#include <vma/vk_mem_alloc.h>

#ifdef DEBUG_SHADERS
#include <renderdoc_app.h>
#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#endif

namespace Framework
{
#ifdef DEBUG_SHADERS
	RENDERDOC_API_1_5_0* rdoc_api = NULL;

	void initDebugging()
	{
#ifdef WIN32
		if (HMODULE mod = GetModuleHandleA("renderdoc.dll"))
		{
			pRENDERDOC_GetAPI RENDERDOC_GetAPI =
				(pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
			int ret = RENDERDOC_GetAPI(RENDERDOC_Version::eRENDERDOC_API_Version_1_5_0, (void**)&rdoc_api);
			assert(ret == 1);
		}
#else
		if (void* mod = dlopen("librenderdoc.so", RTLD_NOW | RTLD_NOLOAD))
		{
			pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)dlsym(mod, "RENDERDOC_GetAPI");
			int ret = RENDERDOC_GetAPI(RENDERDOC_API_1_5_0, (void**)&rdoc_api);
			assert(ret == 1);
		}
#endif
	}
	void beginCapture()
	{
		if (rdoc_api) rdoc_api->StartFrameCapture(NULL, NULL);
	}

	void endCapture()
	{	if (rdoc_api) rdoc_api->EndFrameCapture(NULL, NULL);	}
#else
	void initDebugging()
	{
	}

	void beginCapture()
	{
	}

	void endCapture()
	{
	}
#endif

	std::vector<uint32_t> loadShader(const char* path)
	{
		std::ifstream infile(path, std::ios::binary | std::ios::ate);
		if (!infile.good())
			throw std::runtime_error("Unable to open shader file \"" + std::string(path) + "\"");
		std::streamsize size = infile.tellg();
		std::vector<uint32_t> buffer((size + 3) / 4);
		infile.seekg(std::ios::beg);
		infile.read((char*)buffer.data(), size);
		return buffer;
	}

	void setupBasicCompute(const char* app_name, uint32_t api_version,
		const std::vector<const char*>& extensions_instance,
		const std::vector<const char*>& extensions_device, vk::UniqueInstance& instance, vk::PhysicalDevice& physicalDevice, vk::UniqueDevice& device, vk::Queue& queue, vk::UniqueCommandPool& pool)
	{
		// TODO: remove this
		throw NotImplemented(__FUNCTION__);

		// TODO: Create a unique instance. For that, prepare an app info with the given name and API version.
		// It should enable all extensions provided in extensions_instance. Store it in the provided output 
		// parameter.
		// 
		// TODO: Select a physical device. It should be a device that supports AT LEAST the given API version.
		// Furthermore, it should have at least one queue family that supports both COMPUTE and TRANSFER.
		// For each physical device, go through its collection of queue family properties. If the properties
		// of a particular queue family have the queue flags for COMPUTE and TRANSFER set, keep the index of
		// this queue family for later and store that physical device. The check can be done like this:
		// 
		// vk::QueueFamilyProperties props = physicalDeviceQueueFamilyProps[i];
		// if ((props.queueFlags & vk::QueueFlagBits::eCompute) && (props.queueFlags & vk::QueueFlagBits::eTransfer))
		// { 
		//	   <remember the index i, it is your chosen queue family's identifier>
		//	   <remember the physical device, it is the one that provides the queue family we want>
		// }
		// 
		// If no suitable physical device / queue is found, emit an error message. If you hit this error and
		// you are sure you did everything correct, notify the lecturer and ask for help. Store the physical 
		// device you selected in the output parameter physicalDevcie.
		//
		// TODO: Create a unique (logical) device from the physical device. The device should be created with 
		// a single queue from the family you identified above. The queue priority should be 1.0. The device 
		// should enable all extensions listed in extensions_device. Store the device in the provided output 
		// parameter. Get its first queue and store it in the provided output parameter.
		// 
		// TODO: Create a unique command pool. Use the queue family index you found above. Make sure that the
		// buffers allocated from this command pool can be reset (i.e., recorded multiple times). This is 
		// achieved by adding the vk::CommandPoolCreateFlagBits::eResetCommandBuffer flag to the create
		// info. Store the created pool in the output parameter. 
	}

	VmaAllocator createAllocator(vk::Instance instance, uint32_t apiVersion, vk::PhysicalDevice physicalDevice, vk::Device device)
	{
		// TODO: remove this
		throw NotImplemented(__FUNCTION__);

		// TODO: Create a VMA allocator. Follow the standard procedure given in
		// the official examples. Return the created allocator.
	}

	void setupComputePipeline(const char* sourceFile, const std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts,
		vk::Device device, vk::UniqueShaderModule& shaderModule, vk::UniquePipelineLayout& pipelineLayout,
		vk::UniquePipelineCache& pipelineCache, vk::UniquePipeline& pipeline)
	{
		// TODO: remove this
		throw NotImplemented(__FUNCTION__);

		// TODO: Create a unique shader module. For this, use the loadShader() function to load the
		// compiled SPIR-V code from the source file. Store the shader module in the provided output parameter.
		//
		// TODO: Create a unique pipeline layout, using the incoming descriptorSetLayouts to configure it
		// for accessing descriptors (=shader input resources). Store it in the provided output parameter.
		// 
		// TODO: Create a unique pipeline cache. Store it in the provided output parameter.
		//
		// TODO: Create a unique pipeline. First, prepare a shader stage info. It should be a compute stage
		// that uses the shader module you made, using the function named "main" as an entry point. Prepare
		// a pipeline create info that uses the stage info and the above pipeline layout. Finally, create
		// a pipeline with the above pipeline cache and this create info. The returned object is a pair
		// (.result, .value). Check that the .result is vk::Result::eSuccess. If it is not, emit an error.
		// If it is, store the generated .value in the pipeline output parameter. Because .value is a
		// unique pointer, you will need to std::move( ) it into the output parameter.
	}
}