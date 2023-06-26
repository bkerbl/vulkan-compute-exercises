#pragma once

#ifndef GPUSIM_FRAMEWORK_H_INCLUDED
#define GPUSIM_FRAMEWORK_H_INLCUDED

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <iostream>
#include <fstream>
#include <vma/vk_mem_alloc.h>

namespace Framework
{
	class NotImplemented : public std::logic_error
	{
	private:
		std::string _text;
	public:
		NotImplemented(const char* function) : std::logic_error("Not implemented")
		{
			_text = std::string("Not implememented: ") + function;
		};

		virtual const char* what() const throw()
		{ return _text.c_str();	}
	};

	void initDebugging();
	void beginCapture();
	void endCapture();

	/// <summary>
	/// Loads the contents of a compiled SPIR-V binary file.
	/// </summary>
	/// <param name="path">(in): The path to the compiled SPIR-V binary file</param>
	/// <returns>A vector containing the loaded data as byte code</returns>
	std::vector<uint32_t> loadShader(const char* path);

	/// <summary>
	/// Creates a basic compute setup, including one instance, one logical device, one queue and a command buffer pool.
	/// </summary>
	/// <param name="app_name">(in): The name that shows up for your running application</param>
	/// <param name="api_version">(in): The Vulkan API version that should be used</param>
	/// <param name="extensions_instance">(in): A list of extensions to load for the Vulkan instance</param>
	/// <param name="extensions_device">(in): A list of extensions to load for the logical device</param>
	/// <param name="instance">(out): Parameter for storing the created instance in</param>
	/// <param name="physicalDevice">(out): Parameter for storing the selected physical device in</param>
	/// <param name="device">(out): Parameter for storing the created logical device in</param>
	/// <param name="queue">(out): Parameter for storing the queue in</param>
	/// <param name="commandPool">(out): Parameter for storing command buffer pool</param>
	void setupBasicCompute(const char* app_name, uint32_t api_version,
		const std::vector<const char*>& extensions_instance, const std::vector<const char*>& extensions_device,
		vk::UniqueInstance& instance, vk::PhysicalDevice& physicalDevice, vk::UniqueDevice& device, vk::Queue& queue, vk::UniqueCommandPool& pool);

	/// <summary>
	/// Function to create a Vulkan Memory Allocator. Helps with all kinds of
	/// memory-related tasks, and is capable of managing it for you.
	/// </summary>
	/// <param name="instance">The instance that the allocator should use</param>
	/// <param name="apiVersion">The API version whose features the allocator may use</param>
	/// <param name="physicalDevice">The physical device the allocator should be associated with</param>
	/// <param name="device">The logical device the allocator should be associated with</param>
	/// <returns>The newly created allocator</returns>
	VmaAllocator createAllocator(vk::Instance instance, uint32_t apiVersion, vk::PhysicalDevice physicalDevice, vk::Device device);

	/// <summary>
	/// Prepares a compute shader pipeline with a shader module that runs the code in the source file.
	/// </summary>
	/// <param name="sourceFile">(in): The compiled SPIR-V file to turn into a shader module</param>
	/// <param name="descriptorSetLayouts">(in): Layouts for pipeline descriptor sets</param>
	/// <param name="device">(in): The device for which to create the shader module</param>
	/// <param name="shaderModule">(out): Parameter for the completed shader module</param>
	/// <param name="pipelineLayout">(out): Parameter for the compute pipeline layout</param>
	/// <param name="pipelineCache">(out): Parameter for the compute pipeline cache</param>
	/// <param name="pipeline">(out): Parameter for the compute pipeline itself</param>
	void setupComputePipeline(const char* sourceFile, const std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts, vk::Device device, vk::UniqueShaderModule& shaderModule, vk::UniquePipelineLayout& pipelineLayout, vk::UniquePipelineCache& pipelineCache, vk::UniquePipeline& pipeline);
	
	/// <summary>
	/// Loads the contents of an .obj file as a point cloud.
	/// </summary>
	/// <typeparam name="P">(template): The point class to build. Must have members 
	/// glm::vec3 "position" and "color"</typeparam>
	/// <param name="path">(in): The path to the file to be loaded</param>
	/// <param name="pointcloud">(out): The vector in which the loaded points are stored</param>
	/// <param name="minimum">(out): Model minimum bounds</param>
	/// <param name="maximum">(out): Model maximum bounds</param>
	template<typename P>
	void readPointCloud(const char* path, std::vector<P>& pointcloud, glm::vec3& minimum, glm::vec3& maximum)
	{
		std::ifstream infile(path);
		if (!infile.good())
			throw std::runtime_error("Unable to open point cloud file \"" + std::string(path) + "\"");

		std::cout << "Framework: Reading point cloud..." << std::endl;
		std::string str((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
		std::stringstream file(str);
		
		minimum = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
		maximum = glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		std::string line;
		std::stringstream ss;
		char lineStart;
		P point;
		while (std::getline(file, line))
		{
			ss << line;
			if (ss >> lineStart && lineStart == 'v')
			{
				if (!(ss >> point.position.x >> point.position.y >> point.position.z))
					continue;
				if (!(ss >> point.color.x >> point.color.y >> point.color.z))
					point.color = glm::vec3(0.5f, 0.5f, 0.5f);
				pointcloud.push_back(point);
				minimum = glm::min(point.position, minimum);
				maximum = glm::max(point.position, maximum);
			}
		}
		std::cout << "Framework: Done reading!" << std::endl;
	}
};

#endif GPUSIM_FRAMEWORK_H_INCLUDED