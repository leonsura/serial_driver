#include "ros/ros.h"
#include <serial/serial.h>
#include <std_msgs/String.h>
#include <std_msgs/Empty.h>

serial::Serial serial_t;

void callback(const std_msgs::String::ConstPtr& msg) {
	ROS_INFO_STREAM("Write to serial port" << msg->data);
	serial_t.write(msg->data);
}

int main (int argc, char** argv) {
	ros::init(argc, argv, "serial_node");
	ros::NodeHandle nh;

	ros::Subscriber serial_sub = nh.subscribe("serial_tx", 1000, callback);
	ros::Publisher serial_pub = nh.advertise<std_msgs::String>("serial_rx", 1000);

	try
	{
		serial_t.setPort("/dev/ttyUSB0");
		serial_t.setBaudrate(115200);
		serial::Timeout to = serial::Timeout::simpleTimeout(1000);
		serial_t.setTimeout(to);
		serial_t.open();
	}
	catch (serial::IOException& e)
	{
		ROS_ERROR_STREAM("Unable to open port ");
	}

	if (serial_t.isOpen())
	{
		ROS_INFO_STREAM("Serial Port Opened");
	}
	else
	{
		return -1;
	}

	ros::Rate loop_rate(100);
	while (ros::ok()){
		ros::spinOnce();

		if (serial_t.available()) {
			ROS_INFO_STREAM("Reading from serial port");
			std_msgs::String serial_data;

			serial_data.data = serial_t.read(serial_t.available());
			ROS_INFO_STREAM("Read: " << serial_data.data);

			serial_pub.publish(serial_data);
		}

		loop_rate.sleep();
	}
}

