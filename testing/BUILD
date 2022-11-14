cc_test(
  name = "test",
  size = "small",
  srcs = ["test-db.cpp","test-auth-routes.cpp", "test-matchmaking.cpp"],
  deps = [
    "//main:main",
    "//main:db-service",
    "//api-endpoints:api-endpoints",
    "//api-endpoints:api-matchmaking",
    "@com_google_googletest//:gtest_main"],
)