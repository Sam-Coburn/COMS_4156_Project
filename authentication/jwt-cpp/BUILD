load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(name = "jwt-cpp", 
    includes = ["include"],
    deps = ["jwt-cpp-internal"],
    linkopts = ["-lcrypto"],
    visibility = ["//visibility:public"],
)
cc_import(name = "jwt-cpp-internal",
    hdrs = glob(["include/**/*.h"]),
    visibility = ["//visibility:public"],
)