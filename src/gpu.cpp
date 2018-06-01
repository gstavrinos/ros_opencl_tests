#include <string>
#include <ros/ros.h>
#include <ros/package.h>
#include <sensor_msgs/Image.h>
#include <ros_opencl/ros_opencl.hpp>

ros::Publisher pub;
ros_opencl::ROS_OpenCL roscl;
sensor_msgs::Image prev_msg;

void callback (const sensor_msgs::Image& msg){
    if (prev_msg.data.size() > 0){
        sensor_msgs::Image result = sensor_msgs::Image(msg);
        std::vector<char> v(msg.data.begin(), msg.data.end());
        std::vector<char> v2(prev_msg.data.begin(), prev_msg.data.end());
        roscl.process(&v, v2);
        result.data.assign(v.begin(), v.end());
        pub.publish(result);
    }
    prev_msg = msg;
}

int main (int argc, char** argv){
    ros::init (argc, argv, "ros_opencl_tests_gpu");
    ros::NodeHandle nh;
    std::string in_topic;
    std::string result_topic;
    nh.param("ros_opencl_tests_gpu/in_topic", in_topic, std::string("/usb_cam/image_raw"));
    nh.param("ros_opencl_tests_gpu/result_topic", result_topic, std::string("ros_opencl_tests_gpu/result"));
    std::string full_kernel_path = ros::package::getPath("ros_opencl_tests") + "/kernels/kernel.cl";
    roscl = new ros_opencl::ROS_OpenCL(full_kernel_path, "imageProc");
    pub = nh.advertise<sensor_msgs::Image>(result_topic, 1);
    ros::Subscriber s = nh.subscribe (in_topic, 1, callback);
    while(ros::ok()){
        ros::spin();
    }
}