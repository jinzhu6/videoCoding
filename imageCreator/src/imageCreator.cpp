/*
 * author: Johannes Goth (cmm-jg)
 *
 */

#include "ros/ros.h"
#include "sensor_msgs/Image.h"
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include "opencv2/core/core.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace enc = sensor_msgs::image_encodings;

void transmitImage(){
	// TODO: creating a socket connection to remote host
}

// global attributes
const float SNAPSHOT_INTERVAL = 0.5;
int imageCounter = 0;


void imageCallback(const sensor_msgs::Image& img)
{
	ROS_INFO("Image callback method ...");

	// openCV image pointer
	cv_bridge::CvImageConstPtr cvptrS;

	// file name and path to the image files
	std::stringstream imgFile;

	try
	{
		// convert the sensor_msgs::Image to cv_bridge::CvImageConstPtr
		cvptrS = cv_bridge::toCvShare(img, cvptrS, enc::BGR8);

		// TODO: save generic path with getlogin_r() defined in unistd.h
		imgFile << "/home/cmm-jg/Bilder/snapshot" << imageCounter << ".jpg";

		if(cvptrS != NULL){
			// write cv::Mat image to hdd as jpeg
			cv::imwrite(imgFile.str(), cvptrS->image);
		}
		imageCounter++;
	}
	catch (cv_bridge::Exception& e)
	{
		ROS_ERROR("cv_bridge exception: %s", e.what());
		return;
	}

	/*
	// sensor_imgs -> OpenCV  Copy
	cv_bridge::CvImagePtr cvptrC;
	try
	{
		cvptrC = cv_bridge::toCvCopy(img, enc::MONO8);
		//Mat save_img;

		//cv::imwrite("test.jpg", cvptrC->image);
	}
	catch (cv_bridge::Exception& e)
	{
		ROS_ERROR("cv_bridge exception: %s", e.what());
		return;
	}
	 */
}

int main(int argc, char **argv)
{

	ros::init(argc, argv, "imageCreator");

	// main access point to communications with the ROS system
	ros::NodeHandle n;
	ROS_INFO("subscribing for thermal_image_view ...");
	ros::Subscriber sub = n.subscribe("/optris/thermal_image_view", 2, imageCallback);

	ros::Rate loop_rate(SNAPSHOT_INTERVAL);

	while(ros::ok()){
		ros::spinOnce();
		loop_rate.sleep();
	}

	return 0;
}
