#include <string>
#include <ros/ros.h>
#include <ros/package.h>
#include <sensor_msgs/Image.h>
#include <ros_opencl/ros_opencl.hpp>

ros::Publisher pub;
int blur_radius;
ros_opencl::ROS_OpenCL roscl;

void callback (const sensor_msgs::Image& msg){
    sensor_msgs::Image result = sensor_msgs::Image(msg);
    std::vector<int> v(msg.data.begin(), msg.data.end());
    // Adding a copy of the vector to have the original image too
    v.insert(v.end(), msg.data.begin(), msg.data.end());
    std::vector<int> v2;
    v2.push_back(msg.step);
    v2.push_back(msg.data.size());
    v2.push_back(blur_radius);
    ROS_OpenCL_Params rop;
    rop.global_work_size.push_back(msg.data.size() / 3);
    roscl.process(&v, v2, &rop);
    result.data.assign(v.begin(), v.begin()+msg.data.size());
    pub.publish(result);
}

int main (int argc, char** argv){
    ros::init (argc, argv, "ros_opencl_tests_gpu");
    ros::NodeHandle nh;
    std::string in_topic;
    std::string result_topic;
    nh.param("ros_opencl_tests_gpu/in_topic", in_topic, std::string("/usb_cam/image_raw"));
    nh.param("ros_opencl_tests_gpu/result_topic", result_topic, std::string("ros_opencl_tests_gpu/result"));
    nh.param("ros_opencl_tests_gpu/blur_radius", blur_radius, 5);
    std::string full_kernel_path = ros::package::getPath("ros_opencl_tests") + "/kernels/kernel.cl";
    roscl = new ros_opencl::ROS_OpenCL(full_kernel_path, "imageProc");
    pub = nh.advertise<sensor_msgs::Image>(result_topic, 1);
    ros::Subscriber s = nh.subscribe (in_topic, 1, callback);
    while(ros::ok()){
        ros::spin();
    }
}
