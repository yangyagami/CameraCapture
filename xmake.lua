add_rules("mode.release", "mode.debug", "mode.check")

if is_mode("debug") or is_mode("check") then
    add_defines("_DEBUG")
end

add_cxflags("-Werror")

target("CameraCapture")
    set_kind("shared")

    add_files("src/backends/v4l2/*.cpp")
    add_includedirs("src/backends/v4l2", { public = true } )
    add_includedirs("/usr/include/opencv4", { public = true } )
    add_includedirs("src", { public = true } )

    add_links(
        "opencv_world",
        { public = true }
    )

target("CameraCaptureTest")
    set_kind("binary")
    add_deps("CameraCapture")

    add_files("test/src/*.cpp")
