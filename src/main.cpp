// Standard include files
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>

using namespace cv;

int main(int argc, char **argv)
{
  //VideoCapture cam("ant3.mp4");
  
  VideoCapture cam(0);

  cam.set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
  cam.set(CV_CAP_PROP_FPS, 30);
  cam.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
  cam.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);
  
  if (!cam.isOpened()) {
    std::cerr << "no camera detected" << std::endl;
    std::cin.get();
    return 0;
  }

  int width = cam.get(CV_CAP_PROP_FRAME_WIDTH);
  int height = cam.get(CV_CAP_PROP_FRAME_HEIGHT);

  std::cout << "Resolution: " << width << "x" << height << std::endl;

  Mat frame(height, width, CV_8UC1);

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

  Mat prev(height, width, CV_8UC1), curr(height, width, CV_8UC1), next(height, width, CV_8UC1);
  Mat result(height, width, CV_8UC1), d1(height, width, CV_8UC1), d2(height, width, CV_8UC1);

  for (;;) {
    prev = curr;
    curr = next;
    cam >> next;
    blur(next, next, Size(3, 3));

    cvtColor(next, next, CV_RGB2GRAY);

    absdiff(curr, next, d1);
    //absdiff(curr, prev, d2);

    //bitwise_and(d1, d2, result);
    //threshold(result, result, 35, 255, CV_THRESH_BINARY);
    if (mean(d1)[0] > 1) continue;
    //if (mean(d1)[0] > 0.1) continue;

    threshold(d1, d1, 5, 255, CV_THRESH_BINARY);
    result += d1;
    //result -= 1;

    imshow("", result);
    if (waitKey(10) == 27)
      break;
  }
}