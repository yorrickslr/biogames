// Standard include files
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/features2d.hpp>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <math.h>

using namespace cv;
using namespace std;

// set gloabal tracking variables
int brightnessThreshold = 25;
int camExposure = -6;
int usedBackground = 95;

void onExposure(int slider_val, void* cam)
{
	((cv::VideoCapture*)cam)->set(CV_CAP_PROP_EXPOSURE, slider_val - 6);
}

void onBackground(int slider_val, void* cam) {}

void onBrightness(int slider_val, void* cam) {}

void contourDetection(Mat &inputImage) {
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// Find contours
	findContours(inputImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));

	/// Get the moments
	vector<Moments> mu(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		mu[i] = moments(contours[i], false);
	}

	///  Get the mass centers:
	vector<Point2f> mc(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
	}

	/// Draw contours
	Mat drawing = Mat::zeros(inputImage.size(), CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
	{
		Scalar color = Scalar(0, 0, 255);
		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
		circle(drawing, mc[i], 4, color, -1, 8, 0);
	}
	/// Show in a window
	namedWindow("Contours", CV_WINDOW_AUTOSIZE);
	imshow("Contours", drawing);
}

VideoCapture setCamera(int &exposure) {
	// cv::VideoCapture cam("input3.avi");
	cv::VideoCapture cam(0);

	cam.set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
	cam.set(CV_CAP_PROP_FPS, 30);
	cam.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	cam.set(CV_CAP_PROP_AUTO_EXPOSURE, 0);
	cam.set(CV_CAP_PROP_EXPOSURE, exposure);
	cam.set(CV_CAP_PROP_GAIN, 1);
	cam.set(CV_CAP_PROP_FOCUS, 1);

	if (!cam.isOpened()) {
		std::cerr << "no camera detected" << std::endl;
		std::cin.get();
		return 0;
	}
	return cam;
}

//void createControlWindow(int &set1, int &set2, int &set3, VideoCapture &cam) {
void createControlWindow(VideoCapture &cam) {
	cvNamedWindow("control", 1);
	//exposure slider
	int a = cv::createTrackbar("exposure", "control", &camExposure, 5, onExposure, &cam);
	//background adaption slider
	int b = cv::createTrackbar("background", "control", &usedBackground, 100, onBackground);
	//brightness slider
	int c = cv::createTrackbar("brightness", "control", &brightnessThreshold, 50, onBrightness);
}

void measureFramerate(VideoCapture &cam) {
	// std::cout << "measuring fps... ";
	// time_t start, end;
	// time(&start);
	// for (int i = 0; i < 64; i++) {
	// 	cam >> frame;
	// }
	// time(&end);
	// std::cout << 64/difftime(end, start) << "fps" << std::endl;
	// std::cout << "press any key to continue..." << std::endl;
	// std::cin.get();
}

void startTracking(VideoCapture &cam) {
	int width = cam.get(CV_CAP_PROP_FRAME_WIDTH);
	int height = cam.get(CV_CAP_PROP_FRAME_HEIGHT);
	std::cout << "Resolution: " << width << "x" << height << std::endl;

	//measureFramerate(cam);

	//create matrizes for further use
	cv::Mat frameFromCamera(height, width, CV_8UC3), inputToBlob(height, width, CV_8UC3), result(height, width, CV_8UC3);
	cv::Mat curr(height, width, CV_32F), diff(height, width, CV_32F), background(height, width, CV_32F);

	cam >> frameFromCamera;
	frameFromCamera.convertTo(background, CV_32F);
	result = background;
	cv::Mat grayDiff(height, width, CV_8UC3);
	for (double f = 1;; f++) {
		cam >> frameFromCamera;
		frameFromCamera.convertTo(curr, CV_32F);
		//blur(curr, curr, cv::Size(3, 3));

		// adapt the background slowly to current changes
		float fraction = exp(-usedBackground);
		background *= (1.0 - fraction);
		background += (fraction)* curr;

		// create diff-image out of the background and the current image
		cv::absdiff(background, curr, diff);
		cvtColor(diff, grayDiff, CV_RGB2GRAY);
		grayDiff.convertTo(inputToBlob, CV_8UC1);
		cv::threshold(inputToBlob, inputToBlob, brightnessThreshold, 255, CV_THRESH_BINARY);

		std::cerr << "start detection" << std::endl;
		contourDetection(inputToBlob);
		std::cerr << 1 << std::endl;

		std::cerr << 2 << std::endl;
		//imshow("1", result);
		imshow("2", frameFromCamera);
		//imshow("3", background);
		if (cv::waitKey(10) == 27)
			break;
	}
}

int main(int argc, char **argv)
{
	VideoCapture cam = setCamera(camExposure);

	//createControlWindow(camExposure, usedBackground, brightnessThreshold,cam);
	createControlWindow(cam);

	startTracking(cam);
}
