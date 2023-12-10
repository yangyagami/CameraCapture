# Build
First you need install xmake && opencv.
```bash
xmake && xmake run
```
# Usage
```cpp
#include <iostream>

#include "CameraCreator.hpp"

int main() {
    // Create a camera for now just support v4l2 backend.
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
```
# Log Info
Build with debug or check mode would enable log.
```bash
# or -m check
xmake f -m debug && xmake
xmake run
```
Output:
```bash
INFO: Open camera success: /dev/video0. -> (src/backends/v4l2/CameraV4l2.cpp:37:open)
INFO: Driver name: Droidcam
Card name: Droidcam
Bus info: platform:v4l2loopback_dc-000
Version: 393526
 -> (src/backends/v4l2/CameraV4l2.cpp:233:queryInfo)
INFO: Format: Y U 1 2
Format dexcription: Planar YUV 4:2:0
 -> (src/backends/v4l2/CameraV4l2.cpp:253:queryInfo)
INFO: width: 640
height: 480
 -> (src/backends/v4l2/CameraV4l2.cpp:313:getformat)
ERROR: Cannot open camera device: /dev/video1. -> (src/backends/v4l2/CameraV4l2.cpp:32:open)
error: execv(/home/yangsiyu/Workspace/CameraCapture/build/linux/x86_64/check/CameraCaptureTest ) failed(255)
```
