cc_library(
    name = "mock-lib",
    hdrs = ["MockDBService.h", "MockAuthService.h"],
)

cc_test(
  name = "test",
  size = "small",
  srcs = ["test-db-integration.cpp", "test-auth-routes.cpp", "test-auth.cpp", "test-auth-routes-integration.cpp", 
    "test-matchmaking.cpp", "test-player-endpoints.cpp"],
  deps = [
    "//main:main",
    "//main:db-service",
    "//api-endpoints:api-endpoints",
    "@com_google_googletest//:gtest_main",
    "mock-lib"],
)