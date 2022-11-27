// "Copyright [year] <Copyright Owner>"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "main/db-service.h"
#include "api-endpoints/api-endpoints-lib.h"

using ::testing::_;
using ::testing::Return;


class MatchmakingTestFixture: public testing::Test {
 public:
    static DBService DB;
    static AuthService auth;
    static Matchmaking M;
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
      auth = AuthService();
      M = Matchmaking();
      api = APIEndPoints(&DB, &auth);
    }

    static void TearDownTestCase() {
      DB.tearDownDatabase();
    }

    void createGame(Game_Details g, std::string token, int *game_id) {
      crow::request req;
      crow::response res;
      crow::json::wvalue body;

        std::vector<std::string> parms;
        parms.push_back(g.game_parameter1_name);
        parms.push_back(g.game_parameter2_name);
        parms.push_back(g.game_parameter3_name);
        parms.push_back(g.game_parameter4_name);

        std::vector<float> weights;
        weights.push_back(g.game_parameter1_weight);
        weights.push_back(g.game_parameter2_weight);
        weights.push_back(g.game_parameter3_weight);
        weights.push_back(g.game_parameter4_weight);

        req.add_header("Authorization", token);
        body = {
            {"name", g.game_name},
            {"category", g.category},
            {"teams_per_match", g.teams_per_match},
            {"players_per_team", g.players_per_team}
        };
        body["parameters"] = parms;
        body["weights"] = weights;
        req.body = body.dump();
        std::pair <int, std::string> res_pair = api.postGames(req);
        ASSERT_EQ(res.code, res_pair.first);
        *game_id = std::stoi(res_pair.second);
    }

    void addPlayersStats(std::vector<Player_Game_Ratings> pgrs, std::string token, int game_id) {
        crow::request req;
        crow::response res;
        crow::json::wvalue body;
        req.add_header("Authorization", token);
        body = {};
        for (Player_Game_Ratings pgr : pgrs) {
            body[pgr.player_email]["game_parameter1_value"] = pgr.game_parameter1_value;
            body[pgr.player_email]["game_parameter2_value"] = pgr.game_parameter2_value;
            body[pgr.player_email]["game_parameter3_value"] = pgr.game_parameter3_value;
            body[pgr.player_email]["game_parameter4_value"] = pgr.game_parameter4_value;
        }
        req.body = body.dump();
        res = api.addPlayersStats(req, game_id);
        ASSERT_EQ(res.code, 200);
    }
};

DBService MatchmakingTestFixture::DB;
AuthService MatchmakingTestFixture::auth;
Matchmaking MatchmakingTestFixture::M;
APIEndPoints MatchmakingTestFixture::api;

TEST_F(MatchmakingTestFixture,  Matchmaking_Endpoint_Tests_Set1) {
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

TEST_F(MatchmakingTestFixture,  Matchmaking_Endpoint_Tests_Set2) {
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

    // Valid Game Creation
    Game_Details chess;
    chess.game_name = "Chess";
    chess.game_parameter1_name = "rating";
    chess.game_parameter2_name = "";
    chess.game_parameter3_name = "";
    chess.game_parameter4_name = "";
    chess.game_parameter1_weight = 1;
    chess.game_parameter2_weight = 0;
    chess.game_parameter3_weight = 0;
    chess.game_parameter4_weight = 0;
    chess.players_per_team = 1;
    chess.teams_per_match = 2;
    chess.category = "Strategy";
    createGame(chess, token, &chess.game_id);


    std::vector<Player_Game_Ratings> pgrs;
    Player_Game_Ratings pgr1;
    pgr1.player_email = "player_1@gmail.com";
    pgr1.game_id = chess.game_id;
    pgr1.game_parameter1_value = 2000;
    pgr1.game_parameter2_value = 0;
    pgr1.game_parameter3_value = 0;
    pgr1.game_parameter4_value = 0;
    pgrs.push_back(pgr1);

    Player_Game_Ratings pgr2;
    pgr2.player_email = "player_2@gmail.com";
    pgr2.game_id = chess.game_id;
    pgr2.game_parameter1_value = 2000;
    pgr2.game_parameter2_value = 0;
    pgr2.game_parameter3_value = 0;
    pgr2.game_parameter4_value = 0;
    pgrs.push_back(pgr2);

    Player_Game_Ratings pgr3;
    pgr3.player_email = "player_3@gmail.com";
    pgr3.game_id = chess.game_id;
    pgr3.game_parameter1_value = 2000;
    pgr3.game_parameter2_value = 0;
    pgr3.game_parameter3_value = 0;
    pgr3.game_parameter4_value = 0;
    pgrs.push_back(pgr3);

    Player_Game_Ratings pgr4;
    pgr4.player_email = "player_4@gmail.com";
    pgr4.game_id = chess.game_id;
    pgr4.game_parameter1_value = 2000;
    pgr4.game_parameter2_value = 0;
    pgr4.game_parameter3_value = 0;
    pgr4.game_parameter4_value = 0;
    pgrs.push_back(pgr4);


    addPlayersStats(pgrs, token, chess.game_id);

    // Test: No Player Emails Given
    body = {
        {"matchmaking_type", "basic"},
        {"game_id", "1"}
    };
    req.body = body.dump();
    req.add_header("Authorization", token);
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
    req.add_header("Authorization", token);
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
    req.add_header("Authorization", token);
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
    req.add_header("Authorization", token);
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
    req.add_header("Authorization", token);
    res =  api.matchmake(req, &M);
    ASSERT_EQ(res.code, 200);
}

TEST_F(MatchmakingTestFixture, Matchmaking_Backend_Tests_Set1) {
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

    // Valid Game Creation
    Game_Details chess;
    chess.game_name = "Chess";
    chess.game_parameter1_name = "rating";
    chess.game_parameter2_name = "";
    chess.game_parameter3_name = "";
    chess.game_parameter4_name = "";
    chess.game_parameter1_weight = 1;
    chess.game_parameter2_weight = 0;
    chess.game_parameter3_weight = 0;
    chess.game_parameter4_weight = 0;
    chess.players_per_team = 1;
    chess.teams_per_match = 2;
    chess.category = "Strategy";
    createGame(chess, token, &chess.game_id);


    std::vector<Player_Game_Ratings> pgrs;
    Player_Game_Ratings pgr1;
    pgr1.player_email = "player_1@gmail.com";
    pgr1.game_id = chess.game_id;
    pgr1.game_parameter1_value = 2000;
    pgr1.game_parameter2_value = 0;
    pgr1.game_parameter3_value = 0;
    pgr1.game_parameter4_value = 0;
    pgrs.push_back(pgr1);

    Player_Game_Ratings pgr2;
    pgr2.player_email = "player_2@gmail.com";
    pgr2.game_id = chess.game_id;
    pgr2.game_parameter1_value = 1000;
    pgr2.game_parameter2_value = 0;
    pgr2.game_parameter3_value = 0;
    pgr2.game_parameter4_value = 0;
    pgrs.push_back(pgr2);

    addPlayersStats(pgrs, token, chess.game_id);

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

    // Test: Chess with 2 Players
    int game_id = chess.game_id;
    std::vector<std::string> player_emails;
    player_emails.push_back("player_1@gmail.com");
    player_emails.push_back("player_2@gmail.com");
    backend_result = M.matchmakingBackendBasic(game_id, player_emails, &DB);

    std::vector<std::vector<std::vector<std::string> > > test_lobbies = std::get<0>(backend_result);
    for (uint64_t i = 0; i < lobbies.size(); i++)
        for (uint64_t j = 0; j < lobbies.at(i).size(); j++)
            for (uint64_t k = 0; k < lobbies.at(i).at(j).size(); k++)
                ASSERT_EQ(test_lobbies.at(i).at(j).at(k), lobbies.at(i).at(j).at(k));
}

TEST_F(MatchmakingTestFixture, Matchmaking_Backend_Tests_Set2) {
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

    // Valid Game Creation
    Game_Details chess;
    chess.game_name = "Chess";
    chess.game_parameter1_name = "rating";
    chess.game_parameter2_name = "";
    chess.game_parameter3_name = "";
    chess.game_parameter4_name = "";
    chess.game_parameter1_weight = 1;
    chess.game_parameter2_weight = 0;
    chess.game_parameter3_weight = 0;
    chess.game_parameter4_weight = 0;
    chess.players_per_team = 1;
    chess.teams_per_match = 2;
    chess.category = "Strategy";
    createGame(chess, token, &chess.game_id);


    std::vector<Player_Game_Ratings> pgrs;
    Player_Game_Ratings pgr1;
    pgr1.player_email = "player_1@gmail.com";
    pgr1.game_id = chess.game_id;
    pgr1.game_parameter1_value = 2000;
    pgr1.game_parameter2_value = 0;
    pgr1.game_parameter3_value = 0;
    pgr1.game_parameter4_value = 0;
    pgrs.push_back(pgr1);

    addPlayersStats(pgrs, token, chess.game_id);

    std::tuple<
    std::vector<std::vector<std::vector<std::string> > >,
    std::vector<std::string> > backend_result;

    std::vector<std::string> overflow;

    overflow.push_back("player_1@gmail.com");

    // Test: Basic Chess with 1 Player
    int game_id = chess.game_id;
    std::vector<std::string> player_emails;
    player_emails.push_back("player_1@gmail.com");
    backend_result = M.matchmakingBackendBasic(game_id, player_emails, &DB);

    std::vector<std::string> test_overflow = std::get<1>(backend_result);
    for (uint64_t i = 0; i < overflow.size(); i++)
        ASSERT_EQ(test_overflow.at(i), overflow.at(i));
}

TEST_F(MatchmakingTestFixture, Matchmaking_Backend_Tests_Set3) {
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

    // Valid Game Creation
    Game_Details chess;
    chess.game_name = "Chess";
    chess.game_parameter1_name = "rating";
    chess.game_parameter2_name = "";
    chess.game_parameter3_name = "";
    chess.game_parameter4_name = "";
    chess.game_parameter1_weight = 1;
    chess.game_parameter2_weight = 0;
    chess.game_parameter3_weight = 0;
    chess.game_parameter4_weight = 0;
    chess.players_per_team = 1;
    chess.teams_per_match = 2;
    chess.category = "Strategy";
    createGame(chess, token, &chess.game_id);


    std::vector<Player_Game_Ratings> pgrs;
    Player_Game_Ratings pgr1;
    pgr1.player_email = "player_1@gmail.com";
    pgr1.game_id = chess.game_id;
    pgr1.game_parameter1_value = 2000;
    pgr1.game_parameter2_value = 0;
    pgr1.game_parameter3_value = 0;
    pgr1.game_parameter4_value = 0;
    pgrs.push_back(pgr1);

    Player_Game_Ratings pgr2;
    pgr2.player_email = "player_2@gmail.com";
    pgr2.game_id = chess.game_id;
    pgr2.game_parameter1_value = 1000;
    pgr2.game_parameter2_value = 0;
    pgr2.game_parameter3_value = 0;
    pgr2.game_parameter4_value = 0;
    pgrs.push_back(pgr2);

    Player_Game_Ratings pgr3;
    pgr3.player_email = "player_3@gmail.com";
    pgr3.game_id = chess.game_id;
    pgr3.game_parameter1_value = 0;
    pgr3.game_parameter2_value = 0;
    pgr3.game_parameter3_value = 0;
    pgr3.game_parameter4_value = 0;
    pgrs.push_back(pgr3);

    addPlayersStats(pgrs, token, chess.game_id);

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

    // Test: Basic Chess with 3 Players
    int game_id = chess.game_id;
    std::vector<std::string> player_emails;
    player_emails.push_back("player_1@gmail.com");
    player_emails.push_back("player_2@gmail.com");
    player_emails.push_back("player_3@gmail.com");
    backend_result = M.matchmakingBackendBasic(game_id, player_emails, &DB);

    std::vector<std::vector<std::vector<std::string> > > test_lobbies = std::get<0>(backend_result);
    for (uint64_t i = 0; i < lobbies.size(); i++)
        for (uint64_t j = 0; j < lobbies.at(i).size(); j++)
            for (uint64_t k = 0; k < lobbies.at(i).at(j).size(); k++)
                ASSERT_EQ(test_lobbies.at(i).at(j).at(k), lobbies.at(i).at(j).at(k));

    std::vector<std::string> test_overflow = std::get<1>(backend_result);
    for (uint64_t i = 0; i < overflow.size(); i++)
        ASSERT_EQ(test_overflow.at(i), overflow.at(i));
}

TEST_F(MatchmakingTestFixture, Matchmaking_Backend_Tests_Set4) {
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

    // Valid Game Creation
    Game_Details chess;
    chess.game_name = "Chess";
    chess.game_parameter1_name = "rating";
    chess.game_parameter2_name = "";
    chess.game_parameter3_name = "";
    chess.game_parameter4_name = "";
    chess.game_parameter1_weight = 1;
    chess.game_parameter2_weight = 0;
    chess.game_parameter3_weight = 0;
    chess.game_parameter4_weight = 0;
    chess.players_per_team = 1;
    chess.teams_per_match = 2;
    chess.category = "Strategy";
    createGame(chess, token, &chess.game_id);

    // Valid Game Creation
    Game_Details checkers;
    checkers.game_name = "Checkers";
    checkers.game_parameter1_name = "rating";
    checkers.game_parameter2_name = "";
    checkers.game_parameter3_name = "";
    checkers.game_parameter4_name = "";
    checkers.game_parameter1_weight = 1;
    checkers.game_parameter2_weight = 0;
    checkers.game_parameter3_weight = 0;
    checkers.game_parameter4_weight = 0;
    checkers.players_per_team = 1;
    checkers.teams_per_match = 2;
    checkers.category = "Strategy";
    createGame(checkers, token, &checkers.game_id);

    std::vector<Player_Game_Ratings> pgrs;
    Player_Game_Ratings pgr1;
    pgr1.player_email = "player_1@gmail.com";
    pgr1.game_id = chess.game_id;
    pgr1.game_parameter1_value = 2000;
    pgr1.game_parameter2_value = 0;
    pgr1.game_parameter3_value = 0;
    pgr1.game_parameter4_value = 0;
    pgrs.push_back(pgr1);

    Player_Game_Ratings pgr2;
    pgr2.player_email = "player_2@gmail.com";
    pgr2.game_id = chess.game_id;
    pgr2.game_parameter1_value = 1000;
    pgr2.game_parameter2_value = 0;
    pgr2.game_parameter3_value = 0;
    pgr2.game_parameter4_value = 0;
    pgrs.push_back(pgr2);

    addPlayersStats(pgrs, token, chess.game_id);

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
    int game_id = chess.game_id;
    std::vector<std::string> player_emails;
    player_emails.push_back("player_1@gmail.com");
    player_emails.push_back("player_2@gmail.com");
    backend_result = M.matchmakingBackendAdvanced(game_id, player_emails, &DB);

    std::vector<std::vector<std::vector<std::string> > > test_lobbies = std::get<0>(backend_result);
    for (uint64_t i = 0; i < lobbies.size(); i++)
        for (uint64_t j = 0; j < lobbies.at(i).size(); j++)
            for (uint64_t k = 0; k < lobbies.at(i).at(j).size(); k++)
                ASSERT_EQ(test_lobbies.at(i).at(j).at(k), lobbies.at(i).at(j).at(k));
}

TEST_F(MatchmakingTestFixture, Matchmaking_Backend_Tests_Set5) {
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

    // Valid Game Creation
    Game_Details chess;
    chess.game_name = "Chess";
    chess.game_parameter1_name = "rating";
    chess.game_parameter2_name = "";
    chess.game_parameter3_name = "";
    chess.game_parameter4_name = "";
    chess.game_parameter1_weight = 1;
    chess.game_parameter2_weight = 0;
    chess.game_parameter3_weight = 0;
    chess.game_parameter4_weight = 0;
    chess.players_per_team = 1;
    chess.teams_per_match = 2;
    chess.category = "Strategy";
    createGame(chess, token, &chess.game_id);

    // Valid Game Creation
    Game_Details checkers;
    checkers.game_name = "Checkers";
    checkers.game_parameter1_name = "rating";
    checkers.game_parameter2_name = "";
    checkers.game_parameter3_name = "";
    checkers.game_parameter4_name = "";
    checkers.game_parameter1_weight = 1;
    checkers.game_parameter2_weight = 0;
    checkers.game_parameter3_weight = 0;
    checkers.game_parameter4_weight = 0;
    checkers.players_per_team = 1;
    checkers.teams_per_match = 2;
    checkers.category = "Strategy";
    createGame(checkers, token, &checkers.game_id);

    std::vector<Player_Game_Ratings> pgrs;
    Player_Game_Ratings pgr1;
    pgr1.player_email = "player_1@gmail.com";
    pgr1.game_id = chess.game_id;
    pgr1.game_parameter1_value = 2000;
    pgr1.game_parameter2_value = 0;
    pgr1.game_parameter3_value = 0;
    pgr1.game_parameter4_value = 0;
    pgrs.push_back(pgr1);

    Player_Game_Ratings pgr2;
    pgr2.player_email = "player_2@gmail.com";
    pgr2.game_id = chess.game_id;
    pgr2.game_parameter1_value = 1000;
    pgr2.game_parameter2_value = 0;
    pgr2.game_parameter3_value = 0;
    pgr2.game_parameter4_value = 0;
    pgrs.push_back(pgr2);

    addPlayersStats(pgrs, token, chess.game_id);

    pgrs.clear();
    pgr1.player_email = "player_1@gmail.com";
    pgr1.game_id = checkers.game_id;
    pgr1.game_parameter1_value = 2000;
    pgr1.game_parameter2_value = 0;
    pgr1.game_parameter3_value = 0;
    pgr1.game_parameter4_value = 0;
    pgrs.push_back(pgr1);

    addPlayersStats(pgrs, token, checkers.game_id);

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
    int game_id = chess.game_id;
    std::vector<std::string> player_emails;
    player_emails.push_back("player_1@gmail.com");
    player_emails.push_back("player_2@gmail.com");
    backend_result = M.matchmakingBackendAdvanced(game_id, player_emails, &DB);

    std::vector<std::vector<std::vector<std::string> > > test_lobbies = std::get<0>(backend_result);
    for (uint64_t i = 0; i < lobbies.size(); i++)
        for (uint64_t j = 0; j < lobbies.at(i).size(); j++)
            for (uint64_t k = 0; k < lobbies.at(i).at(j).size(); k++)
                ASSERT_EQ(test_lobbies.at(i).at(j).at(k), lobbies.at(i).at(j).at(k));
}

TEST_F(MatchmakingTestFixture, Matchmaking_Backend_Tests_Set6) {
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

    // Valid Game Creation
    Game_Details chess;
    chess.game_name = "Chess";
    chess.game_parameter1_name = "rating";
    chess.game_parameter2_name = "";
    chess.game_parameter3_name = "";
    chess.game_parameter4_name = "";
    chess.game_parameter1_weight = 1;
    chess.game_parameter2_weight = 0;
    chess.game_parameter3_weight = 0;
    chess.game_parameter4_weight = 0;
    chess.players_per_team = 1;
    chess.teams_per_match = 2;
    chess.category = "Strategy";
    createGame(chess, token, &chess.game_id);

    // Valid Game Creation
    Game_Details checkers;
    checkers.game_name = "Checkers";
    checkers.game_parameter1_name = "rating";
    checkers.game_parameter2_name = "";
    checkers.game_parameter3_name = "";
    checkers.game_parameter4_name = "";
    checkers.game_parameter1_weight = 1;
    checkers.game_parameter2_weight = 0;
    checkers.game_parameter3_weight = 0;
    checkers.game_parameter4_weight = 0;
    checkers.players_per_team = 1;
    checkers.teams_per_match = 2;
    checkers.category = "Strategy";
    createGame(checkers, token, &checkers.game_id);

    std::vector<Player_Game_Ratings> pgrs;
    Player_Game_Ratings pgr1;
    pgr1.player_email = "player_1@gmail.com";
    pgr1.game_id = chess.game_id;
    pgr1.game_parameter1_value = 2000;
    pgr1.game_parameter2_value = 0;
    pgr1.game_parameter3_value = 0;
    pgr1.game_parameter4_value = 0;
    pgrs.push_back(pgr1);

    Player_Game_Ratings pgr2;
    pgr2.player_email = "player_2@gmail.com";
    pgr2.game_id = chess.game_id;
    pgr2.game_parameter1_value = 1000;
    pgr2.game_parameter2_value = 0;
    pgr2.game_parameter3_value = 0;
    pgr2.game_parameter4_value = 0;
    pgrs.push_back(pgr2);

    addPlayersStats(pgrs, token, chess.game_id);

    pgrs.clear();
    pgr2.player_email = "player_2@gmail.com";
    pgr2.game_id = checkers.game_id;
    pgr2.game_parameter1_value = 2000;
    pgr2.game_parameter2_value = 0;
    pgr2.game_parameter3_value = 0;
    pgr2.game_parameter4_value = 0;
    pgrs.push_back(pgr2);

    addPlayersStats(pgrs, token, checkers.game_id);

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
    int game_id = chess.game_id;
    std::vector<std::string> player_emails;
    player_emails.push_back("player_1@gmail.com");
    player_emails.push_back("player_2@gmail.com");
    backend_result = M.matchmakingBackendAdvanced(game_id, player_emails, &DB);

    std::vector<std::vector<std::vector<std::string> > > test_lobbies = std::get<0>(backend_result);
    for (uint64_t i = 0; i < lobbies.size(); i++)
        for (uint64_t j = 0; j < lobbies.at(i).size(); j++)
            for (uint64_t k = 0; k < lobbies.at(i).at(j).size(); k++)
                ASSERT_EQ(test_lobbies.at(i).at(j).at(k), lobbies.at(i).at(j).at(k));
}

TEST_F(MatchmakingTestFixture, Matchmaking_Backend_Tests_Set7) {
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

    // Valid Game Creation
    Game_Details chess;
    chess.game_name = "Chess";
    chess.game_parameter1_name = "rating";
    chess.game_parameter2_name = "";
    chess.game_parameter3_name = "";
    chess.game_parameter4_name = "";
    chess.game_parameter1_weight = 1;
    chess.game_parameter2_weight = 0;
    chess.game_parameter3_weight = 0;
    chess.game_parameter4_weight = 0;
    chess.players_per_team = 1;
    chess.teams_per_match = 2;
    chess.category = "Strategy";
    createGame(chess, token, &chess.game_id);

    // Valid Game Creation
    Game_Details checkers;
    checkers.game_name = "Checkers";
    checkers.game_parameter1_name = "rating";
    checkers.game_parameter2_name = "";
    checkers.game_parameter3_name = "";
    checkers.game_parameter4_name = "";
    checkers.game_parameter1_weight = 1;
    checkers.game_parameter2_weight = 0;
    checkers.game_parameter3_weight = 0;
    checkers.game_parameter4_weight = 0;
    checkers.players_per_team = 1;
    checkers.teams_per_match = 2;
    checkers.category = "Strategy";
    createGame(checkers, token, &checkers.game_id);

    std::vector<Player_Game_Ratings> pgrs;
    Player_Game_Ratings pgr1;
    pgr1.player_email = "player_1@gmail.com";
    pgr1.game_id = chess.game_id;
    pgr1.game_parameter1_value = 2000;
    pgr1.game_parameter2_value = 0;
    pgr1.game_parameter3_value = 0;
    pgr1.game_parameter4_value = 0;
    pgrs.push_back(pgr1);

    Player_Game_Ratings pgr2;
    pgr2.player_email = "player_2@gmail.com";
    pgr2.game_id = chess.game_id;
    pgr2.game_parameter1_value = 1000;
    pgr2.game_parameter2_value = 0;
    pgr2.game_parameter3_value = 0;
    pgr2.game_parameter4_value = 0;
    pgrs.push_back(pgr2);

    addPlayersStats(pgrs, token, chess.game_id);

    pgrs.clear();
    pgr1.player_email = "player_1@gmail.com";
    pgr1.game_id = checkers.game_id;
    pgr1.game_parameter1_value = 500;
    pgr1.game_parameter2_value = 0;
    pgr1.game_parameter3_value = 0;
    pgr1.game_parameter4_value = 0;
    pgrs.push_back(pgr1);

    pgr2.player_email = "player_2@gmail.com";
    pgr2.game_id = checkers.game_id;
    pgr2.game_parameter1_value = 2000;
    pgr2.game_parameter2_value = 0;
    pgr2.game_parameter3_value = 0;
    pgr2.game_parameter4_value = 0;
    pgrs.push_back(pgr2);

    addPlayersStats(pgrs, token, checkers.game_id);

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
    int game_id = chess.game_id;
    std::vector<std::string> player_emails;
    player_emails.push_back("player_1@gmail.com");
    player_emails.push_back("player_2@gmail.com");
    backend_result = M.matchmakingBackendAdvanced(game_id, player_emails, &DB);

    std::vector<std::vector<std::vector<std::string> > > test_lobbies = std::get<0>(backend_result);
    for (uint64_t i = 0; i < lobbies.size(); i++)
        for (uint64_t j = 0; j < lobbies.at(i).size(); j++)
            for (uint64_t k = 0; k < lobbies.at(i).at(j).size(); k++)
                ASSERT_EQ(test_lobbies.at(i).at(j).at(k), lobbies.at(i).at(j).at(k));
}

TEST_F(MatchmakingTestFixture, Matchmaking_Backend_Tests_Set8) {
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

    // Valid Game Creation
    Game_Details chess;
    chess.game_name = "Chess";
    chess.game_parameter1_name = "rating";
    chess.game_parameter2_name = "";
    chess.game_parameter3_name = "";
    chess.game_parameter4_name = "";
    chess.game_parameter1_weight = 1;
    chess.game_parameter2_weight = 0;
    chess.game_parameter3_weight = 0;
    chess.game_parameter4_weight = 0;
    chess.players_per_team = 1;
    chess.teams_per_match = 2;
    chess.category = "Strategy";
    createGame(chess, token, &chess.game_id);

    // Valid Game Creation
    Game_Details checkers;
    checkers.game_name = "Checkers";
    checkers.game_parameter1_name = "rating";
    checkers.game_parameter2_name = "";
    checkers.game_parameter3_name = "";
    checkers.game_parameter4_name = "";
    checkers.game_parameter1_weight = 1;
    checkers.game_parameter2_weight = 0;
    checkers.game_parameter3_weight = 0;
    checkers.game_parameter4_weight = 0;
    checkers.players_per_team = 1;
    checkers.teams_per_match = 2;
    checkers.category = "Strategy";
    createGame(checkers, token, &checkers.game_id);

    std::vector<Player_Game_Ratings> pgrs;
    Player_Game_Ratings pgr1;
    pgr1.player_email = "player_1@gmail.com";
    pgr1.game_id = chess.game_id;
    pgr1.game_parameter1_value = 400;
    pgr1.game_parameter2_value = 0;
    pgr1.game_parameter3_value = 0;
    pgr1.game_parameter4_value = 0;
    pgrs.push_back(pgr1);

    Player_Game_Ratings pgr2;
    pgr2.player_email = "player_2@gmail.com";
    pgr2.game_id = chess.game_id;
    pgr2.game_parameter1_value = 200;
    pgr2.game_parameter2_value = 0;
    pgr2.game_parameter3_value = 0;
    pgr2.game_parameter4_value = 0;
    pgrs.push_back(pgr2);

    Player_Game_Ratings pgr3;
    pgr3.player_email = "player_3@gmail.com";
    pgr3.game_id = chess.game_id;
    pgr3.game_parameter1_value = 1000;
    pgr3.game_parameter2_value = 0;
    pgr3.game_parameter3_value = 0;
    pgr3.game_parameter4_value = 0;
    pgrs.push_back(pgr3);

    addPlayersStats(pgrs, token, chess.game_id);

    pgrs.clear();
    pgr1.player_email = "player_1@gmail.com";
    pgr1.game_id = checkers.game_id;
    pgr1.game_parameter1_value = 50;
    pgr1.game_parameter2_value = 0;
    pgr1.game_parameter3_value = 0;
    pgr1.game_parameter4_value = 0;
    pgrs.push_back(pgr1);

    pgr2.player_email = "player_2@gmail.com";
    pgr2.game_id = checkers.game_id;
    pgr2.game_parameter1_value = 100;
    pgr2.game_parameter2_value = 0;
    pgr2.game_parameter3_value = 0;
    pgr2.game_parameter4_value = 0;
    pgrs.push_back(pgr2);

    pgr3.player_email = "player_3@gmail.com";
    pgr3.game_id = checkers.game_id;
    pgr3.game_parameter1_value = 50;
    pgr3.game_parameter2_value = 0;
    pgr3.game_parameter3_value = 0;
    pgr3.game_parameter4_value = 0;
    pgrs.push_back(pgr3);

    addPlayersStats(pgrs, token, checkers.game_id);

    std::tuple<
    std::vector<std::vector<std::vector<std::string> > >,
    std::vector<std::string> > backend_result;

    std::vector<std::vector<std::vector<std::string> > > lobbies;
    std::vector<std::vector<std::string> > games;
    std::vector<std::string> team1;
    std::vector<std::string> team2;

    team1.push_back("player_3@gmail.com");
    team2.push_back("player_2@gmail.com");
    games.push_back(team1);
    games.push_back(team2);
    lobbies.push_back(games);

    std::vector<std::string> overflow;
    overflow.push_back("player_1@gmail.com");

    // Test: Advanced Chess with 3 Players
    int game_id = chess.game_id;
    std::vector<std::string> player_emails;
    player_emails.push_back("player_1@gmail.com");
    player_emails.push_back("player_2@gmail.com");
    player_emails.push_back("player_3@gmail.com");
    backend_result = M.matchmakingBackendAdvanced(game_id, player_emails, &DB);

    std::vector<std::vector<std::vector<std::string> > > test_lobbies = std::get<0>(backend_result);
    for (uint64_t i = 0; i < lobbies.size(); i++)
        for (uint64_t j = 0; j < lobbies.at(i).size(); j++)
            for (uint64_t k = 0; k < lobbies.at(i).at(j).size(); k++)
                ASSERT_EQ(test_lobbies.at(i).at(j).at(k), lobbies.at(i).at(j).at(k));

    std::vector<std::string> test_overflow = std::get<1>(backend_result);
    for (uint64_t i = 0; i < overflow.size(); i++)
        ASSERT_EQ(test_overflow.at(i), overflow.at(i));
}

TEST_F(MatchmakingTestFixture, Matchmaking_Backend_Tests_Set9) {
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

    // Valid Game Creation
    Game_Details chess;
    chess.game_name = "Chess";
    chess.game_parameter1_name = "rating";
    chess.game_parameter2_name = "";
    chess.game_parameter3_name = "";
    chess.game_parameter4_name = "";
    chess.game_parameter1_weight = 1;
    chess.game_parameter2_weight = 0;
    chess.game_parameter3_weight = 0;
    chess.game_parameter4_weight = 0;
    chess.players_per_team = 1;
    chess.teams_per_match = 2;
    chess.category = "Strategy";
    createGame(chess, token, &chess.game_id);

    // Valid Game Creation
    Game_Details checkers;
    checkers.game_name = "Checkers";
    checkers.game_parameter1_name = "rating";
    checkers.game_parameter2_name = "";
    checkers.game_parameter3_name = "";
    checkers.game_parameter4_name = "";
    checkers.game_parameter1_weight = 1;
    checkers.game_parameter2_weight = 0;
    checkers.game_parameter3_weight = 0;
    checkers.game_parameter4_weight = 0;
    checkers.players_per_team = 1;
    checkers.teams_per_match = 2;
    checkers.category = "Strategy";
    createGame(checkers, token, &checkers.game_id);

    std::vector<Player_Game_Ratings> pgrs;
    Player_Game_Ratings pgr1;
    pgr1.player_email = "player_1@gmail.com";
    pgr1.game_id = chess.game_id;
    pgr1.game_parameter1_value = 400;
    pgr1.game_parameter2_value = 0;
    pgr1.game_parameter3_value = 0;
    pgr1.game_parameter4_value = 0;
    pgrs.push_back(pgr1);

    addPlayersStats(pgrs, token, chess.game_id);

    pgrs.clear();
    pgr1.player_email = "player_1@gmail.com";
    pgr1.game_id = checkers.game_id;
    pgr1.game_parameter1_value = 50;
    pgr1.game_parameter2_value = 0;
    pgr1.game_parameter3_value = 0;
    pgr1.game_parameter4_value = 0;
    pgrs.push_back(pgr1);

    addPlayersStats(pgrs, token, checkers.game_id);

    std::tuple<
    std::vector<std::vector<std::vector<std::string> > >,
    std::vector<std::string> > backend_result;

    std::vector<std::string> overflow;
    overflow.push_back("player_1@gmail.com");

    // Test: Only 1 Player Passed to Advance Chess Matchmaking
    int game_id = 1;
    std::vector<std::string> player_emails;
    player_emails.push_back("player_1@gmail.com");
    backend_result = M.matchmakingBackendAdvanced(game_id, player_emails, &DB);

    std::vector<std::string> test_overflow = std::get<1>(backend_result);
    for (uint64_t i = 0; i < overflow.size(); i++)
        ASSERT_EQ(test_overflow.at(i), overflow.at(i));
}
