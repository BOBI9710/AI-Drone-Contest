#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cv_bridge/cv_bridge.h>

#include <iostream>
#include <iomanip>

using namespace cv;
using namespace std;

void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
  try
  {
    Mat img = cv_bridge::toCvShare(msg, "bgr8")->image;
    
    Mat gray_img;
    cvtColor(img, gray_img, CV_BGR2GRAY);
    
    Mat blur_img;
    GaussianBlur(gray_img, blur_img, Size(7,7), 2, 2);

	Mat img_houghC;
	blur_img.copyTo(img_houghC);
	
	vector<Vec3f> circles;
	HoughCircles(blur_img, circles, CV_HOUGH_GRADIENT, 1, 30, 130, 50, 0, 0);
	
	for (size_t i = 0; i < circles.size(); i++)
	{
		
                Vec3i c = circles[i];
		Point center(c[0], c[1]);
		int radius = c[2];

		circle(img_houghC, center, radius, Scalar(0, 255, 0), 2);
		circle(img_houghC, center, 2, Scalar(0, 0, 255), 3);
	}


    imshow("view", img_houghC);
    cv::waitKey(30);
  }
  catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
  }

}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "image_process");
  ros::NodeHandle nh;
  cv::namedWindow("view");

  image_transport::ImageTransport it(nh);
  image_transport::Subscriber sub = it.subscribe("/iris/usb_cam/image_raw", 1, imageCallback);
  ros::spin();
  cv::destroyWindow("view");
}
