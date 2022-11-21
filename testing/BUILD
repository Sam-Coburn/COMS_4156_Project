cc_library(
    name = "mock-libs",
    hdrs = ["MockDBService.h",
    "MockAuthService.h"],
)

""" one giant test with everything """
cc_test(
  name = "test",
  size = "small",
  srcs = ["test-db-integration.cpp", 
          "test-auth-routes.cpp",
          "test-auth.cpp",
          "test-auth-routes-integration.cpp",
          "test-matchmaking.cpp",
          "test-matchmaking-integration.cpp",
          "test-get-post-games-routes.cc"
          "test-player-endpoints.cpp"],
  deps = [
    "//main:main",
    "//main:db-service",
    "//api-endpoints:api-endpoints",
    "//authentication:auth-service",
    "@com_google_googletest//:gtest_main",
    "//testing:mock-libs"],
)

""" unit tests """
cc_test(
  name = "test-auth-routes-units",
  size = "small",
  srcs = ["test-auth-routes.cpp"],
  deps = [
    "//main:main",
    "//main:db-service",
    "//api-endpoints:api-endpoints",
    "//authentication:auth-service",
    "@com_google_googletest//:gtest_main",
    "//testing:mock-libs"],
)

cc_test(
  name = "test-matchmaking-route-units",
  size = "small",
  srcs = ["test-matchmaking.cpp"],
  deps = [
    "//main:main",
    "//main:db-service",
    "//api-endpoints:api-endpoints",
    "//authentication:auth-service",
    "@com_google_googletest//:gtest_main",
    "//testing:mock-libs"],
)

cc_test(
  name = "test-get-post-games-routes-units",
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

""" integration tests """
cc_test(
  name = "test-auth-routes-integration",
  size = "small",
  srcs = ["test-auth-routes-integration.cpp"],
  deps = [
    "//main:main",
    "//main:db-service",
    "//api-endpoints:api-endpoints",
    "//authentication:auth-service",
    "@com_google_googletest//:gtest_main",
    "//testing:mock-libs"],
)

""" end-to-end system tests??? """