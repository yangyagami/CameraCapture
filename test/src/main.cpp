#include <iostream>

#include "CameraCreator.hpp"

int main() {
    auto camera = CameraCreator::create();
    if (camera->open(0) == false) {
        return -1;
    }
    cv::Mat frame(480, 640, CV_8UC3, cv::Scalar(0, 0, 0));
    while (true) {
        if (camera->read(frame) == false) {
            break;
        }
        cv::imshow("image", frame);
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }
    cv::destroyWindow("image");
    camera->close();
    return 0;
}
