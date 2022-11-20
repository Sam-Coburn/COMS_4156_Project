// "Copyright [year] <Copyright Owner>"

#ifndef TESTING_MOCKAUTHSERVICE_H_
#define TESTING_MOCKAUTHSERVICE_H_

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <utility>
#include <string>
#include "authentication/auth-service.h"

class MockAuthService : public AuthService {
 public:
  MOCK_METHOD(std::string, encryptPassword, (std::string password), (override));
  MOCK_METHOD(bool, validatePassword, (std::string password, std::string hash), (override));
  MOCK_METHOD(std::string, createJWT, (std::string password, int seconds), (override));
  MOCK_METHOD((std::pair<bool, std::string>), decodeAndVerifyJWT, (std::string token), (override));
};

#endif  // TESTING_MOCKAUTHSERVICE_H_