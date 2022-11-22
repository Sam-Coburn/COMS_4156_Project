// "Copyright [year] <Copyright Owner>"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "api-endpoints/api-endpoints-lib.h"
#include "main/db-service.h"
#include "authentication/auth-service.h"

using ::testing::_;
using ::testing::Return;

class MockAuthService : public AuthService {
 public:
  MOCK_METHOD(std::string, encryptPassword, (std::string password), (override));
  MOCK_METHOD(bool, validatePassword, (std::string password, std::string hash), (override));
  MOCK_METHOD(std::string, createJWT, (std::string password, int seconds), (override));
  MOCK_METHOD((std::pair<bool, std::string>), decodeAndVerifyJWT, (std::string token), (override));
};

class MockDBService : public DBService {
 public:
  MOCK_METHOD(Player, get_player, (std::string player_email), (override));
  MOCK_METHOD(Developer, get_developer, (std::string developer_email), (override));
  MOCK_METHOD(Game_Details, get_game_details, (int game_id), (override));
  MOCK_METHOD(Player_Game_Ratings, get_player_game_rating, (std::string player_email, int game_id), (override));
  MOCK_METHOD(Joined_Player_Game_Ratings, get_joined_player_game_rating,
  (std::string player_email, int game_id), (override));

  MOCK_METHOD(std::vector<Player>, get_all_players, (), (override));
  MOCK_METHOD(std::vector<Developer>, get_all_developers, (), (override));
  MOCK_METHOD(std::vector<Game_Details>, get_all_games, (), (override));
  MOCK_METHOD(std::vector<Joined_Player_Game_Ratings>, get_all_player_game_ratings_for_game,
  (int game_id), (override));
  MOCK_METHOD(std::vector<Game_Details>, get_all_games_for_developer,
  (std::string developer_email), (override));

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
  MOCK_METHOD(Player_Game_Ratings, update_player_rating,
  (std::string player_email, int game_id, Player_Game_Ratings PGR), (override));
};

TEST(AuthRouteTest, Authenticate_Token_Test) {
  MockDBService DB;
  MockAuthService auth;
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

  EXPECT_CALL(DB, add_developer(_)).Times(1)
  .WillOnce(Return(valid_developer));

  EXPECT_CALL(auth, encryptPassword(_)).Times(1);

  EXPECT_CALL(auth, validatePassword(_, _)).Times(1)
  .WillOnce(Return(true));

  EXPECT_CALL(auth, createJWT(_, _)).Times(1);

  EXPECT_CALL(auth, decodeAndVerifyJWT(_)).Times(3)
  .WillOnce(Return(std::make_pair(false, "Invalid token")))
  .WillOnce(Return(std::make_pair(false, "Expired token")))
  .WillOnce(Return(std::make_pair(true, "some_email@gmail.com")));

  EXPECT_CALL(DB, get_developer(_)).Times(2)
  .WillOnce(Return(valid_developer))
  .WillOnce(Return(valid_developer));

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
  req.add_header("Authorization", "Valid Token");
  result = api.authenticateToken(req);
  ASSERT_EQ(result.first, true);
}

TEST(AuthRouteTest, Developer_Owns_Game_Tests) {
  MockDBService DB;
  MockAuthService auth;
  APIEndPoints api = APIEndPoints(&DB, &auth);
  bool res;

  Game_Details game1;
  Game_Details game2;
  Game_Details game_invalid;

  game1.developer_email = "dev1@gmail.com";
  game1.game_id = 1;
  game1.is_valid = true;
  game2.developer_email = "dev2@gmail.com";
  game2.game_id = 2;
  game2.is_valid = true;
  game_invalid.developer_email = "dev3@gmail.com";
  game_invalid.game_id = 3;
  game_invalid.is_valid = false;

  // Invalid Game_Details object
  EXPECT_CALL(DB, get_game_details(_)).Times(1)
  .WillOnce(Return(game_invalid));
  res = api.developerOwnsGame(game_invalid.developer_email, game_invalid.game_id);
  EXPECT_EQ(res, false);

  // Developer does not own game
  EXPECT_CALL(DB, get_game_details(_)).Times(1)
  .WillOnce(Return(game2));
  res = api.developerOwnsGame(game1.developer_email, game2.game_id);
  EXPECT_EQ(res, false);

  // Developer owns game
  EXPECT_CALL(DB, get_game_details(_)).Times(1)
  .WillOnce(Return(game1));
  res = api.developerOwnsGame(game1.developer_email, game1.game_id);
  EXPECT_EQ(res, true);
}

TEST(AuthRouteTest, Post_SignUp_Tests) {
  MockDBService DB;
  MockAuthService auth;
  APIEndPoints api = APIEndPoints(&DB, &auth);

  Developer valid_developer;
  valid_developer.developer_email = "some_email@gmail.com";
  valid_developer.developer_password = "some_password";
  valid_developer.is_valid = true;

  Developer invalid_developer;
  invalid_developer.is_valid = false;

  EXPECT_CALL(DB, add_developer(_)).Times(2)
  .WillOnce(Return(valid_developer))
  .WillOnce(Return(invalid_developer));

  EXPECT_CALL(auth, encryptPassword(_)).Times(2);

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
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postSignUp(req);
  ASSERT_EQ(res.code, 400);
  ASSERT_EQ(res.body, "Developer already exists");
}


TEST(AuthRouteTest, Post_Login_Tests) {
  MockDBService DB;
  MockAuthService auth;
  APIEndPoints api = APIEndPoints(&DB, &auth);

  Developer valid_developer;
  valid_developer.is_valid = true;
  valid_developer.developer_email = "some_email@gmail.com";
  valid_developer.developer_password = "some_password";

  Developer invalid_developer;
  invalid_developer.is_valid = false;

  EXPECT_CALL(DB, add_developer(_)).Times(1)
  .WillOnce(Return(valid_developer));

  EXPECT_CALL(auth, encryptPassword(_)).Times(1);

  EXPECT_CALL(auth, validatePassword(_, _)).Times(2)
  .WillOnce(Return(false))
  .WillOnce(Return(true));

  EXPECT_CALL(auth, createJWT(_, _)).Times(1);

  EXPECT_CALL(DB, get_developer(_)).Times(3)
  .WillOnce(Return(invalid_developer))
  .WillOnce(Return(valid_developer))
  .WillOnce(Return(valid_developer));

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


TEST(AuthRouteTest, Delete_Login_Tests) {
  MockDBService DB;
  MockAuthService auth;
  APIEndPoints api = APIEndPoints(&DB, &auth);

  Developer valid_developer;
  valid_developer.is_valid = true;
  valid_developer.developer_email = "some_email@gmail.com";
  valid_developer.developer_password = "some_password";

  Developer invalid_developer;
  invalid_developer.is_valid = false;

  EXPECT_CALL(DB, add_developer(_)).Times(1)
  .WillOnce(Return(valid_developer));

  EXPECT_CALL(auth, encryptPassword(_)).Times(1);

  EXPECT_CALL(auth, validatePassword(_, _)).Times(1)
  .WillOnce(Return(true));

  EXPECT_CALL(auth, decodeAndVerifyJWT(_)).Times(3)
  .WillOnce(Return(std::make_pair(false, "Invalid token")))
  .WillOnce(Return(std::make_pair(false, "Expired token")))
  .WillOnce(Return(std::make_pair(true, "some_email@gmail.com")));

  EXPECT_CALL(auth, createJWT(_, _)).Times(1);

  EXPECT_CALL(DB, get_developer(_)).Times(2)
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

  // Valid Login
  body = {{"developer_email", "some_email@gmail.com"}, {"developer_password", "some_password"}};
  req.body = body.dump();
  res = api.postLogin(req);
  ASSERT_EQ(res.code, 200);

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
  req.add_header("Authorization", "Valid Token");
  res = api.deleteLogin(req);
  ASSERT_EQ(res.code, 200);
}
