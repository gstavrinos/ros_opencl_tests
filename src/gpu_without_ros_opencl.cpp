#include <string>
#include <fstream>
#include <ros/ros.h>
#include <ros/package.h>
#include <sensor_msgs/Image.h>

#ifdef __APPLE__
    #include "OpenCL/opencl.h"
#else
    #include "CL/cl.h"
#endif

ros::Publisher pub;
int blur_radius;
cl_kernel kernel;
cl_context context;
cl_program program;
std::vector<cl_device_id> deviceIds;

void checkError (const cl_int error){
    if (error != CL_SUCCESS) {
        ROS_ERROR("OpenCL call failed with error: %d: ", error);
        exit (1);
    }
}

std::string LoadKernel (const char* name){
    std::ifstream in (name);
    std::string result((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    return result;
}

cl_program createProgram (const std::string& source, const cl_context context){
    size_t lengths [1] = { source.size () };
    const char* sources [1] = { source.data () };

    cl_int error = 0;
    cl_program program_ = clCreateProgramWithSource(context, 1, sources, lengths, &error);
    checkError (error);

    return program_;
}

void callback (const sensor_msgs::Image& msg){
    sensor_msgs::Image result_ = sensor_msgs::Image(msg);
    std::vector<int> v(msg.data.begin(), msg.data.end());
    // Adding a copy of the vector to have the original image too
    v.insert(v.end(), msg.data.begin(), msg.data.end());
    std::vector<int> v2;
    v2.push_back(msg.step);
    v2.push_back(msg.data.size());
    v2.push_back(blur_radius);
    size_t sz = v.size();
    size_t sz2 = v2.size();
    size_t typesz = sizeof(int) * sz;
    size_t typesz2 = sizeof(int) * sz2;
    cl_int error = 0;
    cl_mem buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, typesz, NULL, &error);
    checkError(error);
    cl_mem buffer2 = clCreateBuffer(context, CL_MEM_WRITE_ONLY, typesz2, NULL, &error);
    checkError(error);
    clSetKernelArg (kernel, 0, sizeof (cl_mem), &buffer);
    clSetKernelArg (kernel, 1, sizeof (cl_mem), &buffer2);
    cl_command_queue queue = clCreateCommandQueueWithProperties (context, deviceIds [0], NULL, &error);
    clEnqueueWriteBuffer(queue, buffer, CL_TRUE, 0, typesz, &v[0], 0, NULL, NULL);
    checkError (error);
    clEnqueueWriteBuffer(queue, buffer2, CL_TRUE, 0, typesz2, &v2[0], 0, NULL, NULL);
    checkError (error);
    size_t size = msg.data.size() / 3;
    cl_event gpuExec;
    checkError (clEnqueueNDRangeKernel (queue, kernel, 1, NULL, &size, NULL, 0, NULL, &gpuExec));
    clWaitForEvents(1, &gpuExec);
    int *result = (int *) malloc(typesz);
    checkError(clEnqueueReadBuffer(queue, buffer, CL_TRUE, 0, typesz, result, 0, NULL, NULL));
    result_.data.assign(result, result+sz/2);
    clReleaseCommandQueue (queue);
    clReleaseMemObject(buffer);
    clReleaseMemObject(buffer2);
    clReleaseEvent(gpuExec);
    free(result);
    pub.publish(result_);
}

int main (int argc, char** argv){
    ros::init (argc, argv, "ros_opencl_tests_gpu_without_ros_opencl");
    ros::NodeHandle nh;
    std::string in_topic;
    std::string result_topic;
    nh.param("ros_opencl_tests_gpu_without_ros_opencl/in_topic", in_topic, std::string("/usb_cam/image_raw"));
    nh.param("ros_opencl_tests_gpu_without_ros_opencl/result_topic", result_topic, std::string("ros_opencl_tests_gpu_without_ros_opencl/result"));
    nh.param("ros_opencl_tests_gpu_without_ros_opencl/blur_radius", blur_radius, 5);
    ROS_ERROR("%d", blur_radius);
    std::string full_kernel_path = ros::package::getPath("ros_opencl_tests") + "/kernels/kernel.cl";
    cl_uint platformIdCount = 0;
    clGetPlatformIDs (0, NULL, &platformIdCount);
    if (platformIdCount == 0) {
        ROS_ERROR("No OpenCL platform found");
        exit (1);
    }
    std::vector<cl_platform_id> platformIds (platformIdCount);
    clGetPlatformIDs (platformIdCount, platformIds.data (), NULL);
    for (cl_uint i = 0; i < platformIdCount; ++i) {
        size_t size = 0;
        clGetPlatformInfo (platformIds [i], CL_PLATFORM_NAME, 0, NULL, &size);
        std::string result;
        result.resize (size);
        clGetPlatformInfo (platformIds [i], CL_PLATFORM_NAME, size, const_cast<char*> (result.data ()), NULL);
        ROS_WARN("\t (%d) : %s", i+1, result.c_str());
    }
    cl_uint deviceIdCount = 0;
    clGetDeviceIDs (platformIds [0], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceIdCount);
    if (deviceIdCount == 0) {
        ROS_ERROR("No OpenCL devices found");
        exit (1);
    }
    deviceIds  = std::vector<cl_device_id>(deviceIdCount);
    clGetDeviceIDs (platformIds [0], CL_DEVICE_TYPE_ALL, deviceIdCount, deviceIds.data(), NULL);
    for (cl_uint i = 0; i < deviceIdCount; ++i) {
        size_t size = 0;
        clGetDeviceInfo (deviceIds [i], CL_DEVICE_NAME, 0, NULL, &size);
        std::string result;
        result.resize (size);
        clGetDeviceInfo (deviceIds [i], CL_DEVICE_NAME, size, const_cast<char*> (result.data ()), NULL);
        ROS_WARN("\t (%d) : %s", i+1, result.c_str());
    }
    const cl_context_properties contextProperties [] = {CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties> (platformIds [0]), 0, 0};
    cl_int error = CL_SUCCESS;
    context = clCreateContext (contextProperties, deviceIdCount, deviceIds.data (), NULL, NULL, &error);
    checkError (error);
    program = createProgram (LoadKernel (full_kernel_path.c_str()), context);
    checkError (clBuildProgram (program, deviceIdCount, deviceIds.data (), "-D FILTER_SIZE=1", NULL, NULL));
    kernel = clCreateKernel (program, "imageProc", &error);
    checkError (error);
    pub = nh.advertise<sensor_msgs::Image>(result_topic, 1);
    ros::Subscriber s = nh.subscribe (in_topic, 1, callback);
    while(ros::ok()){
        ros::spin();
    }
    clReleaseKernel (kernel);
    clReleaseProgram (program);
    clReleaseContext (context);
}
