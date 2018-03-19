#include "dsooutputwrapper.h"
#include "FullSystem/FullSystem.h"
#include "util/settings.h"
#include "util/Undistort.h"
#include "ros/ros.h"
#include "sensor_msgs/Image.h"

#include "opencv2/imgproc/imgproc.hpp"
#include <cv_bridge/cv_bridge.h>

#include <image_transport/image_transport.h>
#include <sensor_msgs/image_encodings.h>
#include <geometry_msgs/PointStamped.h>

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

FullSystem* fullSystem = 0;
Undistort* undistorter = 0;

string calib = "";
string vignetteFile = "";
string gammaFile = "";

void imageCallback(const sensor_msgs::Image::ConstPtr& msg)
{
  cv_bridge::CvImagePtr cv_img_ptr;
  try
  {
    cv_img_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    MinimalImageB minImg((int)cv_img_ptr->image.cols, (int)cv_img_ptr->image.rows,(unsigned char*)cv_img_ptr->image.data);
    	ImageAndExposure* undistImg = undistorter->undistort<unsigned char>(&minImg, 1,0, 1.0f);
    	fullSystem->addActiveFrame(undistImg, frameID);
  }
  catch(cv_bridge::Exception& e)
  {
    ROS_ERROR("cv_bridge exception: %s", e.what());
    return;
  }
}

int main(int argc, char **argv)
{
  ros::init(argc, argc, "monocular_odometry");
  ros::NodeHandle nh;

  undistorter = Undistort::getUndistorterForFile(calib, gammaFile, vignetteFile);
  fullSystem = new FullSystem();
  fullSystem->linearizeOperation=false;
  image_transport::ImageTransport it(nh);
  image_transport::Subscriber image_raw_sub = it.subscribe("/hardware_camera/camera/image_raw", 1, imageCallback);
  ros::spin()
}
