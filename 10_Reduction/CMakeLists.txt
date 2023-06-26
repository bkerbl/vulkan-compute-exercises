add_executable(10_Reduction main.cpp)

addGLSL(10_Reduction shaders/reduce.comp)

target_include_directories(10_Reduction PRIVATE Vulkan::Headers)
target_link_libraries(10_Reduction Vulkan::Vulkan framework)