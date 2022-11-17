cc_library(
    name = "mock-lib",
    hdrs = ["MockDBService.h"],
)

cc_test(
  name = "test",
  size = "small",
  srcs = ["test-db.cpp",
          "test-auth-routes.cpp",
          "test-get-post-games-routes.cc",],
  deps = [
    "//main:main",
    "//main:db-service",
    "//api-endpoints:api-endpoints",
    "@com_google_googletest//:gtest_main",
    "mock-lib"],
)