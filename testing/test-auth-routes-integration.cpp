// "Copyright [year] <Copyright Owner>"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "api-endpoints/api-endpoints-lib.h"
#include "main/db-service.h"
#include "authentication/auth-service.h"

using ::testing::_;
using ::testing::Return;

class AuthRouteTestFixture: public testing::Test
{
  public:
    static DBService DB;
    static AuthService auth;

    virtual void SetUp() {
      DB.tearDownDatabase();
      DB.setUpDatabase();
    }

    virtual void TearDown() {
      DB.tearDownDatabase();
    }

    static void SetUpTestCase() {
      DB = DBService("tcp://127.0.0.1:3306", "dbuser", "123", "test_matchmaking_api_db");
      auth = AuthService();
    }

    static void TearDownTestCase() {
      DB.tearDownDatabase();
    }
};

DBService AuthRouteTestFixture::DB;
AuthService AuthRouteTestFixture::auth;

TEST_F(AuthRouteTestFixture, Authenticate_Token_Test) {
  APIEndPoints api = APIEndPoints(&DB, &auth);

  crow::request req;
  crow::response res;
  crow::json::wvalue body;
  std::pair<bool, std::string> result;

  Developer valid_developer;
  valid_developer.developer_email = "some_email@gmail.com";
  valid_developer.developer_password = "some_password";
  valid_developer.is_valid = true;

  Developer invalid_developer;
  invalid_developer.is_valid = false;

  std::string token;

  // Valid SignUp
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postSignUp(req);
  ASSERT_EQ(res.code, 200);

  // Valid Login
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postLogin(req);
  ASSERT_EQ(res.code, 200);
  token = res.body.substr(res.body.find(":") + 2);

  // No Authorization Header
  result = api.authenticateToken(req);
  ASSERT_EQ(result.first, false);
  ASSERT_EQ(result.second, "Invalid Header");

  // Invalid token
  req.add_header("Authorization", "Random String");
  result = api.authenticateToken(req);
  ASSERT_EQ(result.first, false);

  // Expired token
  req.add_header("Authorization", "Expired Token");
  result = api.authenticateToken(req);
  ASSERT_EQ(result.first, false);
  
  // Valid token
  req.add_header("Authorization", token);
  result = api.authenticateToken(req);
  ASSERT_EQ(result.first, true);
}

TEST_F(AuthRouteTestFixture, Post_SignUp_Tests) {
  APIEndPoints api = APIEndPoints(&DB, &auth);

  Developer valid_developer;
  valid_developer.developer_email = "some_email@gmail.com";
  valid_developer.developer_password = "some_password";
  valid_developer.is_valid = true;

  Developer invalid_developer;
  invalid_developer.is_valid = false;

  crow::request req;
  crow::response res;
  crow::json::wvalue body;

  // Empty Body
  body = {};
  req.body = body.dump();
  res = api.postSignUp(req);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Invalid body (missing both parameters)
  body = {{"some_random_parameter", "something"}};
  req.body = body.dump();
  res = api.postSignUp(req);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Invalid body (missing developer_password)
  body = {{"developer_email", "some_email@gmail.com"}};
  req.body = body.dump();
  res = api.postSignUp(req);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Invalid body (missing developer_email)
  body = {{"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postSignUp(req);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Valid Body
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postSignUp(req);
  ASSERT_EQ(res.code, 200);

  // Invalid Body (developer already exists)
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}} ;
  req.body = body.dump();
  res = api.postSignUp(req);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Developer already exists");
}


TEST_F(AuthRouteTestFixture, Post_Login_Tests) {
  APIEndPoints api = APIEndPoints(&DB, &auth);

  Developer valid_developer;
  valid_developer.is_valid = true;
  valid_developer.developer_email = "some_email@gmail.com";
  valid_developer.developer_password = "some_password";

  Developer invalid_developer;
  invalid_developer.is_valid = false;

  crow::request req;
  crow::response res;
  crow::json::wvalue body;

  // Valid SignUp
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postSignUp(req);
  ASSERT_EQ(res.code, 200);

  // Empty Body
  body = {};
  req.body = body.dump();
  res = api.postLogin(req);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Invalid body (missing both parameters)
  body = {{"some_random_parameter", "something"}};
  req.body = body.dump();
  res = api.postLogin(req);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Invalid body (missing developer_password)
  body = {{"developer_email", "some_email@gmail.com"}};
  req.body = body.dump();
  res = api.postLogin(req);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Invalid body (missing developer_email)
  body = {{"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postLogin(req);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Invalid Login (Invalid Developer)
  body = {{"developer_email", "fake_email@gmail.com"}, {"developer_password", "wrong_password"}};
  req.body = body.dump();
  res = api.postLogin(req);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Developer does not exist");

  // Invalid Login (Invalid Credentials)
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "wrong_password"}};
  req.body = body.dump();
  res = api.postLogin(req);
  ASSERT_EQ(res.code, 401);
  ASSERT_EQ(res.body, "Invalid credentials");

  // Valid Login
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postLogin(req);
  ASSERT_EQ(res.code, 200);
}


TEST_F(AuthRouteTestFixture, Delete_Login_Tests) {
  APIEndPoints api = APIEndPoints(&DB, &auth);

  Developer valid_developer;
  valid_developer.is_valid = true;
  valid_developer.developer_email = "some_email@gmail.com";
  valid_developer.developer_password = "some_password";

  Developer invalid_developer;
  invalid_developer.is_valid = false;

  crow::request req;
  crow::response res;
  crow::json::wvalue body;
  std::string token;

  // Valid SignUp
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postSignUp(req);
  ASSERT_EQ(res.code, 200);

  // Valid Login
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postLogin(req);
  ASSERT_EQ(res.code, 200);
  token = res.body.substr(res.body.find(":") + 2);

  // No Authorization Header
  res = api.deleteLogin(req);
  ASSERT_EQ(res.code, 401);
  ASSERT_EQ(res.body, "Invalid Header");

  // Invalid token
  req.add_header("Authorization", "Random String");
  res = api.deleteLogin(req);
  ASSERT_EQ(res.code, 401);

  // Invalid token
  req.add_header("Authorization", "Expired Token");
  res = api.deleteLogin(req);
  ASSERT_EQ(res.code, 401);
  
  // Valid Delete
  req.add_header("Authorization", token);
  res = api.deleteLogin(req);
  ASSERT_EQ(res.code, 200);
}