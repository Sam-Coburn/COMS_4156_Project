// "Copyright [year] <Copyright Owner>"

#ifndef TESTING_MOCKAPIENDPOINTS_H_
#define TESTING_MOCKAPIENDPOINTS_H_

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <string>
#include "api-endpoints/api-endpoints-lib.h"

// I just realized to be able to mock authentication and use those mocked methods
// in testing of APIEndPoints methods, authentication would need to be in a different class.

class MockAPIEndPoints : public APIEndPoints {
 public:
//   MOCK_METHOD(crow::response, postSignUp, (const crow::request& req), (override));
//   MOCK_METHOD(crow::response, postLogin, (const crow::request& req), (override));
//   MOCK_METHOD(crow::response, deleteLogin, (const crow::request& req), (override));
};

#endif  // TESTING_MOCKAPIENDPOINTS_H_

