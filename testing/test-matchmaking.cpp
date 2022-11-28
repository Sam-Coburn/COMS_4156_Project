// "Copyright [year] <Copyright Owner>"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "main/db-service.h"
#include "api-endpoints/api-endpoints-lib.h"

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
  MOCK_METHOD(Player_Game_Ratings, update_player_rating,
  (std::string player_email, int game_id, Player_Game_Ratings PGR), (override));
};

class MockMatchmaking: public Matchmaking {
 public:
  MOCK_METHOD((std::tuple<
        std::vector<std::vector<std::vector<std::string> > >,
        std::vector<std::string> >), matchmakingBackendBasic,
        (int game_id, std::vector<std::string> player_emails, DBService* DB));
  MOCK_METHOD((std::tuple<
        std::vector<std::vector<std::vector<std::string> > >,
        std::vector<std::string> >), matchmakingBackendAdvanced,
        (int game_id, std::vector<std::string> player_emails, DBService* DB));
};

TEST(MatchmakingTests,  Matchmaking_Endpoint_Tests_Set1) {
    MockDBService DB;
    MockAuthService auth;
    MockMatchmaking M;

    APIEndPoints api = APIEndPoints(&DB, &auth);

    crow::request req;
    crow::response res;
    crow::json::wvalue body;

    Developer valid_developer;
    valid_developer.developer_email = "some_email@gmail.com";
    valid_developer.developer_password = "some_password";
    valid_developer.is_valid = true;

    std::vector<Game_Details> games;

    EXPECT_CALL(DB, get_developer(_))
    .WillRepeatedly(Return(valid_developer));

    EXPECT_CALL(DB, get_all_games_for_developer(_))
    .WillRepeatedly(Return(games));

    EXPECT_CALL(auth, decodeAndVerifyJWT(_))
    .WillRepeatedly(Return(std::make_pair(200, "some_email@gmail.com")));

    // Test: Empty Request Body
    body = {};
    req.body = body.dump();
    req.add_header("Authorization", "VALID TOKEN");
    res =  api.matchmake(req, &M);
    ASSERT_EQ(res.code, 400);

    // Test: No Game ID Given
    body = {
        {"matchmaking_type", "basic"},
        {"player_emails", "[\"player1@gmail.com\"]"}
    };
    req.body = body.dump();
    req.add_header("Authorization", "VALID TOKEN");
    res =  api.matchmake(req, &M);
    ASSERT_EQ(res.code, 400);
    ASSERT_EQ(res.body, "Incorrect Request Format.\n");

    // Test: No Player Emails Given
    body = {
        {"game_id", 1}
    };
    req.body = body.dump();
    req.add_header("Authorization", "VALID TOKEN");
    res =  api.matchmake(req, &M);
    ASSERT_EQ(res.code, 400);
}

TEST(MatchmakingTests,  Matchmaking_Endpoint_Tests_Set2) {
    MockDBService DB;
    MockAuthService auth;
    MockMatchmaking M;

    APIEndPoints api = APIEndPoints(&DB, &auth);

    std::vector<Game_Details> good_developer_games;
    Game_Details g;
    g.game_id = 1;
    good_developer_games.push_back(g);

    std::vector<std::string> player_names;
    player_names.push_back("player_1@gmail.com");
    player_names.push_back("player_2@gmail.com");
    player_names.push_back("player_3@gmail.com");
    player_names.push_back("player_4@gmail.com");

    Player p_good;
    p_good.is_valid = true;

    Player p_bad;
    p_bad.is_valid = false;

    Game_Details chess;
    chess.game_parameter1_weight = 1;
    chess.game_parameter1_weight = 0;
    chess.game_parameter1_weight = 0;
    chess.game_parameter1_weight = 0;
    chess.players_per_team = 1;
    chess.teams_per_match = 2;
    chess.category = "Strategy";
    chess.is_valid = true;

    Player_Game_Ratings player_1;
    player_1.game_parameter1_value = 2000;
    player_1.game_parameter2_value = 0;
    player_1.game_parameter3_value = 0;
    player_1.game_parameter4_value = 0;
    player_1.is_valid = true;

    Developer valid_developer;
    valid_developer.developer_email = "developer@gmail.com";
    valid_developer.developer_password = "some_password";
    valid_developer.is_valid = true;

    std::tuple<
    std::vector<std::vector<std::vector<std::string> > >,
    std::vector<std::string> > matchmaking_result;

    EXPECT_CALL(DB, get_developer(_))
    .WillRepeatedly(Return(valid_developer));

    EXPECT_CALL(auth, decodeAndVerifyJWT(_))
    .WillRepeatedly(Return(std::make_pair(200, "developer@gmail.com")));

    EXPECT_CALL(DB, get_all_games_for_developer(_))
    .WillRepeatedly(Return(good_developer_games));

    EXPECT_CALL(DB, get_player(_))
    .WillOnce(Return(p_good))
    .WillOnce(Return(p_bad))
    .WillRepeatedly(Return(p_good));

    EXPECT_CALL(DB, get_player_game_rating(_, _))
    .WillRepeatedly(Return(player_1));

    EXPECT_CALL(DB, get_game_details(_))
    .WillRepeatedly(Return(chess));

    crow::request req;
    crow::response res;
    crow::json::wvalue body;

    // Test: No Player Emails Given
    body = {
        {"matchmaking_type", "basic"},
        {"game_id", "1"}
    };
    req.body = body.dump();
    req.add_header("Authorization", "VALID TOKEN");
    res =  api.matchmake(req, &M);
    ASSERT_EQ(res.code, 400);
    ASSERT_EQ(res.body, "Incorrect Request Format.\n");

    // Test: Given Game ID does not Belong to Developer
    body = {
        {"matchmaking_type", "basic"},
        {"game_id", "-1"},
        {"player_emails", "[\"player1@gmail.com\"]"}
    };
    req.body = body.dump();
    req.add_header("Authorization", "VALID TOKEN");
    res =  api.matchmake(req, &M);
    ASSERT_EQ(res.code, 400);
    ASSERT_EQ(res.body, "Given Game ID does not belong to the Given Developer.\n");

    // Test: Passed a non-existent player email
    body = {
        {"matchmaking_type", "basic"},
        {"game_id", "1"}
    };
    std::vector<std::string> player_emails_1;
    player_emails_1.push_back("player_1@gmail.com");
    player_emails_1.push_back("player_5@gmail.com");
    body["player_emails"] = player_emails_1;
    req.body = body.dump();
    req.add_header("Authorization", "VALID TOKEN");
    res =  api.matchmake(req, &M);
    ASSERT_EQ(res.code, 400);
    ASSERT_EQ(res.body, "The following player IDs were not found: player_5@gmail.com\n");

    // Test: Passed a repeat player email
    body = {
        {"matchmaking_type", "basic"},
        {"game_id", "1"}
    };
    std::vector<std::string> player_emails_2;
    player_emails_2.push_back("player_1@gmail.com");
    player_emails_2.push_back("player_1@gmail.com");
    player_emails_2.push_back("player_1@gmail.com");
    player_emails_2.push_back("player_2@gmail.com");
    body["player_emails"] = player_emails_2;
    req.body = body.dump();
    req.add_header("Authorization", "VALID TOKEN");
    res =  api.matchmake(req, &M);
    ASSERT_EQ(res.code, 400);
    ASSERT_EQ(res.body, "The following player IDs were found multiple times in the input: player_1@gmail.com\n");

    // Test: No Matchmaking Type Given
    body = {
        {"developer_email", "developer@gmail.com"},
        {"game_id", "1"},
    };
    std::vector<std::string> player_emails_4;
    player_emails_4.push_back("player_1@gmail.com");
    player_emails_4.push_back("player_2@gmail.com");
    body["player_emails"] = player_emails_4;
    req.body = body.dump();
    res =  api.matchmake(req, &M);
    ASSERT_EQ(res.code, 400);
    ASSERT_EQ(res.body, "Incorrect Request Format.\n");

    // Test: Basic Matchmaking Request Body
    body = {
        {"matchmaking_type", "basic"},
        {"game_id", "1"}
    };
    std::vector<std::string> player_emails_3;
    player_emails_3.push_back("player_1@gmail.com");
    player_emails_3.push_back("player_2@gmail.com");
    body["player_emails"] = player_emails_3;
    req.body = body.dump();
    req.add_header("Authorization", "VALID TOKEN");
    res =  api.matchmake(req, &M);
    ASSERT_EQ(res.code, 200);
}

TEST(MatchmakingTests, Matchmaking_Backend_Tests_Set1) {
    Matchmaking matchmaking;

    MockDBService DB;

    Game_Details chess;
    chess.game_parameter1_weight = 1;
    chess.game_parameter1_weight = 0;
    chess.game_parameter1_weight = 0;
    chess.game_parameter1_weight = 0;
    chess.players_per_team = 1;
    chess.teams_per_match = 2;
    chess.category = "Strategy";

    Player_Game_Ratings player_1;
    player_1.game_parameter1_value = 2000;
    player_1.game_parameter2_value = 0;
    player_1.game_parameter3_value = 0;
    player_1.game_parameter4_value = 0;
    player_1.is_valid = true;

    Player_Game_Ratings player_2;
    player_2.game_parameter1_value = 1000;
    player_2.game_parameter2_value = 0;
    player_2.game_parameter3_value = 0;
    player_2.game_parameter4_value = 0;
    player_2.is_valid = true;

    EXPECT_CALL(DB, get_game_details(_))
    .WillRepeatedly(Return(chess));

    EXPECT_CALL(DB, get_player_game_rating(_, _))
    .Times(2)
    .WillOnce(Return(player_1))
    .WillOnce(Return(player_2));

    std::tuple<
    std::vector<std::vector<std::vector<std::string> > >,
    std::vector<std::string> > backend_result;

    std::vector<std::vector<std::vector<std::string> > > lobbies;
    std::vector<std::vector<std::string> > games;
    std::vector<std::string> team1;
    std::vector<std::string> team2;

    team1.push_back("player_1@gmail.com");
    team2.push_back("player_2@gmail.com");
    games.push_back(team2);
    games.push_back(team1);
    lobbies.push_back(games);

    // Test: Chess with 2 Players
    int game_id = 1;
    std::vector<std::string> player_emails;
    player_emails.push_back("player_1@gmail.com");
    player_emails.push_back("player_2@gmail.com");
    backend_result = matchmaking.matchmakingBackendBasic(game_id, player_emails, &DB);

    std::vector<std::vector<std::vector<std::string> > > test_lobbies = std::get<0>(backend_result);
    for (uint64_t i = 0; i < lobbies.size(); i++)
        for (uint64_t j = 0; j < lobbies.at(i).size(); j++)
            for (uint64_t k = 0; k < lobbies.at(i).at(j).size(); k++)
                ASSERT_EQ(test_lobbies.at(i).at(j).at(k), lobbies.at(i).at(j).at(k));
}

TEST(MatchmakingTests, Matchmaking_Backend_Tests_Set2) {
    Matchmaking matchmaking;

    MockDBService DB;

    Game_Details chess;
    chess.game_parameter1_weight = 1;
    chess.game_parameter1_weight = 0;
    chess.game_parameter1_weight = 0;
    chess.game_parameter1_weight = 0;
    chess.players_per_team = 1;
    chess.teams_per_match = 2;
    chess.category = "Strategy";
    chess.is_valid = true;

    Player_Game_Ratings player_1;
    player_1.game_parameter1_value = 2000;
    player_1.game_parameter2_value = 0;
    player_1.game_parameter3_value = 0;
    player_1.game_parameter4_value = 0;
    player_1.is_valid = true;

    EXPECT_CALL(DB, get_game_details(_))
    .WillRepeatedly(Return(chess));

    EXPECT_CALL(DB, get_player_game_rating(_, _))
    .Times(1)
    .WillOnce(Return(player_1));

    std::tuple<
    std::vector<std::vector<std::vector<std::string> > >,
    std::vector<std::string> > backend_result;

    std::vector<std::string> overflow;

    overflow.push_back("player_1@gmail.com");

    // Test: Basic Chess with 1 Player
    int game_id = 1;
    std::vector<std::string> player_emails;
    player_emails.push_back("player_1@gmail.com");
    backend_result = matchmaking.matchmakingBackendBasic(game_id, player_emails, &DB);

    std::vector<std::string> test_overflow = std::get<1>(backend_result);
    for (uint64_t i = 0; i < overflow.size(); i++)
        ASSERT_EQ(test_overflow.at(i), overflow.at(i));
}

TEST(MatchmakingTests, Matchmaking_Backend_Tests_Set3) {
    Matchmaking matchmaking;

    MockDBService DB;

    Game_Details chess;
    chess.game_parameter1_weight = 1;
    chess.game_parameter1_weight = 0;
    chess.game_parameter1_weight = 0;
    chess.game_parameter1_weight = 0;
    chess.players_per_team = 1;
    chess.teams_per_match = 2;
    chess.category = "Strategy";
    chess.is_valid = true;

    Player_Game_Ratings player_1;
    player_1.game_parameter1_value = 2000;
    player_1.game_parameter2_value = 0;
    player_1.game_parameter3_value = 0;
    player_1.game_parameter4_value = 0;
    player_1.is_valid = true;

    Player_Game_Ratings player_2;
    player_2.game_parameter1_value = 1000;
    player_2.game_parameter2_value = 0;
    player_2.game_parameter3_value = 0;
    player_2.game_parameter4_value = 0;
    player_2.is_valid = true;

    Player_Game_Ratings player_3;
    player_3.game_parameter1_value = 0;
    player_3.game_parameter2_value = 0;
    player_3.game_parameter3_value = 0;
    player_3.game_parameter4_value = 0;
    player_3.is_valid = true;

    EXPECT_CALL(DB, get_game_details(_))
    .WillRepeatedly(Return(chess));

    EXPECT_CALL(DB, get_player_game_rating(_, _))
    .Times(3)
    .WillOnce(Return(player_1))
    .WillOnce(Return(player_2))
    .WillOnce(Return(player_3));

    std::tuple<
    std::vector<std::vector<std::vector<std::string> > >,
    std::vector<std::string> > backend_result;

    std::vector<std::vector<std::vector<std::string> > > lobbies;
    std::vector<std::vector<std::string> > games;
    std::vector<std::string> team1;
    std::vector<std::string> team2;

    team1.push_back("player_2@gmail.com");
    team2.push_back("player_3@gmail.com");
    games.push_back(team2);
    games.push_back(team1);
    lobbies.push_back(games);

    std::vector<std::string> overflow;
    overflow.push_back("player_1@gmail.com");

    // Test: Basic Chess with 3 Players
    int game_id = 1;
    std::vector<std::string> player_emails;
    player_emails.push_back("player_1@gmail.com");
    player_emails.push_back("player_2@gmail.com");
    player_emails.push_back("player_3@gmail.com");
    backend_result = matchmaking.matchmakingBackendBasic(game_id, player_emails, &DB);

    std::vector<std::vector<std::vector<std::string> > > test_lobbies = std::get<0>(backend_result);
    for (uint64_t i = 0; i < lobbies.size(); i++)
        for (uint64_t j = 0; j < lobbies.at(i).size(); j++)
            for (uint64_t k = 0; k < lobbies.at(i).at(j).size(); k++)
                ASSERT_EQ(test_lobbies.at(i).at(j).at(k), lobbies.at(i).at(j).at(k));

    std::vector<std::string> test_overflow = std::get<1>(backend_result);
    for (uint64_t i = 0; i < overflow.size(); i++)
        ASSERT_EQ(test_overflow.at(i), overflow.at(i));
}

TEST(MatchmakingTests, Matchmaking_Backend_Tests_Set4) {
    Matchmaking matchmaking;

    MockDBService DB;

    Game_Details chess;
    chess.game_parameter1_weight = 1;
    chess.game_parameter1_weight = 0;
    chess.game_parameter1_weight = 0;
    chess.game_parameter1_weight = 0;
    chess.players_per_team = 1;
    chess.teams_per_match = 2;
    chess.category = "Strategy";
    chess.is_valid = true;

    Game_Details checkers;
    checkers.game_parameter1_weight = 1;
    checkers.game_parameter1_weight = 0;
    checkers.game_parameter1_weight = 0;
    checkers.game_parameter1_weight = 0;
    checkers.players_per_team = 1;
    checkers.teams_per_match = 2;
    checkers.category = "Strategy";
    checkers.is_valid = true;

    std::vector<Game_Details> all_games;
    all_games.push_back(chess);
    all_games.push_back(checkers);

    Player_Game_Ratings player_1_chess;
    player_1_chess.game_parameter1_value = 2000;
    player_1_chess.game_parameter2_value = 0;
    player_1_chess.game_parameter3_value = 0;
    player_1_chess.game_parameter4_value = 0;
    player_1_chess.is_valid = true;

    Player_Game_Ratings player_2_chess;
    player_2_chess.game_parameter1_value = 1000;
    player_2_chess.game_parameter2_value = 0;
    player_2_chess.game_parameter3_value = 0;
    player_2_chess.game_parameter4_value = 0;
    player_2_chess.is_valid = true;

    Player_Game_Ratings null_player;
    null_player.is_valid = false;

    EXPECT_CALL(DB, get_game_details(_))
    .WillRepeatedly(Return(chess));

    EXPECT_CALL(DB, get_all_games())
    .WillOnce(Return(all_games));

    EXPECT_CALL(DB, get_player_game_rating(_, _))
    .WillOnce(Return(player_1_chess))
    .WillOnce(Return(player_2_chess))
    .WillRepeatedly(Return(null_player));

    std::tuple<
    std::vector<std::vector<std::vector<std::string> > >,
    std::vector<std::string> > backend_result;

    std::vector<std::vector<std::vector<std::string> > > lobbies;
    std::vector<std::vector<std::string> > games;
    std::vector<std::string> team1;
    std::vector<std::string> team2;

    team1.push_back("player_1@gmail.com");
    team2.push_back("player_2@gmail.com");
    games.push_back(team1);
    games.push_back(team2);
    lobbies.push_back(games);

    // Test: Advanced Chess with 2 Players
    // -- Both only play Chess
    int game_id = 1;
    std::vector<std::string> player_emails;
    player_emails.push_back("player_1@gmail.com");
    player_emails.push_back("player_2@gmail.com");
    backend_result = matchmaking.matchmakingBackendAdvanced(game_id, player_emails, &DB);

    std::vector<std::vector<std::vector<std::string> > > test_lobbies = std::get<0>(backend_result);
    for (uint64_t i = 0; i < lobbies.size(); i++)
        for (uint64_t j = 0; j < lobbies.at(i).size(); j++)
            for (uint64_t k = 0; k < lobbies.at(i).at(j).size(); k++)
                ASSERT_EQ(test_lobbies.at(i).at(j).at(k), lobbies.at(i).at(j).at(k));
}

TEST(MatchmakingTests, Matchmaking_Backend_Tests_Set5) {
    Matchmaking matchmaking;

    MockDBService DB;

    Game_Details chess;
    chess.game_parameter1_weight = 1;
    chess.game_parameter1_weight = 0;
    chess.game_parameter1_weight = 0;
    chess.game_parameter1_weight = 0;
    chess.players_per_team = 1;
    chess.teams_per_match = 2;
    chess.category = "Strategy";
    chess.is_valid = true;

    Game_Details checkers;
    checkers.game_parameter1_weight = 1;
    checkers.game_parameter1_weight = 0;
    checkers.game_parameter1_weight = 0;
    checkers.game_parameter1_weight = 0;
    checkers.players_per_team = 1;
    checkers.teams_per_match = 2;
    checkers.category = "Strategy";
    checkers.is_valid = true;

    std::vector<Game_Details> all_games;
    all_games.push_back(chess);
    all_games.push_back(checkers);

    Player_Game_Ratings player_1_chess;
    player_1_chess.game_parameter1_value = 2000;
    player_1_chess.game_parameter2_value = 0;
    player_1_chess.game_parameter3_value = 0;
    player_1_chess.game_parameter4_value = 0;
    player_1_chess.is_valid = true;

    Player_Game_Ratings player_1_checkers;
    player_1_checkers.game_parameter1_value = 2000;
    player_1_checkers.game_parameter2_value = 0;
    player_1_checkers.game_parameter3_value = 0;
    player_1_checkers.game_parameter4_value = 0;
    player_1_checkers.is_valid = true;

    Player_Game_Ratings player_2_chess;
    player_2_chess.game_parameter1_value = 1000;
    player_2_chess.game_parameter2_value = 0;
    player_2_chess.game_parameter3_value = 0;
    player_2_chess.game_parameter4_value = 0;
    player_2_chess.is_valid = true;

    Player_Game_Ratings null_player;
    null_player.is_valid = false;

    EXPECT_CALL(DB, get_game_details(_))
    .WillRepeatedly(Return(chess));

    EXPECT_CALL(DB, get_all_games())
    .WillOnce(Return(all_games));

    EXPECT_CALL(DB, get_player_game_rating(_, _))
    .WillOnce(Return(player_1_chess))
    .WillOnce(Return(player_2_chess))
    .WillOnce(Return(player_1_checkers))
    .WillOnce(Return(null_player));

    std::tuple<
    std::vector<std::vector<std::vector<std::string> > >,
    std::vector<std::string> > backend_result;

    std::vector<std::vector<std::vector<std::string> > > lobbies;
    std::vector<std::vector<std::string> > games;
    std::vector<std::string> team1;
    std::vector<std::string> team2;

    team1.push_back("player_1@gmail.com");
    team2.push_back("player_2@gmail.com");
    games.push_back(team1);
    games.push_back(team2);
    lobbies.push_back(games);

    // Test: Advanced Chess with 2 Players
    // -- Player 1 Plays Chess and Checkers
    int game_id = 1;
    std::vector<std::string> player_emails;
    player_emails.push_back("player_1@gmail.com");
    player_emails.push_back("player_2@gmail.com");
    backend_result = matchmaking.matchmakingBackendAdvanced(game_id, player_emails, &DB);

    std::vector<std::vector<std::vector<std::string> > > test_lobbies = std::get<0>(backend_result);
    for (uint64_t i = 0; i < lobbies.size(); i++)
        for (uint64_t j = 0; j < lobbies.at(i).size(); j++)
            for (uint64_t k = 0; k < lobbies.at(i).at(j).size(); k++)
                ASSERT_EQ(test_lobbies.at(i).at(j).at(k), lobbies.at(i).at(j).at(k));
}

TEST(MatchmakingTests, Matchmaking_Backend_Tests_Set6) {
    Matchmaking matchmaking;

    MockDBService DB;

    Game_Details chess;
    chess.game_parameter1_weight = 1;
    chess.game_parameter1_weight = 0;
    chess.game_parameter1_weight = 0;
    chess.game_parameter1_weight = 0;
    chess.players_per_team = 1;
    chess.teams_per_match = 2;
    chess.category = "Strategy";
    chess.is_valid = true;

    Game_Details checkers;
    checkers.game_parameter1_weight = 1;
    checkers.game_parameter1_weight = 0;
    checkers.game_parameter1_weight = 0;
    checkers.game_parameter1_weight = 0;
    checkers.players_per_team = 1;
    checkers.teams_per_match = 2;
    checkers.category = "Strategy";
    checkers.is_valid = true;

    std::vector<Game_Details> all_games;
    all_games.push_back(chess);
    all_games.push_back(checkers);

    Player_Game_Ratings player_1_chess;
    player_1_chess.game_parameter1_value = 2000;
    player_1_chess.game_parameter2_value = 0;
    player_1_chess.game_parameter3_value = 0;
    player_1_chess.game_parameter4_value = 0;
    player_1_chess.is_valid = true;

    Player_Game_Ratings player_2_chess;
    player_2_chess.game_parameter1_value = 1000;
    player_2_chess.game_parameter2_value = 0;
    player_2_chess.game_parameter3_value = 0;
    player_2_chess.game_parameter4_value = 0;
    player_2_chess.is_valid = true;

    Player_Game_Ratings player_2_checkers;
    player_2_checkers.game_parameter1_value = 2000;
    player_2_checkers.game_parameter2_value = 0;
    player_2_checkers.game_parameter3_value = 0;
    player_2_checkers.game_parameter4_value = 0;
    player_2_checkers.is_valid = true;

    Player_Game_Ratings null_player;
    null_player.is_valid = false;

    EXPECT_CALL(DB, get_game_details(_))
    .WillRepeatedly(Return(chess));

    EXPECT_CALL(DB, get_all_games())
    .WillOnce(Return(all_games));

    EXPECT_CALL(DB, get_player_game_rating(_, _))
    .WillOnce(Return(player_1_chess))
    .WillOnce(Return(player_2_chess))
    .WillOnce(Return(null_player))
    .WillOnce(Return(player_2_checkers));

    std::tuple<
    std::vector<std::vector<std::vector<std::string> > >,
    std::vector<std::string> > backend_result;

    std::vector<std::vector<std::vector<std::string> > > lobbies;
    std::vector<std::vector<std::string> > games;
    std::vector<std::string> team1;
    std::vector<std::string> team2;

    team1.push_back("player_1@gmail.com");
    team2.push_back("player_2@gmail.com");
    games.push_back(team1);
    games.push_back(team2);
    lobbies.push_back(games);

    // Test: Advanced Chess with 2 Players
    // -- Player 2 Plays Chess and Checkers, not Player 1
    int game_id = 1;
    std::vector<std::string> player_emails;
    player_emails.push_back("player_1@gmail.com");
    player_emails.push_back("player_2@gmail.com");
    backend_result = matchmaking.matchmakingBackendAdvanced(game_id, player_emails, &DB);

    std::vector<std::vector<std::vector<std::string> > > test_lobbies = std::get<0>(backend_result);
    for (uint64_t i = 0; i < lobbies.size(); i++)
        for (uint64_t j = 0; j < lobbies.at(i).size(); j++)
            for (uint64_t k = 0; k < lobbies.at(i).at(j).size(); k++)
                ASSERT_EQ(test_lobbies.at(i).at(j).at(k), lobbies.at(i).at(j).at(k));
}

TEST(MatchmakingTests, Matchmaking_Backend_Tests_Set7) {
    Matchmaking matchmaking;

    MockDBService DB;

    Game_Details chess;
    chess.game_parameter1_weight = 1;
    chess.game_parameter1_weight = 0;
    chess.game_parameter1_weight = 0;
    chess.game_parameter1_weight = 0;
    chess.players_per_team = 1;
    chess.teams_per_match = 2;
    chess.category = "Strategy";
    chess.is_valid = true;

    Game_Details checkers;
    checkers.game_parameter1_weight = 1;
    checkers.game_parameter1_weight = 0;
    checkers.game_parameter1_weight = 0;
    checkers.game_parameter1_weight = 0;
    checkers.players_per_team = 1;
    checkers.teams_per_match = 2;
    checkers.category = "Strategy";
    checkers.is_valid = true;

    std::vector<Game_Details> all_games;
    all_games.push_back(chess);
    all_games.push_back(checkers);

    Player_Game_Ratings player_1_chess;
    player_1_chess.game_parameter1_value = 2000;
    player_1_chess.game_parameter2_value = 0;
    player_1_chess.game_parameter3_value = 0;
    player_1_chess.game_parameter4_value = 0;
    player_1_chess.is_valid = true;

    Player_Game_Ratings player_1_checkers;
    player_1_checkers.game_parameter1_value = 500;
    player_1_checkers.game_parameter2_value = 0;
    player_1_checkers.game_parameter3_value = 0;
    player_1_checkers.game_parameter4_value = 0;
    player_1_checkers.is_valid = true;

    Player_Game_Ratings player_2_chess;
    player_2_chess.game_parameter1_value = 1000;
    player_2_chess.game_parameter2_value = 0;
    player_2_chess.game_parameter3_value = 0;
    player_2_chess.game_parameter4_value = 0;
    player_2_chess.is_valid = true;

    Player_Game_Ratings player_2_checkers;
    player_2_checkers.game_parameter1_value = 2000;
    player_2_checkers.game_parameter2_value = 0;
    player_2_checkers.game_parameter3_value = 0;
    player_2_checkers.game_parameter4_value = 0;
    player_2_checkers.is_valid = true;

    EXPECT_CALL(DB, get_game_details(_))
    .WillRepeatedly(Return(chess));

    EXPECT_CALL(DB, get_all_games())
    .WillOnce(Return(all_games));

    EXPECT_CALL(DB, get_player_game_rating(_, _))
    .WillOnce(Return(player_1_chess))
    .WillOnce(Return(player_2_chess))
    .WillOnce(Return(player_1_checkers))
    .WillOnce(Return(player_2_checkers));

    std::tuple<
    std::vector<std::vector<std::vector<std::string> > >,
    std::vector<std::string> > backend_result;

    std::vector<std::vector<std::vector<std::string> > > lobbies;
    std::vector<std::vector<std::string> > games;
    std::vector<std::string> team1;
    std::vector<std::string> team2;

    team1.push_back("player_1@gmail.com");
    team2.push_back("player_2@gmail.com");
    games.push_back(team1);
    games.push_back(team2);
    lobbies.push_back(games);

    // Test: Advanced Chess with 2 Players
    // -- Player 2 Plays Chess and Checkers, not Player 1
    int game_id = 1;
    std::vector<std::string> player_emails;
    player_emails.push_back("player_1@gmail.com");
    player_emails.push_back("player_2@gmail.com");
    backend_result = matchmaking.matchmakingBackendAdvanced(game_id, player_emails, &DB);

    std::vector<std::vector<std::vector<std::string> > > test_lobbies = std::get<0>(backend_result);
    for (uint64_t i = 0; i < lobbies.size(); i++)
        for (uint64_t j = 0; j < lobbies.at(i).size(); j++)
            for (uint64_t k = 0; k < lobbies.at(i).at(j).size(); k++)
                ASSERT_EQ(test_lobbies.at(i).at(j).at(k), lobbies.at(i).at(j).at(k));
}

TEST(MatchmakingTests, Matchmaking_Backend_Tests_Set8) {
    Matchmaking matchmaking;

    MockDBService DB;

    Game_Details chess;
    chess.game_parameter1_weight = 1;
    chess.game_parameter1_weight = 0;
    chess.game_parameter1_weight = 0;
    chess.game_parameter1_weight = 0;
    chess.players_per_team = 1;
    chess.teams_per_match = 2;
    chess.category = "Strategy";
    chess.is_valid = true;

    Game_Details checkers;
    checkers.game_parameter1_weight = 1;
    checkers.game_parameter1_weight = 0;
    checkers.game_parameter1_weight = 0;
    checkers.game_parameter1_weight = 0;
    checkers.players_per_team = 1;
    checkers.teams_per_match = 2;
    checkers.category = "Strategy";
    checkers.category = "Strategy";
    checkers.is_valid = true;

    std::vector<Game_Details> all_games;
    all_games.push_back(chess);
    all_games.push_back(checkers);

    Player_Game_Ratings player_1_chess;
    player_1_chess.game_parameter1_value = 2000;
    player_1_chess.game_parameter2_value = 0;
    player_1_chess.game_parameter3_value = 0;
    player_1_chess.game_parameter4_value = 0;
    player_1_chess.is_valid = true;

    Player_Game_Ratings player_1_checkers;
    player_1_checkers.game_parameter1_value = 500;
    player_1_checkers.game_parameter2_value = 0;
    player_1_checkers.game_parameter3_value = 0;
    player_1_checkers.game_parameter4_value = 0;
    player_1_checkers.is_valid = true;

    Player_Game_Ratings player_2_chess;
    player_2_chess.game_parameter1_value = 1000;
    player_2_chess.game_parameter2_value = 0;
    player_2_chess.game_parameter3_value = 0;
    player_2_chess.game_parameter4_value = 0;
    player_2_chess.is_valid = true;

    Player_Game_Ratings player_2_checkers;
    player_2_checkers.game_parameter1_value = 2000;
    player_2_checkers.game_parameter2_value = 0;
    player_2_checkers.game_parameter3_value = 0;
    player_2_checkers.game_parameter4_value = 0;
    player_2_checkers.is_valid = true;

    Player_Game_Ratings player_3_chess;
    player_3_chess.game_parameter1_value = 1;
    player_3_chess.game_parameter2_value = 0;
    player_3_chess.game_parameter3_value = 0;
    player_3_chess.game_parameter4_value = 0;
    player_3_chess.is_valid = true;

    Player_Game_Ratings null_player;
    null_player.is_valid = false;

    EXPECT_CALL(DB, get_game_details(_))
    .WillRepeatedly(Return(chess));

    EXPECT_CALL(DB, get_all_games())
    .WillOnce(Return(all_games));

    EXPECT_CALL(DB, get_player_game_rating(_, _))
    .WillOnce(Return(player_1_chess))
    .WillOnce(Return(player_2_chess))
    .WillOnce(Return(player_3_chess))
    .WillOnce(Return(player_1_checkers))
    .WillOnce(Return(player_2_checkers))
    .WillOnce(Return(null_player));

    std::tuple<
    std::vector<std::vector<std::vector<std::string> > >,
    std::vector<std::string> > backend_result;

    std::vector<std::vector<std::vector<std::string> > > lobbies;
    std::vector<std::vector<std::string> > games;
    std::vector<std::string> team1;
    std::vector<std::string> team2;

    team1.push_back("player_1@gmail.com");
    team2.push_back("player_2@gmail.com");
    games.push_back(team1);
    games.push_back(team2);
    lobbies.push_back(games);

    std::vector<std::string> overflow;
    overflow.push_back("player_3@gmail.com");

    // Test: Advanced Chess with 3 Players
    int game_id = 1;
    std::vector<std::string> player_emails;
    player_emails.push_back("player_1@gmail.com");
    player_emails.push_back("player_2@gmail.com");
    player_emails.push_back("player_3@gmail.com");
    backend_result = matchmaking.matchmakingBackendAdvanced(game_id, player_emails, &DB);

    std::vector<std::vector<std::vector<std::string> > > test_lobbies = std::get<0>(backend_result);
    for (uint64_t i = 0; i < lobbies.size(); i++)
        for (uint64_t j = 0; j < lobbies.at(i).size(); j++)
            for (uint64_t k = 0; k < lobbies.at(i).at(j).size(); k++)
                ASSERT_EQ(test_lobbies.at(i).at(j).at(k), lobbies.at(i).at(j).at(k));

    std::vector<std::string> test_overflow = std::get<1>(backend_result);
    for (uint64_t i = 0; i < overflow.size(); i++)
        ASSERT_EQ(test_overflow.at(i), overflow.at(i));
}

TEST(MatchmakingTests, Matchmaking_Backend_Tests_Set9) {
    Matchmaking matchmaking;

    MockDBService DB;

    Game_Details chess;
    chess.game_parameter1_weight = 1;
    chess.game_parameter1_weight = 0;
    chess.game_parameter1_weight = 0;
    chess.game_parameter1_weight = 0;
    chess.players_per_team = 1;
    chess.teams_per_match = 2;
    chess.category = "Strategy";
    chess.is_valid = true;

    Game_Details checkers;
    checkers.game_parameter1_weight = 1;
    checkers.game_parameter1_weight = 0;
    checkers.game_parameter1_weight = 0;
    checkers.game_parameter1_weight = 0;
    checkers.players_per_team = 1;
    checkers.teams_per_match = 2;
    checkers.category = "Strategy";
    checkers.is_valid = true;

    std::vector<Game_Details> all_games;
    all_games.push_back(chess);
    all_games.push_back(checkers);

    Player_Game_Ratings player_1_chess;
    player_1_chess.game_parameter1_value = 2000;
    player_1_chess.game_parameter2_value = 0;
    player_1_chess.game_parameter3_value = 0;
    player_1_chess.game_parameter4_value = 0;
    player_1_chess.is_valid = true;

    Player_Game_Ratings player_1_checkers;
    player_1_checkers.game_parameter1_value = 500;
    player_1_checkers.game_parameter2_value = 0;
    player_1_checkers.game_parameter3_value = 0;
    player_1_checkers.game_parameter4_value = 0;
    player_1_checkers.is_valid = true;

    EXPECT_CALL(DB, get_game_details(_))
    .WillRepeatedly(Return(chess));

    EXPECT_CALL(DB, get_all_games())
    .WillOnce(Return(all_games));

    EXPECT_CALL(DB, get_player_game_rating(_, _))
    .WillOnce(Return(player_1_chess))
    .WillOnce(Return(player_1_checkers));

    std::tuple<
    std::vector<std::vector<std::vector<std::string> > >,
    std::vector<std::string> > backend_result;

    std::vector<std::string> overflow;
    overflow.push_back("player_1@gmail.com");

    // Test: Only 1 Player Passed to Advance Chess Matchmaking
    int game_id = 1;
    std::vector<std::string> player_emails;
    player_emails.push_back("player_1@gmail.com");
    backend_result = matchmaking.matchmakingBackendAdvanced(game_id, player_emails, &DB);

    std::vector<std::string> test_overflow = std::get<1>(backend_result);
    for (uint64_t i = 0; i < overflow.size(); i++)
        ASSERT_EQ(test_overflow.at(i), overflow.at(i));
}
