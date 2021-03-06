#include <string>
#include <ros/ros.h>
#include <sensor_msgs/Image.h>

ros::Publisher pub;
int blur_radius;

void callback (const sensor_msgs::Image& msg){
    sensor_msgs::Image result = sensor_msgs::Image(msg);
    for(int i = 0; i < msg.data.size(); i += 3){
        unsigned sum[3] = {result.data[i], result.data[i+1], result.data[i+2]};
        unsigned cnt = 1;
        for(unsigned b = 0; b < blur_radius; b++){
            if((int)(i - b*msg.step) >= 0){
                sum[0] += msg.data[i-b*msg.step];
                sum[1] += msg.data[i+1-b*msg.step];
                sum[2] += msg.data[i+2-b*msg.step];
                cnt++;
            }
            if((int)(i - b*msg.step - 3) >= 0){
                sum[0] += msg.data[i-b*msg.step-3];
                sum[1] += msg.data[i+1-b*msg.step-3];
                sum[2] += msg.data[i+2-b*msg.step-3];
                cnt++;
            }
            if((int)(i - b*msg.step + 3) >= 0){
                sum[0] += msg.data[i-b*msg.step+3];
                sum[1] += msg.data[i+1-b*msg.step+3];
                sum[2] += msg.data[i+2-b*msg.step+3];
                cnt++;
            }
            if((int)(i + b*msg.step) < msg.data.size()){
                sum[0] += msg.data[i+b*msg.step];
                sum[1] += msg.data[i+1+b*msg.step];
                sum[2] += msg.data[i+2+b*msg.step];
                cnt++;
            }
            if((int)(i + b*msg.step - 3) < msg.data.size()){
                sum[0] += msg.data[i+b*msg.step-3];
                sum[1] += msg.data[i+1+b*msg.step-3];
                sum[2] += msg.data[i+2+b*msg.step-3];
                cnt++;
            }
            if((int)(i + b*msg.step + 3) < msg.data.size()){
                sum[0] += msg.data[i+b*msg.step+3];
                sum[1] += msg.data[i+1+b*msg.step+3];
                sum[2] += msg.data[i+2+b*msg.step+3];
                cnt++;
            }
        }
        result.data[i] = sum[0] / cnt;
        result.data[i+1] = sum[1] / cnt;
        result.data[i+2] = sum[2] / cnt;
    }
    pub.publish(result);
}

int main (int argc, char** argv){
    ros::init (argc, argv, "ros_opencl_tests_cpu");
    ros::NodeHandle nh;
    std::string in_topic;
    std::string result_topic;
    nh.param("ros_opencl_tests_cpu/in_topic", in_topic, std::string("/usb_cam/image_raw"));
    nh.param("ros_opencl_tests_cpu/result_topic", result_topic, std::string("ros_opencl_tests_cpu/result"));
    nh.param("ros_opencl_tests_cpu/blur_radius", blur_radius, 5);
    pub = nh.advertise<sensor_msgs::Image>(result_topic, 1);
    ros::Subscriber s = nh.subscribe (in_topic, 1, callback);
    while(ros::ok()){
        ros::spin();
    }
}
