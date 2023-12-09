#ifndef CAMERA_H
#define CAMERA_H

#ifdef _DEBUG
#define CAMERA_LOG_INFO(msg) std::cout << "\033[1;37mINFO\033[0m: " << msg << " -> (" << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ")" << std::endl;
#define CAMERA_LOG_WARN(msg) std::clog << "\033[1;33mWARN\033[0m: " << msg << " -> (" << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ")" << std::endl;
#define CAMERA_LOG_ERROR(msg) std::cerr << "\033[1;31mERROR\033[0m: " << msg << " -> (" << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ")" << std::endl;
#define MESURE(code) { \
    auto start = clock(); \
    code \
    auto diff = clock() - start; \
    char info[512] = { 0 }; \
    sprintf(info, "elapsed time: %0.6f sec.", (double)diff / CLOCKS_PER_SEC); \
    CAMERA_LOG_INFO(info); \
}
#else
#define CAMERA_LOG_INFO(msg)
#define CAMERA_LOG_WARN(msg)
#define CAMERA_LOG_ERROR(msg)
#define MESURE(code) code
#endif

#include "opencv2/opencv.hpp"

class Camera {
public:
    virtual bool open(int index) = 0;
    virtual bool read(cv::Mat &frame) = 0;
    virtual bool close() = 0;
};

#endif
