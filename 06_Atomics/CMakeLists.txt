add_executable(06_Atomics main.cpp)

addGLSL(06_Atomics shaders/primes.comp)

target_include_directories(06_Atomics PRIVATE Vulkan::Headers)
target_link_libraries(06_Atomics Vulkan::Vulkan framework)