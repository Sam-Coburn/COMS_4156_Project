load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "auth-service",
    srcs = ["auth-service.cpp"],
    hdrs = ["auth-service.h"],
    visibility = [
        "//main:__pkg__", 
        "//api-endpoints:__pkg__", 
        "//testing:__pkg__",
    ],
    deps = [
        "@com_github_google_glog//:glog",
        "//authentication/bcrypt:bcrypt",
        "//authentication/jwt-cpp:jwt-cpp",
    ],
)
