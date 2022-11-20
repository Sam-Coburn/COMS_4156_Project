#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "main/db-service.h"
#include "api-endpoints/api-endpoints-lib.h"

using ::testing::_;
using ::testing::Return;


class MatchmakingTestFixture: public testing::Test
{
  public:
    static DBService DB;
    static AuthService auth;
    static Matchmaking M;

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
      M = Matchmaking();
    }

    static void TearDownTestCase() {
      DB.tearDownDatabase();
    }
};

DBService MatchmakingTestFixture::DB;
AuthService MatchmakingTestFixture::auth;
Matchmaking MatchmakingTestFixture::M;

TEST_F(MatchmakingTestFixture,  Matchmaking_Endpoint_Tests_Set1) {
    APIEndPoints api = APIEndPoints(&DB, &auth);

    crow::request req;
    crow::response res;
    crow::json::wvalue body;
    std::string token;

    Developer valid_developer;
    valid_developer.developer_email = "some_email@gmail.com";
    valid_developer.developer_password = "some_password";
    valid_developer.is_valid = true;

    std::vector<Game_Details> games;

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

    // Test: Empty Request Body
    body = {};
    req.body = body.dump();
    req.add_header("Authorization", token);
    res =  api.matchmake(req, &M);
    ASSERT_EQ(res.code, 400);

    // Test: No Game ID Given
    body = {
        {"matchmaking_type", "basic"},
        {"player_emails", "[\"player1@gmail.com\"]"}
    };
    req.body = body.dump();
    req.add_header("Authorization", token);
    res =  api.matchmake(req, &M);
    ASSERT_EQ(res.code, 400);
    ASSERT_EQ(res.body, "Incorrect Request Format.\n");

    // Test: No Player Emails Given
    body = {
        {"game_id", 1}
    };
    req.body = body.dump();
    req.add_header("Authorization", token);
    res =  api.matchmake(req, &M);
    ASSERT_EQ(res.code, 400);
}
