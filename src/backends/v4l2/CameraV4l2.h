#ifndef CAMERA_V4L2_H
#define CAMERA_V4L2_H

#include "opencv2/opencv.hpp"
#include "Camera.h"


class CameraV4l2 : public Camera {
private:
    int mCameraDev;
private:
    bool ioControl(const std::string &errmsg);
    void commonErrorHandle(const std::string &errmsg);
public:
    CameraV4l2();
    virtual bool open(int index) override;
    virtual bool read(cv::Mat &frame) override;
    virtual bool close() override;
    virtual ~CameraV4l2();
};

#endif
