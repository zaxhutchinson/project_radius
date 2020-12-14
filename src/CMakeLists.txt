cmake_minimum_required(VERSION 3.10)

#set the project name
project(ProjectRadius VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#add the executable
add_executable(${PROJECT_NAME})
target_sources(ProjectRadius 
    PUBLIC
        neuron.cpp
        neuron.hpp
        pr.cpp
        synapse.hpp
        synapse.cpp
        tester.hpp
        unit_tests.hpp
        vec3.cpp
        vec3.hpp
        zxlb_templates.hpp
        zxlb.hpp
        
)

configure_file(ProjectRadius.hpp.in ProjectRadius.hpp)

target_include_directories(ProjectRadius PUBLIC
    "${PROJECT_BINARY_DIR}"
)