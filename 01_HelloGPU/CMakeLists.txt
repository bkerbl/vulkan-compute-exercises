add_executable(01_HelloGPU main.cpp)

addGLSL(01_HelloGPU shaders/hello.comp)

target_include_directories(01_HelloGPU PRIVATE Vulkan::Headers)
target_link_libraries(01_HelloGPU Vulkan::Vulkan framework)