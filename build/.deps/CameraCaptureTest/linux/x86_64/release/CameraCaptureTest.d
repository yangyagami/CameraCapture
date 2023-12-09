{
    files = {
        "build/.objs/CameraCaptureTest/linux/x86_64/release/test/src/main.cpp.o"
    },
    values = {
        "/usr/bin/g++",
        {
            "-m64",
            "-Lbuild/linux/x86_64/release",
            "-Wl,-rpath=$ORIGIN",
            "-lCameraCapture"
        }
    }
}