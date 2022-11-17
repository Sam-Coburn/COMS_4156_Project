load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "db-service",
    srcs = ["db-service.cpp"],
    hdrs = ["db-service.h"],
    copts = ["-I/usr/include/mysql-cppconn-8/jdbc/"],
    linkopts = ["-lpthread", "-lmysqlcppconn"],
    visibility = [
        "//main:__pkg__", 
        "//api-endpoints:__pkg__", 
        "//testing:__pkg__",
    ],
)

cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = [
        "//crow:crow_all",
        ":db-service",
        "//api-endpoints:api-endpoints",
        "@com_github_google_glog//:glog",
    ],
    copts = ["-I/usr/include/mysql-cppconn-8/jdbc/"],
    linkopts = ["-lpthread", "-lmysqlcppconn", "-lboost_system"],
    visibility = ["//testing:__pkg__"],
)
