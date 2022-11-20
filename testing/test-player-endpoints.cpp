// "Copyright 2022 RaisingCanesFanClub"

#include "api-endpoints/api-endpoints-lib.h"
#include "MockDBService.h"
#include "MockAuthService.h"

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;

namespace {
class MockAPIEndPoints : public APIEndPoints {
 public:
    MockAPIEndPoints(DBService* db, AuthService* auth) : APIEndPoints(db, auth) {}
    MOCK_METHOD((std::pair<int, std::string>), authenticateTokenGetErrorCode, (const crow::request& req), (override));
    MOCK_METHOD(bool, developerOwnsGame, (std::string developer_email, int game_id), (override));
};

class PlayerTestFixture : public testing::Test {
 protected:
    void SetUp() override {
        // Adding developers
        valid_developer1.developer_email = "dev1@gmail.com";
        valid_developer1.developer_password = "some_password1";
        valid_developer1.is_valid = true;

        valid_developer2.developer_email = "dev2@gmail.com";
        valid_developer2.developer_password = "some_password2";
        valid_developer2.is_valid = true;

        valid_developer3.developer_email = "dev3@gmail.com";
        valid_developer3.developer_password = "some_password3";
        valid_developer3.is_valid = true;

        invalid_developer.is_valid = false;

        // Adding games
        game1.developer_email = valid_developer1.developer_email;
        game1.game_name = "Skip-Bo";
        game1.category = "Cards";
        game1.game_parameter1_name = "stacks";
        game1.game_parameter2_name = "cardsLeft";
        game1.game_parameter3_name = "stacksCleared";
        game1.game_parameter4_name = "cardsHeld";
        game1.game_parameter1_weight = 2.5;
        game1.game_parameter2_weight = 3.7;
        game1.game_parameter3_weight = 1.8;
        game1.game_parameter4_weight = 0.7;
        game1.teams_per_match = 2;
        game1.players_per_team = 1;
        game1.is_valid = true;
        game1.game_id = 1;

        game2.developer_email = valid_developer2.developer_email;
        game2.game_name = "Speed";
        game2.category = "Cards";
        game2.game_parameter1_name = "timeFinished";
        game2.game_parameter2_name = "cardsLeft";
        game2.game_parameter3_name = "cardsStacked";
        game2.game_parameter4_name = "cardsUsed";
        game2.game_parameter1_weight = 1.25;
        game2.game_parameter2_weight = 3.75;
        game2.game_parameter3_weight = 1.4;
        game2.game_parameter4_weight = 0.63;
        game2.teams_per_match = 2;
        game2.players_per_team = 1;
        game2.is_valid = true;
        game2.game_id = 2;

        game3.developer_email = valid_developer1.developer_email;
        game3.game_name = "War";
        game3.category = "Cards";
        game3.game_parameter1_name = "cardsGainedFromOpponent";
        game3.game_parameter2_name = "cardsLostToOpponent";
        game3.game_parameter3_name = "cardsHeld";
        game3.game_parameter4_name = "warsWon";
        game3.game_parameter1_weight = 2.20;
        game3.game_parameter2_weight = 3.27;
        game3.game_parameter3_weight = 3.22;
        game3.game_parameter4_weight = 1.02;
        game3.teams_per_match = 2;
        game3.players_per_team = 1;
        game3.is_valid = true;
        game3.game_id = 3;

        game4.developer_email = valid_developer2.developer_email;
        game4.game_name = "Monopoly";
        game4.category = "Board";
        game4.game_parameter1_name = "hotelsOwned";
        game4.game_parameter2_name = "timesPassedGo";
        game4.game_parameter3_name = "cash";
        game4.game_parameter4_name = "citiesOwned";
        game4.game_parameter1_weight = 12.20;
        game4.game_parameter2_weight = 3.27;
        game4.game_parameter3_weight = 3.28;
        game4.game_parameter4_weight = 10.20;
        game4.teams_per_match = 6;
        game4.players_per_team = 1;
        game4.is_valid = true;
        game4.game_id = 4;

        // Adding players
        p1.player_email = "player1@gmail.com";
        p1.is_valid = true;

        p2.player_email = "player2@gmail.com";
        p2.is_valid = true;

        p3.player_email = "player3@gmail.com";
        p3.is_valid = true;

        p7_new.player_email = "player7@gmail.com";
        p7_new.is_valid = false;

        p7_created.player_email = p7_new.player_email;
        p7_created.is_valid = true;

        // Adding players' stats
        pgr1.player_email = p1.player_email;
        pgr1.game_id = game1.game_id;
        pgr1.game_parameter1_value = 10.1;
        pgr1.game_parameter2_value = 8.7;
        pgr1.game_parameter3_value = 4.62;
        pgr1.game_parameter4_value = 3.33;
        pgr1.is_valid = true;

        pgr2.player_email = p2.player_email;
        pgr2.game_id = game1.game_id;
        pgr2.game_parameter1_value = 1.1;
        pgr2.game_parameter2_value = 2.7;
        pgr2.game_parameter3_value = 24.62;
        pgr2.game_parameter4_value = 13.33;
        pgr2.is_valid = true;

        pgr3.player_email = p3.player_email;
        pgr3.game_id = game1.game_id;
        pgr3.game_parameter1_value = 10.12;
        pgr3.game_parameter2_value = 38.7;
        pgr3.game_parameter3_value = 44.62;
        pgr3.game_parameter4_value = 31.33;
        pgr3.is_valid = true;

        pgr4.player_email = "player4@gmail.com";
        pgr4.game_id = game2.game_id;
        pgr4.game_parameter1_value = 13.1;
        pgr4.game_parameter2_value = 80.7;
        pgr4.game_parameter3_value = 42.62;
        pgr4.game_parameter4_value = 3.73;
        pgr4.is_valid = true;

        pgr5.player_email = "player5@gmail.com";
        pgr5.game_id = game3.game_id;
        pgr5.game_parameter1_value = 12.1;
        pgr5.game_parameter2_value = 18.7;
        pgr5.game_parameter3_value = 4.92;
        pgr5.game_parameter4_value = 23.33;
        pgr5.is_valid = true;

        pgr6.player_email = "player6@gmail.com";
        pgr6.game_id = game3.game_id;
        pgr6.game_parameter1_value = 10.1;
        pgr6.game_parameter2_value = 28.7;
        pgr6.game_parameter3_value = 4.99;
        pgr6.game_parameter4_value = 13.33;
        pgr6.is_valid = true;

        pgr7_created_error.player_email = p7_new.player_email;
        pgr7_created_error.is_valid = false;

        pgr7.player_email = p7_new.player_email;
        pgr7.game_id = game1.game_id;
        pgr7.game_parameter1_value = 12.1;
        pgr7.game_parameter2_value = 22.7;
        pgr7.game_parameter3_value = 4.79;
        pgr7.game_parameter4_value = 18.33;
        pgr7.is_valid = true;

        pgr_invalid1.player_email = "player.invalid1@gmail.com";
        pgr_invalid1.game_id = game2.game_id;
        pgr_invalid1.is_valid = false;

        pgr_invalid2.player_email = "player.invalid2@gmail.com";
        pgr_invalid2.is_valid = false;

        // Adding players joined with their game ratings
        jpgr_game1_pgr1.player_email = pgr1.player_email;
        jpgr_game1_pgr1.game_id = pgr1.game_id;
        jpgr_game1_pgr1.game_parameter1_name = game1.game_parameter1_name;
        jpgr_game1_pgr1.game_parameter1_value = pgr1.game_parameter1_value;
        jpgr_game1_pgr1.game_parameter2_name = game1.game_parameter2_name;
        jpgr_game1_pgr1.game_parameter2_value = pgr1.game_parameter2_value;
        jpgr_game1_pgr1.game_parameter3_name = game1.game_parameter3_name;
        jpgr_game1_pgr1.game_parameter3_value = pgr1.game_parameter3_value;
        jpgr_game1_pgr1.game_parameter4_name = game1.game_parameter4_name;
        jpgr_game1_pgr1.game_parameter4_value = pgr1.game_parameter4_value;
        jpgr_game1_pgr1.is_valid = true;

        jpgr_game1_pgr2.player_email = pgr2.player_email;
        jpgr_game1_pgr2.game_id = pgr2.game_id;
        jpgr_game1_pgr2.game_parameter1_name = game1.game_parameter1_name;
        jpgr_game1_pgr2.game_parameter1_value = pgr2.game_parameter1_value;
        jpgr_game1_pgr2.game_parameter2_name = game1.game_parameter2_name;
        jpgr_game1_pgr2.game_parameter2_value = pgr2.game_parameter2_value;
        jpgr_game1_pgr2.game_parameter3_name = game1.game_parameter3_name;
        jpgr_game1_pgr2.game_parameter3_value = pgr2.game_parameter3_value;
        jpgr_game1_pgr2.game_parameter4_name = game1.game_parameter4_name;
        jpgr_game1_pgr2.game_parameter4_value = pgr2.game_parameter4_value;
        jpgr_game1_pgr2.is_valid = true;

        jpgr_game1_pgr3.player_email = pgr3.player_email;
        jpgr_game1_pgr3.game_id = pgr3.game_id;
        jpgr_game1_pgr3.game_parameter1_name = game1.game_parameter1_name;
        jpgr_game1_pgr3.game_parameter1_value = pgr3.game_parameter1_value;
        jpgr_game1_pgr3.game_parameter2_name = game1.game_parameter2_name;
        jpgr_game1_pgr3.game_parameter2_value = pgr3.game_parameter2_value;
        jpgr_game1_pgr3.game_parameter3_name = game1.game_parameter3_name;
        jpgr_game1_pgr3.game_parameter3_value = pgr3.game_parameter3_value;
        jpgr_game1_pgr3.game_parameter4_name = game1.game_parameter4_name;
        jpgr_game1_pgr3.game_parameter4_value = pgr3.game_parameter4_value;
        jpgr_game1_pgr3.is_valid = true;

        jpgr_game2_pgr4.player_email = pgr4.player_email;
        jpgr_game2_pgr4.game_id = pgr4.game_id;
        jpgr_game2_pgr4.game_parameter1_name = game2.game_parameter1_name;
        jpgr_game2_pgr4.game_parameter1_value = pgr4.game_parameter1_value;
        jpgr_game2_pgr4.game_parameter2_name = game2.game_parameter2_name;
        jpgr_game2_pgr4.game_parameter2_value = pgr4.game_parameter2_value;
        jpgr_game2_pgr4.game_parameter3_name = game2.game_parameter3_name;
        jpgr_game2_pgr4.game_parameter3_value = pgr4.game_parameter3_value;
        jpgr_game2_pgr4.game_parameter4_name = game2.game_parameter4_name;
        jpgr_game2_pgr4.game_parameter4_value = pgr4.game_parameter4_value;
        jpgr_game2_pgr4.is_valid = true;

        jpgr_game2_pgr_invalid1.player_email = pgr_invalid1.player_email;
        jpgr_game2_pgr_invalid1.is_valid = false;

        // Authorization codes returned by authenticateTokenGetErrorCode
        auth_code_valid = std::make_pair(200, valid_developer1.developer_email);
        auth_code_invalid_header = std::make_pair(400, "Invalid Header");
    }

    MockDBService DB;
    MockAuthService auth;

    std::pair<int, std::string> auth_code_valid;
    std::pair<int, std::string> auth_code_invalid_header;

    Developer valid_developer1;
    Developer valid_developer2;
    Developer valid_developer3;
    Developer invalid_developer;

    Game_Details game1;
    Game_Details game2;
    Game_Details game3;
    Game_Details game4;

    Player p1;
    Player p2;
    Player p3;
    Player p7_new;
    Player p7_created;

    Player_Game_Ratings pgr1;
    Player_Game_Ratings pgr2;
    Player_Game_Ratings pgr3;
    Player_Game_Ratings pgr4;
    Player_Game_Ratings pgr5;
    Player_Game_Ratings pgr6;
    Player_Game_Ratings pgr7_created_error;
    Player_Game_Ratings pgr7;
    Player_Game_Ratings pgr_invalid1;
    Player_Game_Ratings pgr_invalid2;

    Joined_Player_Game_Ratings jpgr_game1_pgr1;
    Joined_Player_Game_Ratings jpgr_game1_pgr2;
    Joined_Player_Game_Ratings jpgr_game1_pgr3;
    Joined_Player_Game_Ratings jpgr_game2_pgr4;
    Joined_Player_Game_Ratings jpgr_game2_pgr_invalid1;

    crow::request req;
    crow::response res;
    crow::json::wvalue body;
    crow::json::wvalue return_body;
};

/*
    --------------------------------------------------------------------------------
    Endpoint: Get Game Players
    (Get all players for a requested game)
    --------------------------------------------------------------------------------
*/

/* Testing invalid developer authentication*/
TEST_F(PlayerTestFixture, GetGamePlayersAuthTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Invalid header
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_invalid_header));
    res = api.getGamePlayers(req, game1.game_id);
    EXPECT_EQ(res.code, 400);
    EXPECT_EQ(res.body, "Invalid Header");

    // Developer does not own game
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(false));

    res = api.getGamePlayers(req, game1.game_id);
    std::string error = "Do not have access to game with game_id " + std::to_string(game1.game_id);
    EXPECT_EQ(res.code, 403);
    EXPECT_EQ(res.body, error);
}

/* No players for this game */
TEST_F(PlayerTestFixture, GetGamePlayersNoPlayersTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Mocking
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(true));

    std::vector<Joined_Player_Game_Ratings> joined_players;
    EXPECT_CALL(DB, get_all_player_game_ratings_for_game(_)).Times(1)
    .WillOnce(Return(joined_players));

    // Performing test
    res = api.getGamePlayers(req, game4.game_id);
    EXPECT_EQ(res.code, 204);
    std::string game4_game_id = std::to_string(game4.game_id);
    EXPECT_EQ(res.body, "No players found for game_id " + game4_game_id);
}

/* Developer owns multiple games, each with players */
TEST_F(PlayerTestFixture, GetGamePlayersDevOwnsMultipleGamesTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Mocking
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(true));

    std::vector<Joined_Player_Game_Ratings> joined_players;
    joined_players.push_back(jpgr_game1_pgr1);
    joined_players.push_back(jpgr_game1_pgr2);
    joined_players.push_back(jpgr_game1_pgr3);
    EXPECT_CALL(DB, get_all_player_game_ratings_for_game(_)).Times(1)
    .WillOnce(Return(joined_players));

    // Performing test
    res = api.getGamePlayers(req, game1.game_id);
    return_body = {
        {pgr1.player_email, {
            {"game_id", game1.game_id},
            {game1.game_parameter1_name, pgr1.game_parameter1_value},
            {game1.game_parameter2_name, pgr1.game_parameter2_value},
            {game1.game_parameter3_name, pgr1.game_parameter3_value},
            {game1.game_parameter4_name, pgr1.game_parameter4_value},
        }},
        {pgr2.player_email, {
            {"game_id", game1.game_id},
            {game1.game_parameter1_name, pgr2.game_parameter1_value},
            {game1.game_parameter2_name, pgr2.game_parameter2_value},
            {game1.game_parameter3_name, pgr2.game_parameter3_value},
            {game1.game_parameter4_name, pgr2.game_parameter4_value},
        }},
        {pgr3.player_email, {
            {"game_id", game1.game_id},
            {game1.game_parameter1_name, pgr3.game_parameter1_value},
            {game1.game_parameter2_name, pgr3.game_parameter2_value},
            {game1.game_parameter3_name, pgr3.game_parameter3_value},
            {game1.game_parameter4_name, pgr3.game_parameter4_value},
        }}
    };
    EXPECT_EQ(res.code, 200);
    EXPECT_EQ(res.body, return_body.dump());
}

/* Invalid player */
TEST_F(PlayerTestFixture, GetGamePlayersInvalidPlayerTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Mocking
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(true));

    std::vector<Joined_Player_Game_Ratings> joined_players;
    joined_players.push_back(jpgr_game2_pgr4);
    joined_players.push_back(jpgr_game2_pgr_invalid1);
    EXPECT_CALL(DB, get_all_player_game_ratings_for_game(_)).Times(1)
    .WillOnce(Return(joined_players));

    // Performing test
    res = api.getGamePlayers(req, game2.game_id);
    EXPECT_EQ(res.code, 500);
    EXPECT_EQ(res.body, "Internal Server Error due to player " + pgr_invalid1.player_email);
}

/*
    --------------------------------------------------------------------------------
    Endpoint: Add Player Stats
    (Add player stats for a game)
    --------------------------------------------------------------------------------
*/

/* Testing invalid developer authentication*/
TEST_F(PlayerTestFixture, AddPlayerStatsAuthTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Invalid header
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_invalid_header));
    res = api.addPlayersStats(req, 1);
    EXPECT_EQ(res.code, 400);
    EXPECT_EQ(res.body, "Invalid Header");

    // Developer does not own game
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(false));

    res = api.addPlayersStats(req, game1.game_id);
    std::string error = "Do not have access to game with game_id " + std::to_string(game1.game_id);
    EXPECT_EQ(res.code, 403);
    EXPECT_EQ(res.body, error);
}

/* No players provided -- Therefore cannot add any stats */
TEST_F(PlayerTestFixture, AddPlayerStatsNoPlayersTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Mocking
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(true));

    // Performing test
    body = {};
    req.body = body.dump();
    res = api.addPlayersStats(req, game1.game_id);
    EXPECT_EQ(res.code, 200);
    EXPECT_EQ(res.body, "No player stats added due to empty request");
}

/* Adding player stats for already existing players */
TEST_F(PlayerTestFixture, AddPlayerStatsPlayersExistTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Mocking
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(true));

    EXPECT_CALL(DB, get_player(_)).Times(2)
    .WillOnce(Return(p1))
    .WillOnce(Return(p2));

    EXPECT_CALL(DB, add_player_rating(_)).Times(2)
    .WillOnce(Return(pgr1))
    .WillOnce(Return(pgr2));

    // Performing test
    body = {
        {pgr1.player_email, {
            {"game_parameter1_value", pgr1.game_parameter1_value},
            {"game_parameter2_value", pgr1.game_parameter2_value},
            {"game_parameter3_value", pgr1.game_parameter3_value},
            {"game_parameter4_value", pgr1.game_parameter4_value},
        }},
        {pgr2.player_email, {
            {"game_parameter1_value", pgr2.game_parameter1_value},
            {"game_parameter2_value", pgr2.game_parameter2_value},
            {"game_parameter3_value", pgr2.game_parameter3_value},
            {"game_parameter4_value", pgr2.game_parameter4_value},
        }}
    };
    req.body = body.dump();
    res = api.addPlayersStats(req, game1.game_id);
    EXPECT_EQ(res.code, 200);
    EXPECT_EQ(res.body, "Player stats were added");
}

/* Adding player stats for player which doesn't exist yet */
TEST_F(PlayerTestFixture, AddPlayerStatsPlayerDoesntExistTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Mocking
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(true));

    EXPECT_CALL(DB, get_player(_)).Times(1)
    .WillOnce(Return(p7_new));
    EXPECT_CALL(DB, add_player(_)).Times(1)
    .WillOnce(Return(p7_created));

    EXPECT_CALL(DB, add_player_rating(_)).Times(1)
    .WillOnce(Return(pgr7));

    // Performing test
    body = {
        {pgr7.player_email, {
            {"game_parameter1_value", pgr7.game_parameter1_value},
            {"game_parameter2_value", pgr7.game_parameter2_value},
            {"game_parameter3_value", pgr7.game_parameter3_value},
            {"game_parameter4_value", pgr7.game_parameter4_value},
        }}
    };
    req.body = body.dump();
    res = api.addPlayersStats(req, game1.game_id);
    EXPECT_EQ(res.code, 200);
    EXPECT_EQ(res.body, "Player stats were added");
}

/* Adding player stats for player which doesn't exist yet -- Player cannot be added */
TEST_F(PlayerTestFixture, AddPlayerStatsPlayerDoesntExistCreateErrorTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Mocking
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(true));

    EXPECT_CALL(DB, get_player(_)).Times(1)
    .WillOnce(Return(p7_new));
    EXPECT_CALL(DB, add_player(_)).Times(1)
    .WillOnce(Return(p7_new));

    // Performing test
    body = {
        {pgr7.player_email, {
            {"game_parameter1_value", pgr7.game_parameter1_value},
            {"game_parameter2_value", pgr7.game_parameter2_value},
            {"game_parameter3_value", pgr7.game_parameter3_value},
            {"game_parameter4_value", pgr7.game_parameter4_value},
        }}
    };
    req.body = body.dump();
    res = api.addPlayersStats(req, game1.game_id);
    EXPECT_EQ(res.code, 500);
    EXPECT_EQ(res.body, "Internal Server Error due to player " + p7_new.player_email);
}

/* Adding player stats for already existing players -- Player stats cannot be added */
TEST_F(PlayerTestFixture, AddPlayerStatsPlayersExistAddingStatsErrorTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Mocking
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(true));

    EXPECT_CALL(DB, get_player(_)).Times(2)
    .WillOnce(Return(p1))
    .WillOnce(Return(p7_created));

    EXPECT_CALL(DB, add_player_rating(_)).Times(2)
    .WillOnce(Return(pgr1))
    .WillOnce(Return(pgr7_created_error));

    // Performing test
    body = {
        {pgr1.player_email, {
            {"game_parameter1_value", pgr1.game_parameter1_value},
            {"game_parameter2_value", pgr1.game_parameter2_value},
            {"game_parameter3_value", pgr1.game_parameter3_value},
            {"game_parameter4_value", pgr1.game_parameter4_value},
        }},
        {pgr7.player_email, {
            {"game_parameter1_value", pgr7.game_parameter1_value},
            {"game_parameter2_value", pgr7.game_parameter2_value},
            {"game_parameter3_value", pgr7.game_parameter3_value},
            {"game_parameter4_value", pgr7.game_parameter4_value},
        }}
    };
    req.body = body.dump();
    res = api.addPlayersStats(req, game1.game_id);
    EXPECT_EQ(res.code, 500);
    EXPECT_EQ(res.body, "Internal Server Error due to player " + pgr7_created_error.player_email);
}

/*
    --------------------------------------------------------------------------------
    Endpoint: Get Players Stats
    (Get specific players' stats for a game)
    --------------------------------------------------------------------------------
*/

/* Testing invalid developer authentication*/
TEST_F(PlayerTestFixture, GetPlayersStatsAuthTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Invalid header
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_invalid_header));
    res = api.getPlayersStats(req, 1);
    EXPECT_EQ(res.code, 400);
    EXPECT_EQ(res.body, "Invalid Header");

    // Developer does not own game
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(false));

    res = api.getPlayersStats(req, game1.game_id);
    std::string error = "Do not have access to game with game_id " + std::to_string(game1.game_id);
    EXPECT_EQ(res.code, 403);
    EXPECT_EQ(res.body, error);
}

/* Player game rating is missing for multiple players */
TEST_F(PlayerTestFixture, GetPlayersStatsMissingStatsTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Mocking
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(true));

    EXPECT_CALL(DB, get_player_game_rating(_, _)).Times(4)
    .WillOnce(Return(pgr1))
    .WillOnce(Return(pgr_invalid1))
    .WillOnce(Return(pgr_invalid2))
    .WillOnce(Return(pgr2));

    // Performing test
    std::vector<std::string> player_emails;
    player_emails.push_back(pgr1.player_email);
    player_emails.push_back(pgr_invalid1.player_email);
    player_emails.push_back(pgr_invalid2.player_email);
    player_emails.push_back(pgr2.player_email);

    body = {
        {"developer_email", valid_developer1.developer_email},
        {"developer_password", valid_developer1.developer_password}
    };
    body["player_emails"] = player_emails;
    req.body = body.dump();
    res = api.getPlayersStats(req, game1.game_id);
    EXPECT_EQ(res.code, 204);
    std::string missing_players = pgr_invalid1.player_email + ", "  + pgr_invalid2.player_email;
    EXPECT_EQ(res.body, "The following players do not exist " + missing_players);
}

/* Player game ratings are valid */
TEST_F(PlayerTestFixture, GetPlayersStatsValidTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Mocking
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(true));

    EXPECT_CALL(DB, get_player_game_rating(_, _)).Times(3)
    .WillOnce(Return(pgr1))
    .WillOnce(Return(pgr2))
    .WillOnce(Return(pgr3));

    // Performing test
    std::vector<std::string> player_emails;
    player_emails.push_back(pgr1.player_email);
    player_emails.push_back(pgr2.player_email);
    player_emails.push_back(pgr3.player_email);

    body = {
        {"developer_email", valid_developer1.developer_email},
        {"developer_password", valid_developer1.developer_password}
    };
    body["player_emails"] = player_emails;
    req.body = body.dump();

    return_body[pgr1.player_email]["game_parameter1_value"] = pgr1.game_parameter1_value;
    return_body[pgr1.player_email]["game_parameter2_value"] = pgr1.game_parameter2_value;
    return_body[pgr1.player_email]["game_parameter3_value"] = pgr1.game_parameter3_value;
    return_body[pgr1.player_email]["game_parameter4_value"] = pgr1.game_parameter4_value;
    return_body[pgr2.player_email]["game_parameter1_value"] = pgr2.game_parameter1_value;
    return_body[pgr2.player_email]["game_parameter2_value"] = pgr2.game_parameter2_value;
    return_body[pgr2.player_email]["game_parameter3_value"] = pgr2.game_parameter3_value;
    return_body[pgr2.player_email]["game_parameter4_value"] = pgr2.game_parameter4_value;
    return_body[pgr3.player_email]["game_parameter1_value"] = pgr3.game_parameter1_value;
    return_body[pgr3.player_email]["game_parameter2_value"] = pgr3.game_parameter2_value;
    return_body[pgr3.player_email]["game_parameter3_value"] = pgr3.game_parameter3_value;
    return_body[pgr3.player_email]["game_parameter4_value"] = pgr3.game_parameter4_value;

    res = api.getPlayersStats(req, game1.game_id);
    EXPECT_EQ(res.code, 200);
    EXPECT_EQ(res.body, return_body.dump());
}

/*
    --------------------------------------------------------------------------------
    Endpoint: Delete Players Stats
    (Delete requested players' stats for a given game)
    --------------------------------------------------------------------------------
*/

/* Testing invalid developer authentication*/
TEST_F(PlayerTestFixture, DeletePlayersStatsAuthTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Invalid header
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_invalid_header));
    res = api.deletePlayersStats(req, 1);
    EXPECT_EQ(res.code, 400);
    EXPECT_EQ(res.body, "Invalid Header");

    // Developer does not own game
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(false));

    res = api.deletePlayersStats(req, game1.game_id);
    std::string error = "Do not have access to game with game_id " + std::to_string(game1.game_id);
    EXPECT_EQ(res.code, 403);
    EXPECT_EQ(res.body, error);
}

/* Attempting to delete multiple players that don't exist */
TEST_F(PlayerTestFixture, DeletePlayersStatsMissingPlayersTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Mocking
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(true));

    EXPECT_CALL(DB, remove_player_rating(_, _)).Times(4)
    .WillOnce(Return(pgr1))
    .WillOnce(Return(pgr_invalid1))
    .WillOnce(Return(pgr_invalid2))
    .WillOnce(Return(pgr2));

    // Performing test
    std::vector<std::string> player_emails;
    player_emails.push_back(pgr1.player_email);
    player_emails.push_back(pgr_invalid1.player_email);
    player_emails.push_back(pgr_invalid2.player_email);
    player_emails.push_back(pgr2.player_email);

    body = {};
    body["player_emails"] = player_emails;
    req.body = body.dump();
    res = api.deletePlayersStats(req, game1.game_id);
    EXPECT_EQ(res.code, 500);
    std::string missing_players = pgr_invalid1.player_email + ", "  + pgr_invalid2.player_email;
    EXPECT_EQ(res.body, "The following players do not exist: " + missing_players);
}

/* Player game ratings are valid */
TEST_F(PlayerTestFixture, DeletePlayersStatsMissingStatsTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Mocking
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(true));

    EXPECT_CALL(DB, remove_player_rating(_, _)).Times(3)
    .WillOnce(Return(pgr1))
    .WillOnce(Return(pgr2))
    .WillOnce(Return(pgr3));

    // Performing test
    std::vector<std::string> player_emails;
    player_emails.push_back(pgr1.player_email);
    player_emails.push_back(pgr2.player_email);
    player_emails.push_back(pgr3.player_email);

    body = {};
    body["player_emails"] = player_emails;
    req.body = body.dump();
    res = api.deletePlayersStats(req, game1.game_id);
    EXPECT_EQ(res.code, 200);
    EXPECT_EQ(res.body, "Player stats were removed");
}

/*
    --------------------------------------------------------------------------------
    Endpoint: Update Players Stats
    (Update requested players' stats for a given game)
    --------------------------------------------------------------------------------
*/

/* Testing invalid developer authentication*/
TEST_F(PlayerTestFixture, UpdatePlayersStatsAuthTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Invalid header
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_invalid_header));
    res = api.updatePlayersStats(req, 1);
    EXPECT_EQ(res.code, 400);
    EXPECT_EQ(res.body, "Invalid Header");

    // Developer does not own game
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(false));

    res = api.updatePlayersStats(req, game1.game_id);
    std::string error = "Do not have access to game with game_id " + std::to_string(game1.game_id);
    EXPECT_EQ(res.code, 403);
    EXPECT_EQ(res.body, error);
}

/* Not giving any stats to update */
TEST_F(PlayerTestFixture, UpdatePlayersStatsMissingStatsTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Mocking
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(true));

    // Performing test
    body = {};
    req.body = body.dump();
    res = api.updatePlayersStats(req, game1.game_id);
    EXPECT_EQ(res.code, 200);
    EXPECT_EQ(res.body, "No player stats added due to empty request");
}

/* Attempting to update players stats with invalid stats */
TEST_F(PlayerTestFixture, UpdatePlayersStatsInvalidStatsTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Mocking
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(true));

    // Performing test
    body = {
        {pgr1.player_email, {
            {"game_parameter1_value", pgr1.game_parameter1_value},
            {"game_parameter2_value", pgr1.game_parameter2_value},
            {"game_parameter3_value", pgr1.game_parameter3_value},
            {"game_parameter4_value", pgr1.game_parameter4_value}
        }},
        {pgr_invalid1.player_email, {
            {"game_parameter2_value", pgr_invalid1.game_parameter2_value}
        }},
        {pgr_invalid2.player_email, {
            {"game_parameter4_value", pgr_invalid2.game_parameter4_value}
        }},
        {pgr4.player_email, {
            {"game_parameter1_value", pgr4.game_parameter1_value},
            {"game_parameter2_value", pgr4.game_parameter2_value},
            {"game_parameter3_value", pgr4.game_parameter3_value},
            {"game_parameter4_value", pgr4.game_parameter4_value}
        }},
    };
    req.body = body.dump();
    res = api.updatePlayersStats(req, game1.game_id);
    EXPECT_EQ(res.code, 204);
    // Players can be listed in any order as long as both players are stated as invalid
    std::string invalid_player_stats1 = "The following players have invalid stats: " + pgr_invalid1.player_email +
        ", "  + pgr_invalid2.player_email;
    std::string invalid_player_stats2 = "The following players have invalid stats: " + pgr_invalid2.player_email +
        ", "  + pgr_invalid1.player_email;
    EXPECT_TRUE((res.body ==  invalid_player_stats1) || (res.body == invalid_player_stats2));
}

/* Attempting to update stats for new player -- But cannot add player */
TEST_F(PlayerTestFixture, UpdatePlayersStatsCannotAddPlayerTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Mocking
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(true));

    EXPECT_CALL(DB, get_player(_)).Times(1)
    .WillOnce(Return(p7_new));

    EXPECT_CALL(DB, add_player(_)).Times(1)
    .WillOnce(Return(p7_new));

    // Performing test
    body = {
        {pgr7.player_email, {
            {"game_parameter1_value", pgr7.game_parameter1_value},
            {"game_parameter2_value", pgr7.game_parameter2_value},
            {"game_parameter3_value", pgr7.game_parameter3_value},
            {"game_parameter4_value", pgr7.game_parameter4_value}
        }}
    };
    req.body = body.dump();
    res = api.updatePlayersStats(req, game1.game_id);
    EXPECT_EQ(res.code, 500);
    EXPECT_EQ(res.body, "Internal Server Error due to player " + p7_new.player_email);
}

/* Unable to update player rating */
TEST_F(PlayerTestFixture, UpdatePlayersStatsCannotUpdateStatsTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Mocking
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(true));

    EXPECT_CALL(DB, get_player(_)).Times(1)
    .WillOnce(Return(p7_new));

    EXPECT_CALL(DB, add_player(_)).Times(1)
    .WillOnce(Return(p7_created));

    EXPECT_CALL(DB, update_player_rating(_, _, _)).Times(1)
    .WillOnce(Return(pgr7_created_error));

    // Performing test
    body = {
        {pgr7.player_email, {
            {"game_parameter1_value", pgr7.game_parameter1_value},
            {"game_parameter2_value", pgr7.game_parameter2_value},
            {"game_parameter3_value", pgr7.game_parameter3_value},
            {"game_parameter4_value", pgr7.game_parameter4_value}
        }}
    };
    req.body = body.dump();
    res = api.updatePlayersStats(req, game1.game_id);
    EXPECT_EQ(res.code, 500);
    EXPECT_EQ(res.body, "Internal Server Error due to player " + pgr7_created_error.player_email);
}

/* Valid update */
TEST_F(PlayerTestFixture, UpdatePlayersStatsValidTest) {
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    // Mocking
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(auth_code_valid));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(true));

    EXPECT_CALL(DB, get_player(_)).Times(3)
    .WillOnce(Return(p1))
    .WillOnce(Return(p2))
    .WillOnce(Return(p3));

    EXPECT_CALL(DB, update_player_rating(_, _, _)).Times(3)
    .WillOnce(Return(pgr1))
    .WillOnce(Return(pgr2))
    .WillOnce(Return(pgr3));

    // Performing test
    body = {
        {pgr1.player_email, {
            {"game_parameter1_value", pgr1.game_parameter1_value},
            {"game_parameter2_value", pgr1.game_parameter2_value},
            {"game_parameter3_value", pgr1.game_parameter3_value},
            {"game_parameter4_value", pgr1.game_parameter4_value}
        }},
        {pgr2.player_email, {
            {"game_parameter1_value", pgr2.game_parameter1_value},
            {"game_parameter2_value", pgr2.game_parameter2_value},
            {"game_parameter3_value", pgr2.game_parameter3_value},
            {"game_parameter4_value", pgr2.game_parameter4_value}
        }},
        {pgr3.player_email, {
            {"game_parameter1_value", pgr3.game_parameter1_value},
            {"game_parameter2_value", pgr3.game_parameter2_value},
            {"game_parameter3_value", pgr3.game_parameter3_value},
            {"game_parameter4_value", pgr3.game_parameter4_value}
        }}
    };
    req.body = body.dump();
    res = api.updatePlayersStats(req, game1.game_id);
    EXPECT_EQ(res.code, 200);
    EXPECT_EQ(res.body, "Player stats were added");
}
}  // namespace
