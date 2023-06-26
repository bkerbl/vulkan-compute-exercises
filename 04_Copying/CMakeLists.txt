add_executable(04_Copying main.cpp)
target_include_directories(04_Copying PRIVATE Vulkan::Headers)
target_link_libraries(04_Copying Vulkan::Vulkan framework)