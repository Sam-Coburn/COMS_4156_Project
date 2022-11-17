cc_test(
  name = "test",
  size = "small",
  srcs = ["test-db-integration.cpp", "test-auth-routes.cpp", "test-auth.cpp", "test-auth-routes-integration.cpp"],
  deps = [
    "//main:main",
    "//main:db-service",
    "//api-endpoints:api-endpoints",
    "@com_google_googletest//:gtest_main"],
)