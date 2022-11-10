// "Copyright [year] <Copyright Owner>"

#include <gtest/gtest.h>
#include "api-endpoints/api-endpoints-lib.h"
#include "main/db-service.h"

class AuthRouteTestFixture: public testing::Test
{
  public:
    static DBService DB;
    static APIEndPoints api;

    virtual void SetUp() {
      DB.tearDownDatabase();
      DB.setUpDatabase();
    }

    virtual void TearDown() {
      DB.tearDownDatabase();
    }

    static void SetUpTestCase() {
      DB = DBService("tcp://127.0.0.1:3306", "dbuser", "123", "test_matchmaking_api_db");
      api = APIEndPoints();
    }

    static void TearDownTestCase() {
      DB.tearDownDatabase();
    }
};

DBService AuthRouteTestFixture::DB;
APIEndPoints AuthRouteTestFixture::api;


TEST_F(AuthRouteTestFixture, Post_SignUp_Tests) {
  crow::request req;
  crow::response res;
  crow::json::wvalue body;

  // Empty Body
  body = {};
  req.body = body.dump();
  res = api.postSignUp(req, DB);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Invalid body (missing both parameters)
  body = {{"some_random_parameter", "something"}};
  req.body = body.dump();
  res = api.postSignUp(req, DB);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Invalid body (missing developer_password)
  body = {{"developer_email", "some_email@gmail.com"}};
  req.body = body.dump();
  res = api.postSignUp(req, DB);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Invalid body (missing developer_email)
  body = {{"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postSignUp(req, DB);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Valid Body
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postSignUp(req, DB);
  ASSERT_EQ(res.code, 200);
  ASSERT_EQ(res.body, "Succesfully signed up");

  // Invalid Body (developer already exists)
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}} ;
  req.body = body.dump();
  res = api.postSignUp(req, DB);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Developer already exists");
}



TEST_F(AuthRouteTestFixture, Post_Login_Tests) {
  crow::request req;
  crow::response res;
  crow::json::wvalue body;

  // Valid SignUp
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postSignUp(req, DB);
  ASSERT_EQ(res.code, 200);
  ASSERT_EQ(res.body, "Succesfully signed up");
  Developer D;
  D = DB.get_developer("some_email@gmail.com");

  // Empty Body
  body = {};
  req.body = body.dump();
  res = api.postLogin(req, DB);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Invalid body (missing both parameters)
  body = {{"some_random_parameter", "something"}};
  req.body = body.dump();
  res = api.postLogin(req, DB);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Invalid body (missing developer_password)
  body = {{"developer_email", "some_email@gmail.com"}};
  req.body = body.dump();
  res = api.postLogin(req, DB);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Invalid body (missing developer_email)
  body = {{"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postLogin(req, DB);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Invalid Login (Invalid Credentials)
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "wrong_password"}};
  req.body = body.dump();
  res = api.postLogin(req, DB);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid credentials");

  // Valid Login
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postLogin(req, DB);
  ASSERT_EQ(res.code, 200);
  ASSERT_EQ(res.body, "Succesfully logged in");
}


TEST_F(AuthRouteTestFixture, Delete_Login_Tests) {
  crow::request req;
  crow::response res;
  crow::json::wvalue body;

  // Valid SignUp
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postSignUp(req, DB);
  ASSERT_EQ(res.code, 200);
  ASSERT_EQ(res.body, "Succesfully signed up");

  // Valid Login
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postLogin(req, DB);
  ASSERT_EQ(res.code, 200);
  ASSERT_EQ(res.body, "Succesfully logged in");

  // Empty Body
  body = {};
  req.body = body.dump();
  res = api.deleteLogin(req, DB);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Invalid body (missing both parameters)
  body = {{"some_random_parameter", "something"}};
  req.body = body.dump();
  res = api.deleteLogin(req, DB);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Invalid body (missing developer_password)
  body = {{"developer_email", "some_email@gmail.com"}};
  req.body = body.dump();
  res = api.deleteLogin(req, DB);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Invalid body (missing developer_email)
  body = {{"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.deleteLogin(req, DB);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");
  
  // Invalid Delete (Invalid Email)
  body = {{"developer_email", "doesnt_exist@gmail.com"}, {"developer_password", "wrong_password"}};
  req.body = body.dump();
  res = api.deleteLogin(req, DB);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "User not found");

  // Invalid Delete (Invalid Credentials)
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "wrong_password"}};
  req.body = body.dump();
  res = api.deleteLogin(req, DB);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid credentials");

  // Valid Delete
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.deleteLogin(req, DB);
  ASSERT_EQ(res.code, 200);
  ASSERT_EQ(res.body, "Succesfully deleted account");

}