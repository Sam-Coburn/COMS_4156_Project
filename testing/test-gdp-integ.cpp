// "Copyright 2022 RaisingCanesFanClub"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "api-endpoints/api-endpoints-lib.h"
#include "main/db-service.h"
#include "authentication/auth-service.h"

using ::testing::_;
using ::testing::Return;

class GameGDPTestIntegrationFixture : public testing::Test {
 public:
    static DBService DB;
    static AuthService auth;
    static APIEndPoints api;

    crow::request req;
    crow::response res;
    crow::json::wvalue body, return_body;

    virtual void SetUp() {
        DB.tearDownDatabase();
        DB.setUpDatabase();
    }

    virtual void TearDown() {
        DB.tearDownDatabase();
    }

    static void SetUpTestCase() {
        DB = DBService("tcp://127.0.0.1:3306",
         "dbuser", "123",
          "test_matchmaking_api_db");
        auth = AuthService();
        api = APIEndPoints(&DB, &auth);
    }

    static void TearDownTestCase() {
        DB.tearDownDatabase();
    }

    void signUp(Developer d) {
        body = {{"developer_email",
         d.developer_email},
          {"developer_password", d.developer_password}};
        req.body = body.dump();
        res = api.postSignUp(req);
        ASSERT_EQ(res.code, 200);
    }

    void logIn(Developer d, std::string *token) {
        body = {{"developer_email", d.developer_email},
         {"developer_password",
          d.developer_password}};
        req.body = body.dump();
        res = api.postLogin(req);
        ASSERT_EQ(res.code, 200);
        *token = res.body.substr(res.body.find(":") + 2);
    }

    void createGame(Game_Details g,
     std::string token, int *game_id) {
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
        std::pair <int, std::string> res_pair
         = api.postGames(req);
        ASSERT_EQ(res.code, res_pair.first);
        *game_id = std::stoi(res_pair.second);
    }
};



DBService GameGDPTestIntegrationFixture::DB;
AuthService GameGDPTestIntegrationFixture::auth;
APIEndPoints GameGDPTestIntegrationFixture::api;

/*Get Tests*/
/*testing first if branch*/
TEST_F(GameGDPTestIntegrationFixture, GameTest) {
    Developer valid_developer;
    valid_developer.developer_email = "some_email@gmail.com";
    valid_developer.developer_password = "some_password";
    valid_developer.is_valid = true;
    std::string token;

    Game_Details game1;
    game1.developer_email = valid_developer.developer_email;
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

    crow::request req;
    crow::response res;
    crow::json::wvalue body;
    crow::json::wvalue return_body;

    // first if branch returns incorrect rheader
    body = {};
    req.body = body.dump();
    res = api.getGame(req, game1.game_id);
    EXPECT_EQ(res.code, 401);
    EXPECT_EQ(res.body, "Invalid Header");
}

/*dev doesn't own game*/
TEST_F(GameGDPTestIntegrationFixture, GameTest2) {
    Developer valid_developer;
    valid_developer.developer_email = "some_email@gmail.com";
    valid_developer.developer_password = "some_password";
    valid_developer.is_valid = true;
    std::string token;

    Game_Details game1;
    game1.developer_email = valid_developer.developer_email;
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

    crow::request req;
    crow::response res;
    crow::json::wvalue body;
    crow::json::wvalue return_body;

    signUp(valid_developer);
    logIn(valid_developer, &(token));

    // authorize user
    req.add_header("Authorization", token);

    // game not created so developer does not own it
    res = api.getGame(req, game1.game_id);
    EXPECT_EQ(res.code, 403);
    EXPECT_EQ(res.body, "Invalid authentication");
}

/*success*/
TEST_F(GameGDPTestIntegrationFixture, GameTest3) {
    Developer valid_developer;
    valid_developer.developer_email = "some_email@gmail.com";
    valid_developer.developer_password = "some_password";
    valid_developer.is_valid = true;
    std::string token;

    Game_Details game1;
    game1.developer_email = valid_developer.developer_email;
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

    crow::request req;
    crow::response res;
    crow::json::wvalue body;
    crow::json::wvalue return_body;

    signUp(valid_developer);
    logIn(valid_developer, &(token));
    createGame(game1, token, &game1.game_id);

    req.add_header("Authorization", token);

    // expcted game details from the database
    return_body["game_id"] = game1.game_id;
    return_body["game_name"] = game1.game_name;
    return_body["game_parameter1_name"] = game1.game_parameter1_name;
    return_body["game_parameter1_weight"] = game1.game_parameter1_weight;
    return_body["game_parameter2_name"] = game1.game_parameter2_name;
    return_body["game_parameter2_weight"] = game1.game_parameter2_weight;
    return_body["game_parameter3_name"] = game1.game_parameter3_name;
    return_body["game_parameter3_weight"] = game1.game_parameter3_weight;
    return_body["game_parameter4_name"] = game1.game_parameter4_name;
    return_body["game_parameter4_weight"] = game1.game_parameter4_weight;
    return_body["category"] = game1.category;
    return_body["players_per_team"] = game1.players_per_team;
    return_body["teams_per_match"] = game1.teams_per_match;

    res = api.getGame(req, game1.game_id);
    EXPECT_EQ(res.code, 200);
    EXPECT_EQ(res.body, return_body.dump());
}

/*Delete*/
/*testing first if branch*/
TEST_F(GameGDPTestIntegrationFixture, GameTest5) {
    Developer valid_developer;
    valid_developer.developer_email = "some_email@gmail.com";
    valid_developer.developer_password = "some_password";
    valid_developer.is_valid = true;
    std::string token;

    Game_Details game1;
    game1.developer_email = valid_developer.developer_email;
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

    crow::request req;
    crow::response res;
    crow::json::wvalue body;
    crow::json::wvalue return_body;

    // first if branch returns incorrect rheader
    body = {};
    req.body = body.dump();
    res = api.deleteGame(req, game1.game_id);
    EXPECT_EQ(res.code, 401);
    EXPECT_EQ(res.body, "Invalid Header");
}

/*dev doesn't own game*/
TEST_F(GameGDPTestIntegrationFixture, GameTest6) {
    Developer valid_developer;
    valid_developer.developer_email = "some_email@gmail.com";
    valid_developer.developer_password = "some_password";
    valid_developer.is_valid = true;
    std::string token;

    Game_Details game1;
    game1.developer_email = valid_developer.developer_email;
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

    crow::request req;
    crow::response res;
    crow::json::wvalue body;
    crow::json::wvalue return_body;

    signUp(valid_developer);
    logIn(valid_developer, &(token));

    // authorize user
    req.add_header("Authorization", token);

    // game not created so developer does not own it
    res = api.deleteGame(req, game1.game_id);
    EXPECT_EQ(res.code, 403);
    EXPECT_EQ(res.body, "Invalid authentication");
}

/*success*/
TEST_F(GameGDPTestIntegrationFixture, GameTest7) {
    Developer valid_developer;
    valid_developer.developer_email = "some_email@gmail.com";
    valid_developer.developer_password = "some_password";
    valid_developer.is_valid = true;
    std::string token;

    Game_Details game1;
    game1.developer_email = valid_developer.developer_email;
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

    crow::request req;
    crow::response res;
    crow::json::wvalue body;
    crow::json::wvalue return_body;

    signUp(valid_developer);
    logIn(valid_developer, &(token));
    createGame(game1, token, &game1.game_id);

    req.add_header("Authorization", token);

    res = api.deleteGame(req, game1.game_id);
    EXPECT_EQ(res.code, 200);
    EXPECT_EQ(res.body, "Succesfully deleted game details");
}
