target("CameraCapture")
    set_kind("shared")

    add_files("src/backends/v4l2/*.cpp")
    add_includedirs("src/backends/v4l2", { public = true } )
    add_includedirs("/usr/include/opencv4", { public = true } )
    add_includedirs("src", { public = true } )

target("CameraCaptureTest")
    set_kind("binary")
    add_deps("CameraCapture")

    add_files("test/src/*.cpp")
