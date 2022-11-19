load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "bcrypt",
    srcs = ["bcrypt.cpp", "blowfish.cpp"],
    hdrs = ["node_blf.h", "openbsd.h", "bcrypt.h"],
    visibility = ["//visibility:public"],
)
