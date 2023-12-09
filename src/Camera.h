#ifndef CAMERA_H
#define CAMERA_H

#ifdef _DEBUG
#define CAMERA_LOG_INFO(msg) std::cout << msg << std::endl;
#define CAMERA_LOG_WARN(msg) std::clog << msg << std::endl;
#define CAMERA_LOG_ERROR(msg) std::cerr << msg << std::endl;
#else
#define CAMERA_LOG_INFO(msg)
#define CAMERA_LOG_WARN(msg)
#define CAMERA_LOG_ERROR(msg)
#endif

#include "opencv2/opencv.hpp"

class Camera {
public:
    virtual bool open(int index) = 0;
    virtual bool read(cv::Mat &frame) = 0;
    virtual bool close() = 0;
};

#endif
