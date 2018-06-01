cmake_minimum_required(VERSION 2.8.3)
project(ros_opencl_tests)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
    ros_opencl
    sensor_msgs
)

include_directories(${catkin_INCLUDE_DIRS})

catkin_package()

add_executable(cpu_side src/cpu.cpp)
target_link_libraries(cpu_side ${catkin_LIBRARIES})

add_executable(gpu_side src/gpu.cpp)
target_link_libraries(gpu_side ${catkin_LIBRARIES})

# add_executable(compare src/compare.cpp)
# target_link_libraries(compare ${catkin_LIBRARIES})
