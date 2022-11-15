// "Copyright [year] <Copyright Owner>"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "api-endpoints/api-endpoints-lib.h"
#include "main/db-service.h"

using ::testing::_;
using ::testing::Return;

class MockDBService : public DBService {
 public:
  MOCK_METHOD(Player, get_player, (std::string player_email), (override));
  MOCK_METHOD(Developer, get_developer, (std::string developer_email), (override));
  MOCK_METHOD(Game_Details, get_game_details, (int game_id), (override));
  MOCK_METHOD(Player_Game_Ratings, get_player_game_rating, (std::string player_email, int game_id), (override));
  MOCK_METHOD(Joined_Player_Game_Ratings, get_joined_player_game_rating, (std::string player_email, int game_id), (override));

  MOCK_METHOD(std::vector<Player>, get_all_players, (), (override));
  MOCK_METHOD(std::vector<Developer>, get_all_developers, (), (override));
  MOCK_METHOD(std::vector<Game_Details>, get_all_games, (), (override));
  MOCK_METHOD(std::vector<Joined_Player_Game_Ratings>, get_all_player_game_ratings_for_game, (int game_id), (override));
  MOCK_METHOD(std::vector<Game_Details>, get_all_games_for_developer, (std::string developer_email), (override));

  MOCK_METHOD(Player, add_player, (Player P), (override));
  MOCK_METHOD(Developer, add_developer, (Developer D), (override));
  MOCK_METHOD(Game_Details, add_game_details, (Game_Details GD), (override));
  MOCK_METHOD(Player_Game_Ratings, add_player_rating, (Player_Game_Ratings PGR), (override));

  MOCK_METHOD(Player, remove_player, (std::string player_email), (override));
  MOCK_METHOD(Developer, remove_developer, (std::string developer_email), (override));
  MOCK_METHOD(Game_Details, remove_game_details, (int game_id), (override));
  MOCK_METHOD(Player_Game_Ratings, remove_player_rating, (std::string player_email, int game_id), (override));

  MOCK_METHOD(Player, update_player, (std::string player_email, Player P), (override));
  MOCK_METHOD(Developer, update_developer, (std::string developer_email, Developer D), (override));
  MOCK_METHOD(Game_Details, update_game_details, (int game_id, Game_Details GD), (override));
  MOCK_METHOD(Player_Game_Ratings, update_player_rating, (std::string player_email, int game_id, Player_Game_Ratings PGR), (override));
};

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
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}} ;
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