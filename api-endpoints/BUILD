load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "api-endpoints",
    srcs = ["api-endpoints.cc", "api-matchmaking.cc"],
    hdrs = ["api-endpoints-lib.h", "api-matchmaking-lib.h"],
    visibility = [
        "//main:__pkg__", 
        "//api-endpoints:__pkg__", 
        "//testing:__pkg__",
    ],
    deps = [
        "@jsoncpp//:jsoncpp",
        "//crow:crow_all",
        "//main:db-service",
        "@com_github_google_glog//:glog",
    ],
)