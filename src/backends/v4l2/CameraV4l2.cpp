#ifdef __cplusplus
extern "C" {
#endif

#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>

#ifdef __cplusplus
}
#endif

#include "CameraV4l2.h"

CameraV4l2::CameraV4l2() : mCameraDev(-1) {

}

bool CameraV4l2::open(int index) {
    char path[512] = { 0 };
    sprintf(path, "/dev/video%d", index);
    mCameraDev = ::open(path, O_RDWR);
    if (mCameraDev < 0) {
        char errmsg[512] = { 0 };
        sprintf(errmsg, "Cannot open camera device: %s.", path);
        CAMERA_LOG_ERROR(errmsg);
    }
    char info[512] = { 0 };
    sprintf(info, "Open camera success: %s.", path);
    CAMERA_LOG_INFO(info);
    return true;
}

bool CameraV4l2::read(cv::Mat &frame) {

    return true;
}

bool CameraV4l2::close() {

    return true;
}

CameraV4l2::~CameraV4l2() {

}

bool CameraV4l2::ioControl(const std::string &errmsg) {
    return true;
}

void CameraV4l2::commonErrorHandle(const std::string &errmsg) {
}

