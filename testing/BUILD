cc_library(
    name = "mock-lib",
    hdrs = ["MockDBService.h"],
)

cc_test(
  name = "test",
  size = "small",
  srcs = ["test-auth-routes.cpp", "test-player-endpoints.cpp"],
  deps = [
    "//main:main",
    "//main:db-service",
    "//api-endpoints:api-endpoints",
    "@com_google_googletest//:gtest_main",
    "mock-lib"],
)