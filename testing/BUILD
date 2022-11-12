cc_test(
  name = "test",
  size = "small",
  srcs = ["test-db.cpp","test-auth-routes.cpp"],
  deps = [
    "//main:main",
    "//main:db-service",
    "//api-endpoints:api-endpoints",
    "@com_google_googletest//:gtest_main"],
)