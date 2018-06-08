#include <string>
#include <ros/ros.h>
#include <sensor_msgs/Image.h>

ros::Publisher pub;
sensor_msgs::Image prev_msg;

void callback (const sensor_msgs::Image& msg){
    if (prev_msg.data.size() > 0){
        sensor_msgs::Image result = sensor_msgs::Image(msg);
        for(int i = 0; i < msg.data.size(); i+=3){
            unsigned sum[3] = {result.data[i], result.data[i+1], result.data[i+2]};
            unsigned cnt = 1;
            if((int)(i - msg.step) >= 0){
                sum[0] += msg.data[i-msg.step];
                sum[1] += msg.data[i+1-msg.step];
                sum[2] += msg.data[i+2-msg.step];
                cnt++;
            }
            if((int)(i - msg.step - 3) >= 0){
                sum[0] += msg.data[i-msg.step-3];
                sum[1] += msg.data[i+1-msg.step-3];
                sum[2] += msg.data[i+2-msg.step-3];
                cnt++;
            }
            if((int)(i - msg.step + 3) >= 0){
                sum[0] += msg.data[i-msg.step+3];
                sum[1] += msg.data[i+1-msg.step+3];
                sum[2] += msg.data[i+2-msg.step+3];
                cnt++;
            }
            if((int)(i + msg.step) < msg.data.size()){
                sum[0] += msg.data[i+msg.step];
                sum[1] += msg.data[i+1+msg.step];
                sum[2] += msg.data[i+2+msg.step];
                cnt++;
            }
            if((int)(i + msg.step - 3) < msg.data.size()){
                sum[0] += msg.data[i+msg.step-3];
                sum[1] += msg.data[i+1+msg.step-3];
                sum[2] += msg.data[i+2+msg.step-3];
                cnt++;
            }
            if((int)(i + msg.step + 3) < msg.data.size()){
                sum[0] += msg.data[i+msg.step+3];
                sum[1] += msg.data[i+1+msg.step+3];
                sum[2] += msg.data[i+2+msg.step+3];
                cnt++;
            }
            if((int)(i - 3) >= 0){
                sum[0] += msg.data[i-3];
                sum[1] += msg.data[i+1-3];
                sum[2] += msg.data[i+2-3];
                cnt++;
            }
            if((int)(i + 3) < msg.data.size()){
                sum[0] += msg.data[i+3];
                sum[1] += msg.data[i+1+3];
                sum[2] += msg.data[i+2+3];
                cnt++;
            }
            result.data[i] = sum[0] / cnt;
            result.data[i+1] = sum[1] / cnt;
            result.data[i+2] = sum[2] / cnt;
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
