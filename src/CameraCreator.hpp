#ifndef CAMERA_CREATOR_H
#define CAMERA_CREATOR_H

#include "CameraV4l2.h"

class CameraCreator {
public:
    static std::shared_ptr<Camera> create() {
        return std::make_shared<CameraV4l2>();
    }
};

#endif
