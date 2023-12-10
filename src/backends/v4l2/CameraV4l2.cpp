#ifdef __cplusplus
extern "C" {
#endif

#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#ifdef __cplusplus
}
#endif

#include <algorithm>

#include "CameraV4l2.h"
#include "opencv2/opencv.hpp"

CameraV4l2::CameraV4l2() : mCameraDev(-1) {
    std::fill(mMapBuffers.begin(), mMapBuffers.end(), nullptr);
    std::fill(mMapSizes.begin(), mMapSizes.end(), 0);
}

bool CameraV4l2::open(int index, const Format &format) {
    char path[512] = { 0 };
    sprintf(path, "/dev/video%d", index);
    mCameraDev = ::open(path, O_RDWR | O_NONBLOCK);
    if (mCameraDev < 0) {
        char errmsg[512] = { 0 };
        sprintf(errmsg, "Cannot open camera device: %s.", path);
        CAMERA_LOG_ERROR(errmsg);
        return false;
    }
    char info[512] = { 0 };
    sprintf(info, "Open camera success: %s.", path);
    CAMERA_LOG_INFO(info);

    mFormat = format;

    // get information
    if (queryInfo() == false) {
        ::close(mCameraDev);
        mCameraDev = -1;
        return false;
    }

    // set format
    if (setformat() == false) {
        ::close(mCameraDev);
        mCameraDev = -1;
        return false;
    }

    // set input
    if (setInput() == false) {
        ::close(mCameraDev);
        mCameraDev = -1;
        return false;
    }

    // get format
    if (getformat() == false) {
        ::close(mCameraDev);
        mCameraDev = -1;
        return false;
    }

    // request buffers
    if (requestBuffers() == false) {
        ::close(mCameraDev);
        mCameraDev = -1;
        return false;
    }

    return true;
}

bool CameraV4l2::read(cv::Mat &frame) {
    MESURE(
        struct v4l2_buffer buffer;
        buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer.memory = V4L2_MEMORY_MMAP;
        if (ioControl(VIDIOC_DQBUF, &buffer) == false) {
            CAMERA_LOG_ERROR("Cannot dequeue buffer!");
            return false;
        }

        switch (mFormat.imgFormat) {
        case ImgFormat::YUV420P: 
        {
            cv::Mat yuv420p(480 * 3 / 2, 640, CV_8UC1, (void *)mMapBuffers[buffer.index]);
            cv::cvtColor(yuv420p, frame, cv::COLOR_YUV420p2RGB);
        }
        break;
        case ImgFormat::MJPEG:
        break;
        }

        if (ioControl(VIDIOC_QBUF, &buffer) == false) {
            CAMERA_LOG_ERROR("Cannot enqueue buffer!");
            return false;
        }
    );
    return true;
}

bool CameraV4l2::close() {
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (mCameraDev >= 0 && ioControl(VIDIOC_STREAMOFF, &type) == false) {
        CAMERA_LOG_ERROR("Close stream failed!!!");
    }
    // TODO
    for (int i = 0; i < 4; i++) {
        if (mMapBuffers[i] != nullptr) {
            munmap(mMapBuffers[i], mMapSizes[i]);
        }
    }

    if (mCameraDev >= 0) { 
        ::close(mCameraDev); 
        mCameraDev = -1;
    }
    return true;
}

CameraV4l2::~CameraV4l2() {
    close();
}

bool CameraV4l2::ioControl(int request, void *output) {
    int ret = -1;
    ret = ioctl(mCameraDev, request, output);
    if (ret < 0) {
        if (errno != EINPROGRESS && errno != EAGAIN) {
            CAMERA_LOG_ERROR(strerror(errno));
            return false;
        } else {
            fd_set fdset;
            FD_ZERO(&fdset);
            FD_SET(mCameraDev, &fdset);
            struct timeval tv;
            int selectStatus = -1;
            tv.tv_sec = 2;
            tv.tv_usec = 0;
            selectStatus = select(mCameraDev + 1, &fdset, nullptr, nullptr, &tv);
            if (selectStatus > 0) {
                ret = ioctl(mCameraDev, request, output);
                if (ret < 0) {
                    CAMERA_LOG_ERROR(strerror(errno));
                    return false;
                }
            } else {
                if (selectStatus == 0) {
                    CAMERA_LOG_ERROR("Timeout!");
                } else {
                    CAMERA_LOG_ERROR(strerror(errno));
                }
                return false;
            }
        }
    }
    return true;
}

bool CameraV4l2::setformat() {
    // get default format
    struct v4l2_format fmt;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioControl(VIDIOC_G_FMT, &fmt) == false) {
        CAMERA_LOG_ERROR("Query cap fmt failed!");
        return false;
    }

    if (mFormat.imgFormat == ImgFormat::NONE) {
        switch (fmt.fmt.pix.pixelformat) {
        case V4L2_PIX_FMT_MJPEG:
            mFormat.imgFormat = ImgFormat::MJPEG;
        break;
        case V4L2_PIX_FMT_YUV420:
            mFormat.imgFormat = ImgFormat::YUV420P;
        break;
        }
    }
    if (mFormat.width <= 0 || mFormat.height <= 0) {
        mFormat.width = fmt.fmt.pix.width;
        mFormat.height = fmt.fmt.pix.height;
    }

    // set format
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = mFormat.width;
    fmt.fmt.pix.height = mFormat.height;

    switch (mFormat.imgFormat) {
    case ImgFormat::MJPEG:
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    break;
    case ImgFormat::YUV420P:
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420;
    break;
    default:
        CAMERA_LOG_WARN("No supported format!");
    break;
    } 

    if (ioControl(VIDIOC_S_FMT, &fmt) == false) {
        CAMERA_LOG_ERROR("Set format failed!");
        return false;
    }

    return true;
}

bool CameraV4l2::queryInfo() {
    struct v4l2_capability cap;
    if (ioControl(VIDIOC_QUERYCAP, &cap) == false) {
        CAMERA_LOG_ERROR("Query cap info failed!");
        return false;
    }
    char devInfo[512] = { 0 };
    sprintf(
        devInfo, 
        "Driver name: %s\n"
        "Card name: %s\n"
        "Bus info: %s\n"
        "Version: %d\n",
        cap.driver,
        cap.card,
        cap.bus_info,
        cap.version
    );
    CAMERA_LOG_INFO(devInfo);

    struct v4l2_fmtdesc fmtdesc;
    fmtdesc.index = 0;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioControl(VIDIOC_ENUM_FMT, &fmtdesc) == false) {
        CAMERA_LOG_ERROR("Query cap format failed!");
        return false;
    }
    char fmtInfo[512] = { 0 };
    sprintf(
        fmtInfo, 
        "Format: %c %c %c %c\n"
        "Format dexcription: %s\n",
        fmtdesc.pixelformat & 0xFF,
        (fmtdesc.pixelformat >> 8) & 0xFF,
        (fmtdesc.pixelformat >> 16) & 0xFF,
        (fmtdesc.pixelformat >> 24) & 0xFF,
        fmtdesc.description
    );
    CAMERA_LOG_INFO(fmtInfo);
    return true;
}

bool CameraV4l2::requestBuffers() {
    struct v4l2_requestbuffers requestbuffers;
    requestbuffers.count = 4;
    requestbuffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    requestbuffers.memory = V4L2_MEMORY_MMAP;
    if (ioControl(VIDIOC_REQBUFS, &requestbuffers) == false) {
        CAMERA_LOG_ERROR("Request buffers failed");
        return false;
    }

    if (requestbuffers.count > mMapBuffers.size() ||
        requestbuffers.count > mMapSizes.size()) {
        CAMERA_LOG_ERROR("Request buffers to large!!!");
        return false;
    } 

    for (int i = 0; i < requestbuffers.count; i++) {
        struct v4l2_buffer buffer;
        buffer.index = i;
        buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer.memory = V4L2_MEMORY_MMAP;
        if (ioControl(VIDIOC_QUERYBUF, &buffer) == false) {
            CAMERA_LOG_ERROR("Query buffer failed");
            return false;
        }
        mMapBuffers[i] = (char *)mmap(nullptr, buffer.length, PROT_READ | PROT_WRITE, MAP_SHARED, mCameraDev, buffer.m.offset);
        mMapSizes[i] = buffer.length;
        if (ioControl(VIDIOC_QBUF, &buffer) == false) {
            CAMERA_LOG_ERROR("Enquery failed");
            return false;
        }
    }

    enum v4l2_buf_type bufferType = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioControl(VIDIOC_STREAMON, &bufferType) == false) {
        CAMERA_LOG_ERROR("Open stream failed");
        return false;
    }
    return true;
}

bool CameraV4l2::getformat() {
    struct v4l2_format fmt;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioControl(VIDIOC_G_FMT, &fmt) == false) {
        CAMERA_LOG_ERROR("Query cap fmt failed!");
        return false;
    }
    char fmtInfo[512] = { 0 };
    sprintf(
        fmtInfo, 
        "width: %d\n"
        "height: %d\n",
        fmt.fmt.pix.width,
        fmt.fmt.pix.height
    );
    CAMERA_LOG_INFO(fmtInfo);

    return true;
}

bool CameraV4l2::setInput() {
    int index = 0;
    if (ioControl(VIDIOC_S_INPUT, &index) == false) {
        CAMERA_LOG_ERROR("Set cap input failed!");
        return false;
    }
    return true;
}

