// Standard include files
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/features2d.hpp>

#include <string>

int main(int argc, char **argv)
{
  cv::VideoCapture cam("input3.avi");
  
  /*cv::VideoCapture cam(0);

  cam.set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
  cam.set(CV_CAP_PROP_FPS, 30);
  cam.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
  cam.set(CV_CAP_PROP_FRAME_HEIGHT, 720);*/
  
  if (!cam.isOpened()) {
    std::cerr << "no camera detected" << std::endl;
    std::cin.get();
    return 0;
  }

  int width = cam.get(CV_CAP_PROP_FRAME_WIDTH);
  int height = cam.get(CV_CAP_PROP_FRAME_HEIGHT);

  std::cout << "Resolution: " << width << "x" << height << std::endl;

  // measure framerate
  /*
  std::cout << "measuring fps... ";
  time_t start, end;
  time(&start);
  for (int i = 0; i < 64; i++) {
    cam >> frame;
  }
  time(&end);
  std::cout << 64/difftime(end, start) << "fps" << std::endl;
  std::cout << "press any key to continue..." << std::endl;
  std::cin.get();
  */

  cv::Mat frame(height, width, CV_8UC1), output(height, width, CV_8UC1), result(height, width, CV_8UC1);
  cv::Mat curr(height, width, CV_64F), diff(height, width, CV_64F), mean(height, width, CV_64F);
  
  /*
  cv::VideoWriter output("output.avi", 0, 30.0, cv::Size(width, height), false);
  if (!output.isOpened()) {
	  std::cerr << "scheisse" << std::endl;
  }*/

  cv::SimpleBlobDetector::Params params;

  // Change thresholds
  params.minThreshold = 3;
  params.maxThreshold = 100;

  // Filter by Area.
  params.filterByArea = true;
  params.minArea = 3;

  cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);
  std::vector<cv::KeyPoint> keypoints;
  
  cam >> frame;
  frame.convertTo(mean, CV_64F);
  result = mean;
  for (double f=1;;f++) {
	cam >> frame;
	frame.convertTo(curr, CV_64F);
    //blur(curr, curr, cv::Size(3, 3));

    //cvtColor(curr, curr, CV_RGB2GRAY);

	mean *= 0.95;
	mean += 0.05 * curr;
	cv::absdiff(mean, curr, diff);
	cv::threshold(diff, diff, 25, 255, CV_THRESH_BINARY);
	diff.convertTo(output, CV_8UC1);

	std::cerr << 0 << std::endl;
	detector->detect(output, keypoints);
	std::cerr << 1 << std::endl;

	cv::drawKeypoints(output, keypoints, result, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	std::cerr << 2 << std::endl;
	imshow("", result);
	//cv::absdiff(mean, curr, diff);
    //bitwise_and(d1, d2, result);
    //threshold(result, result, 35, 255, CV_THRESH_BINARY);
    //if (cv::mean(diff)[0] > 1) continue;
    //if (mean(d1)[0] > 0.1) continue;

    //threshold(diff, diff, 5, 255, CV_THRESH_BINARY);
	//result += diff;
    //result -= 1;

    //imshow("", mean);
	//output.write(curr);
    if (cv::waitKey(10) == 27)
      break;
  }
  //output.release();

  // blob detection
  // diff only
  // mean of previous frames
}
