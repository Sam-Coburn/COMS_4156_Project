// "Copyright 2022 RaisingCanesFanClub"

#include "api-endpoints/api-endpoints-lib.h"
#include "MockDBService.h"
#include "MockAuthService.h"

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;

class MockAPIEndPoints : public APIEndPoints {
 public:
    MockAPIEndPoints(DBService* db, AuthService* auth) : APIEndPoints(db, auth) {}
    MOCK_METHOD((std::pair<int, std::string>), authenticateTokenGetErrorCode, (const crow::request& req), (override));
    MOCK_METHOD(bool, developerOwnsGame, (std::string developer_email, int game_id), (override));
};

/*Get Tests*/
/*testing first if branch*/
TEST(GameTesting, GameTest) {
    MockDBService DB;
    MockAuthService auth;
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    Developer valid_developer;
    valid_developer.developer_email = "some_email@gmail.com";
    valid_developer.developer_password = "some_password";
    valid_developer.is_valid = true;

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

    std::pair<int, std::string> invalid_dev = std::make_pair(404, "Developer does not exist");
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(invalid_dev));

    res = api.getGame(req, game1.game_id);
    EXPECT_EQ(res.code, 404);
    EXPECT_EQ(res.body, "Developer does not exist");
}

/*dev doesn't own game*/
TEST(GameTesting, GameTest2) {
    MockDBService DB;
    MockAuthService auth;
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    Developer valid_developer;
    valid_developer.developer_email = "some_email@gmail.com";
    valid_developer.developer_password = "some_password";
    valid_developer.is_valid = true;

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

    std::pair<int, std::string> valid_dev = std::make_pair(200, valid_developer.developer_email);
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(valid_dev));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(false));

    res = api.getGame(req, game1.game_id);
    EXPECT_EQ(res.code, 403);
    EXPECT_EQ(res.body, "Invalid authentication");
}

/*success*/
TEST(GameTesting, GameTest3) {
    MockDBService DB;
    MockAuthService auth;
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    Developer valid_developer;
    valid_developer.developer_email = "some_email@gmail.com";
    valid_developer.developer_password = "some_password";
    valid_developer.is_valid = true;

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

    std::pair<int, std::string> valid_dev = std::make_pair(200, valid_developer.developer_email);
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(valid_dev));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(true));    
    EXPECT_CALL(DB, get_game_details(_)).Times(1)
    .WillOnce(Return(game1)); 

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

/*game not valid*/
TEST(GameTesting, GameTest4) {
    MockDBService DB;
    MockAuthService auth;
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    Developer valid_developer;
    valid_developer.developer_email = "some_email@gmail.com";
    valid_developer.developer_password = "some_password";
    valid_developer.is_valid = true;

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
    game1.is_valid = false;
    game1.game_id = 1;

    crow::request req;
    crow::response res;
    crow::json::wvalue body;
    crow::json::wvalue return_body;

    std::pair<int, std::string> valid_dev = std::make_pair(200, valid_developer.developer_email);
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(valid_dev));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(true));    
    EXPECT_CALL(DB, get_game_details(_)).Times(1)
    .WillOnce(Return(game1)); 

    res = api.getGame(req, game1.game_id);
    EXPECT_EQ(res.code, 404);
    EXPECT_EQ(res.body, "Game not found");
}

/*Delete*/
/*testing first if branch*/
TEST(GameTesting, GameTest5) {
    MockDBService DB;
    MockAuthService auth;
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    Developer valid_developer;
    valid_developer.developer_email = "some_email@gmail.com";
    valid_developer.developer_password = "some_password";
    valid_developer.is_valid = true;

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

    //testing first if branch
    std::pair<int, std::string> invalid_dev = std::make_pair(404, "Developer does not exist");
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(invalid_dev));

    res = api.deleteGame(req, game1.game_id);
    EXPECT_EQ(res.code, 404);
    EXPECT_EQ(res.body, "Developer does not exist");
}

/*dev doesn't own game*/
TEST(GameTesting, GameTest6) {
    MockDBService DB;
    MockAuthService auth;
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    Developer valid_developer;
    valid_developer.developer_email = "some_email@gmail.com";
    valid_developer.developer_password = "some_password";
    valid_developer.is_valid = true;

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

    std::pair<int, std::string> valid_dev = std::make_pair(200, valid_developer.developer_email);
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(valid_dev));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(false));

    res = api.deleteGame(req, game1.game_id);
    EXPECT_EQ(res.code, 403);
    EXPECT_EQ(res.body, "Invalid authentication");
}

/*success*/
TEST(GameTesting, GameTest7) {
    MockDBService DB;
    MockAuthService auth;
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    Developer valid_developer;
    valid_developer.developer_email = "some_email@gmail.com";
    valid_developer.developer_password = "some_password";
    valid_developer.is_valid = true;

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

    std::pair<int, std::string> valid_dev = std::make_pair(200, valid_developer.developer_email);
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(valid_dev));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(true));    
    EXPECT_CALL(DB, get_game_details(_)).Times(1)
    .WillOnce(Return(game1)); 
    EXPECT_CALL(DB, remove_game_details(_)).Times(1)
    .WillOnce(Return(game1)); 

    res = api.deleteGame(req, game1.game_id);
    EXPECT_EQ(res.code, 200);
    EXPECT_EQ(res.body, "Succesfully deleted game details");
}

/*game not valid*/
TEST(GameTesting, GameTest8) {
    MockDBService DB;
    MockAuthService auth;
    NiceMock<MockAPIEndPoints> api(&DB, &auth);

    Developer valid_developer;
    valid_developer.developer_email = "some_email@gmail.com";
    valid_developer.developer_password = "some_password";
    valid_developer.is_valid = true;

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
    game1.is_valid = false;
    game1.game_id = 1;

    crow::request req;
    crow::response res;
    crow::json::wvalue body;
    crow::json::wvalue return_body;

    std::pair<int, std::string> valid_dev = std::make_pair(200, valid_developer.developer_email);
    EXPECT_CALL(api, authenticateTokenGetErrorCode(_)).Times(1)
    .WillOnce(Return(valid_dev));
    EXPECT_CALL(api, developerOwnsGame(_, _)).Times(1)
    .WillOnce(Return(true));    
    EXPECT_CALL(DB, get_game_details(_)).Times(1)
    .WillOnce(Return(game1));

    res = api.deleteGame(req, game1.game_id);
    EXPECT_EQ(res.code, 400);
    EXPECT_EQ(res.body, "Invalid request body");
}
