/*
 * frameManager.h
 *
 *  Created on: 21.02.2013
 *      Author: Johannes Goth (cmm-jg)
 */

#ifndef FRAMEMANAGER_H_
#define FRAMEMANAGER_H_


// own stuff
#include "frameManager.h"
#include "videoRecorder.h"
#include "videoRecorder.cpp"
// libraries
#include <boost/thread.hpp>
#include <vector>
// ROS includes
#include "ros/ros.h"
#include "sensor_msgs/Image.h"
#include "ImageBuilder.h"
// openCV includes
#include <cv_bridge/cv_bridge.h>
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class FrameManager {
public:
	// public member functions
	FrameManager();
	FrameManager(ros::NodeHandle &nHandler);
	virtual ~FrameManager();
	void processFrame(const sensor_msgs::Image& img);
	int getVideo();
	bool isSnapShotRunning(){return snapshotRunning;};
	void startSnapshots(int interval);
	void stopSnapshots();
	bool isLiveStreamRunning(){return liveStreamRunning;};
	void startLiveStream();
	void stopLiveStream();

private:

	// private member functions
	void cacheFrame(sensor_msgs::Image frame);
	void verifyCacheSize();
	void storeCache(std::vector<sensor_msgs::Image>* cache, bool* threadActive);
	int createVideo();
	std::vector<sensor_msgs::Image>* getCurrentCache();
	void storeFrame(sensor_msgs::Image frame);
	void displayFrame(cv::Mat* mat);
	cv::Mat convertTemperatureValuesToRGB(sensor_msgs::Image* frame, unsigned int* frameCount);
	void createSnapshots(int interval);

	// state machine
	enum states {ON_DEMAND, LIVE_STREAM};
	int stateMachine;

	// cache attributes and references (memory buffers)
	u_int fpv;			// frames per video
	u_int fpc; 			// frames per cache
	u_int fpb; 			// frames per binary
	bool fullVideoAvailable;
	bool createVideoActive;
	bool usingCacheA;
	bool usingCacheB;
	std::vector<sensor_msgs::Image>* cacheA;
	std::vector<sensor_msgs::Image>* cacheB;

	// file storage parameters
	std::string binaryFilePath;
	std::string videoFilePath;
	u_int binaryFileIndex;
	std::vector<boost::mutex*> binaryFileMutexes;

	// termo-to-rgb converter
	optris::ImageBuilder iBuilder;
	bool showFrame;

	// output video parameters
	u_int vfr;			// video frame rate
	int videoCodec; 	// Codec for video coding eg. CV_FOURCC('D','I','V','X')

	// threads parameters
	boost::thread storingThreadA;
	boost::thread storingThreadB;
	boost::thread tCaching;
	bool storingCacheA;
	bool storingCacheB;
	boost::thread creatingVideoThread;
	boost::thread cachingThread;
	boost::thread snapshotThread;
	bool snapshotRunning;
	bool liveStreamRunning;
};

#endif /* FRAMEMANAGERH_ */
