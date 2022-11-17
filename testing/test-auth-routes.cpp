// "Copyright [year] <Copyright Owner>"

#include "api-endpoints/api-endpoints-lib.h"
#include "MockDBService.h"

using ::testing::_;
using ::testing::Return;

TEST(AuthRouteTestFixture, Post_SignUp_Tests) {
  MockDBService DB;
  APIEndPoints api = APIEndPoints(&DB);

  Developer valid_developer;
  valid_developer.developer_email = "some_email@gmail.com";
  valid_developer.developer_password = "some_password";
  valid_developer.is_valid = true;

  Developer invalid_developer;
  invalid_developer.is_valid = false;

  EXPECT_CALL(DB, add_developer(_)).Times(2)
  .WillOnce(Return(valid_developer))
  .WillOnce(Return(invalid_developer));
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
  ASSERT_EQ(res.body, "Succesfully signed up");

  // Invalid Body (developer already exists)
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postSignUp(req);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Developer already exists");
}


TEST(AuthRouteTestFixture, Post_Login_Tests) {
  MockDBService DB;
  APIEndPoints api = APIEndPoints(&DB);
  Developer valid_developer;
  valid_developer.is_valid = true;
  valid_developer.developer_email = "some_email@gmail.com";
  valid_developer.developer_password = "some_password";

  Developer invalid_developer;
  invalid_developer.is_valid = false;

  EXPECT_CALL(DB, add_developer(_)).Times(1)
  .WillOnce(Return(valid_developer));

  EXPECT_CALL(DB, get_developer(_)).Times(2)
  .WillOnce(Return(invalid_developer))
  .WillOnce(Return(valid_developer));

  crow::request req;
  crow::response res;
  crow::json::wvalue body;

  // Valid SignUp
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postSignUp(req);
  ASSERT_EQ(res.code, 200);
  ASSERT_EQ(res.body, "Succesfully signed up");

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

  // Invalid Login (Invalid Credentials)
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "wrong_password"}};
  req.body = body.dump();
  res = api.postLogin(req);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid credentials");

  // Valid Login
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postLogin(req);
  ASSERT_EQ(res.code, 200);
  ASSERT_EQ(res.body, "Succesfully logged in");
}


TEST(AuthRouteTestFixture, Delete_Login_Tests) {
  MockDBService DB;
  APIEndPoints api = APIEndPoints(&DB);
  Developer valid_developer;
  valid_developer.is_valid = true;
  valid_developer.developer_email = "some_email@gmail.com";
  valid_developer.developer_password = "some_password";

  Developer invalid_developer;
  invalid_developer.is_valid = false;

  EXPECT_CALL(DB, add_developer(_)).Times(1)
  .WillOnce(Return(valid_developer));

  EXPECT_CALL(DB, get_developer(_)).Times(4)
  .WillOnce(Return(valid_developer))
  .WillOnce(Return(invalid_developer))
  .WillOnce(Return(valid_developer))
  .WillOnce(Return(valid_developer));

  EXPECT_CALL(DB, remove_developer(_)).Times(1)
  .WillOnce(Return(valid_developer));

  crow::request req;
  crow::response res;
  crow::json::wvalue body;

  // Valid SignUp
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postSignUp(req);
  ASSERT_EQ(res.code, 200);
  ASSERT_EQ(res.body, "Succesfully signed up");

  // Valid Login
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postLogin(req);
  ASSERT_EQ(res.code, 200);
  ASSERT_EQ(res.body, "Succesfully logged in");

  // Empty Body
  body = {};
  req.body = body.dump();
  res = api.deleteLogin(req);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Invalid body (missing both parameters)
  body = {{"some_random_parameter", "something"}};
  req.body = body.dump();
  res = api.deleteLogin(req);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Invalid body (missing developer_password)
  body = {{"developer_email", "some_email@gmail.com"}};
  req.body = body.dump();
  res = api.deleteLogin(req);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Invalid body (missing developer_email)
  body = {{"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.deleteLogin(req);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid request body");

  // Invalid Delete (Invalid Email)
  body = {{"developer_email", "doesnt_exist@gmail.com"}, {"developer_password", "wrong_password"}};
  req.body = body.dump();
  res = api.deleteLogin(req);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "User not found");

  // Invalid Delete (Invalid Credentials)
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "wrong_password"}};
  req.body = body.dump();
  res = api.deleteLogin(req);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Invalid credentials");

  // Valid Delete
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.deleteLogin(req);
  ASSERT_EQ(res.code, 200);
  ASSERT_EQ(res.body, "Succesfully deleted account");
}
