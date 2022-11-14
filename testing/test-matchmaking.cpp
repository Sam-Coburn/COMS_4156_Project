#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "main/db-service.h"
#include "api-endpoints/api-endpoints-lib.h"
#include "api-endpoints/api-matchmaking-lib.h"

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

class MockMatchmaking: public Matchmaking {
 public:
  MOCK_METHOD(td::tuple<
        std::vector<std::vector<std::vector<std::string> > >,
        std::vector<std::string> >, matchmakingBackend, (int game_id, std::vector<std::string> player_emails), (override));
};

TEST(MatchmakingTestFixture,  Matchmaking_Endpoint_Tests) {
    APIEndpoints api = APIEndPoints();

    MockDBService DB;
    Developer d;
    d.developer_email = "developer_email@gmail.com";
    d.developer_password = "correct_password";

    EXPECT_CALL(DB, get_developer())
    .WillOnce(Return(d));

    crow::request req;
    crow::response res;
    crow::json::wvalue body;

    // Incorrect Password Given
    body = {
        {"developer_email", "developer_email@gmail.com"},
        {"developer_password", "correct_password"},
        {"game_id", "1"},
        {"player_emails", "[\"player1@gmail.com\"]"}
    };
    req.body = body.dump();
    res =  api.matchmake();
    ASSERT_EQ(res.code, 200);
    ASSERT_EQ(res.body, "Incorrect Credentials Given.\n");
}

/*
TEST(MatchmakingTestFixture, Matchmaking_Backend_Tests) {

}
*/
