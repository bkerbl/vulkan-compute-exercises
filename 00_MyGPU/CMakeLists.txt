add_executable(00_MyGPU main.cpp)
target_include_directories(00_MyGPU PRIVATE Vulkan::Headers)
target_link_libraries(00_MyGPU Vulkan::Vulkan)