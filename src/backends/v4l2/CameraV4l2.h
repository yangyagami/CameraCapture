#ifndef CAMERA_V4L2_H
#define CAMERA_V4L2_H

#include <array>

#include "opencv2/opencv.hpp"
#include "Camera.h"


class CameraV4l2 : public Camera {
private:
    int mCameraDev;
    std::array<char *, 100> mMapBuffers;
    std::array<size_t, 100> mMapSizes;
private:
    bool ioControl(int request, void *output);
private:
    bool setformat();
    bool getformat();
    bool setInput();
    bool queryInfo();
    bool requestBuffers();
public:
    CameraV4l2();
    virtual bool open(int index) override;
    virtual bool read(cv::Mat &frame) override;
    virtual bool close() override;
    virtual ~CameraV4l2();
};

#endif
