load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = [
        "//crow:crow_all",
    ],
	copts = ["-I/usr/include/mysql-cppconn-8/jdbc/"],
    linkopts = ["-lpthread", "-lmysqlcppconn"],
)
