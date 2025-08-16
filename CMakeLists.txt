cmake_minimum_required(VERSION 3.10.0)

project(NODE_SAMPLE VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(fastcdr CONFIG REQUIRED)
option(NDDS_USE_SANITIZER "Use sanitizers" OFF)

file(GLOB_RECURSE core_src core/*.cpp core/*.hpp rtps/*.hpp rtps/*.cpp utils/*.hpp utils/*.cpp)

add_library(dds_core SHARED ${core_src})

target_compile_definitions(dds_core PUBLIC 
    NDDS_CORE_EXPORTS
    )

if (NDDS_USE_SANITIZER)
    target_compile_options(dds_core PRIVATE -fsanitize=address,undefined)
    target_link_options(dds_core PRIVATE -fsanitize=address,undefined)
endif()

target_link_libraries(dds_core PUBLIC 
    Boost::boost 
    nlohmann_json::nlohmann_json
    fastcdr
    )

add_executable(NODE_SAMPLE main.cpp 
    )

# set_property(TARGET NODE_SAMPLE PROPERTY COMPILE_WARNING_AS_ERROR ON)
if (NDDS_USE_SANITIZER)
    target_compile_options(NODE_SAMPLE PRIVATE -fsanitize=address,undefined)
    target_link_options(NODE_SAMPLE PRIVATE -fsanitize=address,undefined)
endif()

target_link_libraries(NODE_SAMPLE PRIVATE 
    dds_core
)

include(CTest)
enable_testing()

