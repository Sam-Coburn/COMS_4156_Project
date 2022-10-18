load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "api-endpoints",
    srcs = ["api-endpoints.cc"],
    hdrs = ["api-endpoints-lib.h"],
    visibility = [
        "//main:__pkg__", 
        "//api-endpoints:__pkg__", 
        "//testing:__pkg__",
    ],
    deps = [
        "@jsoncpp//:jsoncpp",
        "//crow:crow_all",
    ],
)

cc_library(
    name = "emily",
    srcs = ["emily.cc"],
    hdrs = ["emily.h"],
    visibility = [
        "//main:__pkg__", 
        "//api-endpoints:__pkg__", 
        "//testing:__pkg__"
    ],
    deps = [
        "@jsoncpp//:jsoncpp",
    ],
)

cc_binary(
    name = "emily-test-main",
    srcs = ["emily-test-main.cc"],
    deps = [
        ":emily",
        ":api-endpoints"
    ],
)
