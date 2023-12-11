#include <iostream>

#include "CameraCreator.hpp"

int main() {
    // Create a camera. 
    // For now just support v4l2 backend.
    auto camera = CameraCreator::create();     
    // Create Camera format.
    Camera::Format format;
    format.width = 640;
    format.height = 480;
    format.imgFormat = Camera::ImgFormat::NONE;
    // Open camera with spec format.
    // Or you can simplify use "camera->open(0)" without format.
    // If camera open failed would be return false.
    if (camera->open(0, format) == false) {
        return -1;
    }
    // Create a default frame fill with black color.
    cv::Mat frame(480, 640, CV_8UC3, cv::Scalar(0, 0, 0));
    // Loop
    while (true) {
        // Read a frame from camera. If read failed it would return false.
        // This function would not clear frame if read failed.
        if (camera->read(frame) == false) {
            break;
        }
        // Show image.
        cv::imshow("image", frame);
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }
    // Destroy window with image.
    cv::destroyWindow("image");
    // Close camera. This function also called by destructor. 
    // If it already closed. The destructor would not close again.
    camera->close();
    return 0;
}
