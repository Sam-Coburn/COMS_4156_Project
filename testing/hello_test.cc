#include <gtest/gtest.h>
#include "hello-world-pkg/hello-greet.h"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
  // Expect get_greet to return what we passed in
  EXPECT_STREQ(get_greet("earth").c_str(), "Hello earth");
}