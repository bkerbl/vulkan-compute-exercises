#include <vulkan/vulkan.hpp>
#include <iostream>

int main()
{
	return 0;
}

/*
==================================== Task 0 ====================================
1) Create a Vulkan instance. Give it an application name "Task 0", a version 
number 1, and an engine name of "None". The API version should be 1.3.
2) Query your physical devices. Retrieve their properties and print out 
- the total number of devices
- their names
- the latest API they support (human-readable! Might be a bit tricky!)
3) Physical devices have several types of property collections available. Out of 
all of them, find three properties that should be relevant to compute jobs and
print them out. 
4) Make sure to clean up everything! Either by explicitly destroying your objects 
(in the right order) or by using the smart pointers of vulkan.hpp.
5) Optional: you can go and explore a bit, checking out the individual features
of your devices and queue families it provides. What kinds does it support? 
Check out an advanced feature using the pNext pointer of VkPhysicalDeviceFeatures2.
*/