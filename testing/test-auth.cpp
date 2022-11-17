// "Copyright [year] <Copyright Owner>"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "api-endpoints/api-endpoints-lib.h"
#include "main/db-service.h"
#include "authentication/auth-service.h"


TEST(AuthTest, Auth_Function_Tests) {
  AuthService auth = AuthService();

  std::string plain;
  std::string encrypted;

  plain = "";
  encrypted = auth.encryptPassword(plain);
  ASSERT_EQ(true, auth.validatePassword(plain, encrypted));
  ASSERT_EQ(false, auth.validatePassword("random", encrypted));

  plain = "password";
  encrypted = auth.encryptPassword(plain);
  ASSERT_EQ(true, auth.validatePassword(plain, encrypted));
  ASSERT_EQ(false, auth.validatePassword("random", encrypted));

  std::string email;
  std::string token;
  std::pair<bool, std::string> result;

  email = "";
  token = auth.createJWT(email);
  result = auth.decodeAndVerifyJWT(token);
  ASSERT_EQ(result.first, true);
  ASSERT_EQ(result.second, "");

  email = "fake@gmail.com";
  token = auth.createJWT(email);
  result = auth.decodeAndVerifyJWT(token);
  ASSERT_EQ(result.first, true);
  ASSERT_EQ(result.second, "fake@gmail.com");

  email = "fake@gmail.com";
  token = auth.createJWT(email, 1);
  std::this_thread::sleep_for(std::chrono::milliseconds(1001));
  result = auth.decodeAndVerifyJWT(token);
  ASSERT_EQ(result.first, false);

  result = auth.decodeAndVerifyJWT("SOME RANDOM TOKEN");
  ASSERT_EQ(result.first, false);

}

