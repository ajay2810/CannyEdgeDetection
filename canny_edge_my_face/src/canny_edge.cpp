/*********************************************************************
*
*  Copyright (c) 2011, Ajay Kumar
*  All rights reserved.
*
*********************************************************************/
/** This node subscribes to the Image published by custom_cv_camera,
 * it converts the image to an OpenCV format so it can be processed.
 * The Canny Filter is applied and then the resulting image is converted again to a ROS image 
 * so it can be published into a topic and be used by other nodes.
 * Note: the Canny algorithm is the one showed on the OpenCV documentation tutorials
**/

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dynamic_reconfigure/server.h>
#include <canny_edge_my_face/ThresholdConfig.h>

namespace enc = sensor_msgs::image_encodings;

// Global variables
cv::Mat src, src_gray;  // Image and subimages used for processing
cv::Mat dst, detected_edges;

int lowThreshold = 50;  // If a pixel gradient value is below the lower threshold, then it is rejected
int const max_lowThreshold = 100;  // Maximum lower threshold
int ratio = 3;  // An upper threshold is needed for the Canny filter,
                // the recommended ratio between upper and lower thereshold is 2:1 or 3:1
int kernel_size = 3;  // Kernel size for the Sobel operations to be performed internally by the Canny function

// The lower threshold could be changed by using a dynamic parameter configuration
void callback(canny_edge_my_face::ThresholdConfig &config, uint32_t level)
{
  lowThreshold = config.thres;
}

// Callback used to transform the incoming ROS Image into an OpenCV Image
void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
  cv_bridge::CvImagePtr src_;  // OpenCV full Image variable
  try
  {
    src_ = cv_bridge::toCvCopy(msg, enc::BGR8);  // Conversion
    src = src_->image;  // Assign just the image information to a Mat variable
  }
  catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("cv_bridge exception: %s", e.what());
    return;
  }
}

// Function which applies the Canny Filter
void CannyThreshold(int, void*)
{
  blur(src_gray, detected_edges, cv::Size(3, 3));   // Noise reduction
  // Canny detector OpenCV function
  Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * ratio, kernel_size);
  dst = cv::Scalar::all(0);  // Create a mask
  src.copyTo(dst, detected_edges);  // Apply the mask to the raw Image
}

// Main function
int main(int argc, char** argv)
{
  ros::init(argc, argv, "canny_edge_node");
  ros::NodeHandle n;

  image_transport::ImageTransport it(n);
  image_transport::Publisher pub = it.advertise("/canny", 1);  // Filtered image publisher
  image_transport::Subscriber sub = it.subscribe("/camera/image", 1, imageCallback);  // custom_cv_camera subscriber

  // Dynamic reconfigure
  dynamic_reconfigure::Server<canny_edge_my_face::ThresholdConfig> server;
  dynamic_reconfigure::Server<canny_edge_my_face::ThresholdConfig>::CallbackType f;
  f = boost::bind(&callback, _1, _2);
  server.setCallback(f);

  ros::Rate loop_rate(5);
  sensor_msgs::ImagePtr msg_;  // Image message for publisher
  while (ros::ok())
  {
    if (!src.empty())
    {
      dst.create(src.size(), src.type());   // Create a matrix with raw Image size for future mask creation
      cv::cvtColor(src, src_gray, CV_BGR2GRAY);   // RGB to Gray Scale
      CannyThreshold(0, 0);  // Canny function
      msg_ = cv_bridge::CvImage(std_msgs::Header(), "bgr8", dst).toImageMsg();  // OpenCV filtered image to ROS Image
      pub.publish(msg_);  // Filtered image publication
      cv::waitKey(1);
    }

    ros::spinOnce();
    loop_rate.sleep();
  }

  return 0;
}
