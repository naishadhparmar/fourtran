#include "FullSystem/FullSystem.h"
#include "util/settings.h"
#include "util/Undistort.h"
#include "ros/ros.h"
#include "sensor_msgs/Image.h"
#include "monocular_odometry/DSOWrapper.h"

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
using namespace dso;

FullSystem* fullSystem = 0;
Undistort* undistorter = 0;

string calib = "";
string vignetteFile = "";
string gammaFile = "";
long frameID = 0L;

void imageCallback(const sensor_msgs::Image::ConstPtr& msg)
{
  cv_bridge::CvImagePtr cv_img_ptr;
  try
  {
    cv_img_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    MinimalImageB minImg((int)cv_img_ptr->image.cols, (int)cv_img_ptr->image.rows,(unsigned char*)cv_img_ptr->image.data);
    	ImageAndExposure* undistImg = undistorter->undistort<unsigned char>(&minImg, 1,0, 1.0f);
    	fullSystem->addActiveFrame(undistImg, frameID++);
  }
  catch(cv_bridge::Exception& e)
  {
    ROS_ERROR("cv_bridge exception: %s", e.what());
    return;
  }
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "monocular_odometry");
  ros::NodeHandle nh;

  undistorter = Undistort::getUndistorterForFile("", "", "");
  setGlobalCalib(
           (int)undistorter->getSize()[0],
           (int)undistorter->getSize()[1],
           undistorter->getK().cast<float>());
  fullSystem = new FullSystem();
  fullSystem->linearizeOperation=false;
  IOWrap::DSOWrapper * lol = new IOWrap::DSOWrapper();
  fullSystem->outputWrapper.push_back(lol);
  image_transport::ImageTransport it(nh);
  image_transport::Subscriber image_raw_sub = it.subscribe("/cam_lifecam/image_raw", 1, imageCallback);
  ros::spin();
}
