cmake_minimum_required(VERSION 3.10)
project(rbm)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Add OpenCV's header path
include_directories(${OpenCV_INCLUDE_DIRS})

# Add the libraries for OpenCV
set(requiredLibs ${requiredLibs} ${OpenCV_LIBS})

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

add_definitions(-DNDEBUG)
add_definitions(-DNDEBUG_TRACE)

add_executable(rbm src/rbm.cpp)

add_library(sfc_lib
        src/mnist_reader.cpp src/mnist_reader.h
        src/exceptions.h
        src/neuron.cpp src/neuron.h
        src/activation_funcs.cpp src/activation_funcs.h
        src/rbm_net.cpp src/rbm_net.h)

target_link_libraries(rbm sfc_lib ${requiredLibs})
