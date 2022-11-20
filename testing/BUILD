cc_library(
    name = "mock-libs",
    hdrs = ["MockDBService.h",
    "MockAuthService.h"],
)

cc_test(
  name = "test",
  size = "small",
  srcs = ["test-db-integration.cpp", 
          "test-auth-routes.cpp",
          "test-auth.cpp",
          "test-auth-routes-integration.cpp",
          "test-matchmaking.cpp",
          "test-get-post-games-routes.cc"],
  deps = [
    "//main:main",
    "//main:db-service",
    "//api-endpoints:api-endpoints",
    "//authentication:auth-service",
    "@com_google_googletest//:gtest_main",
    "//testing:mock-libs"],
)

cc_test(
  name = "test-get-post-games",
  size = "small",
  srcs = ["test-get-post-games-routes.cc"],
  deps = [
    "//main:main",
    "//main:db-service",
    "//api-endpoints:api-endpoints",
    "//authentication:auth-service",
    "@com_google_googletest//:gtest_main",
    "//testing:mock-libs"],
)