#include <string>
#include <ros/ros.h>
#include <sensor_msgs/Image.h>

ros::Publisher pub;
sensor_msgs::Image prev_msg;

void callback (const sensor_msgs::Image& msg){
    if (prev_msg.data.size() > 0){
        sensor_msgs::Image result = sensor_msgs::Image(msg);
        for(unsigned i = 0; i < msg.data.size(); i++){
            // TODO more complicated example
            if (abs(prev_msg.data[i] - msg.data[i]) < 20){
                result.data[i] = 0;
            }
        }
        pub.publish(result);
    }
    prev_msg = msg;
}

int main (int argc, char** argv){
    ros::init (argc, argv, "ros_opencl_tests_cpu");
    ros::NodeHandle nh;
    std::string int_topic;
    std::string result_topic;
    nh.param("ros_opencl_tests_cpu/int_topic", int_topic, std::string("/usb_cam/image_raw"));
    nh.param("ros_opencl_tests_cpu/result_topic", result_topic, std::string("ros_opencl_tests_cpu/result"));
    pub = nh.advertise<sensor_msgs::Image>(result_topic, 1);
    ros::Subscriber s = nh.subscribe (int_topic, 1, callback);
    while(ros::ok()){
        ros::spin();
    }
}
