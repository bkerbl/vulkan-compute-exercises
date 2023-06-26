cmake_minimum_required(VERSION 3.8)
project(GPUSim-Vulkan)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)

IF (DEBUG_SHADERS)
	find_path(RENDERDOC_DIR "renderdoc_app.h" REQUIRED)
	find_path(RENDERDOC_HEADERS "renderdoc_app.h" REQUIRED PATHS ${RENDERDOC_DIR} NO_CACHE)
ENDIF()

add_library(framework framework.h framework.cpp)
target_include_directories(framework PRIVATE ${RENDERDOC_HEADERS} Vulkan::Headers PUBLIC ../framework )
target_link_libraries(framework Vulkan::Vulkan)