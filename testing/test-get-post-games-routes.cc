// "Copyright [year] <Copyright Owner>"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "api-endpoints/api-endpoints-lib.h"
#include "authentication/auth-service.h"
#include "testing/MockDBService.h"
#include "testing/MockAuthService.h"
#include "main/db-service.h"

using ::testing::_;
using ::testing::Return;

// what kind of input body for the postGames request do we want to use?
enum POST_GAME_BODY_KIND { COPY, EMPTY, BAD_JSON, ROOT_NULL, NOT_NAME_VAL_PAIRS,
                           INVALID_NAME, INVALID_PARAMS, INVALID_PARAMS_ELT,
                           INVALID_WEIGHTS, INVALID_WEIGHTS_ELT, INVALID_CATEGORY };
// return a valid Game_Details struct for poker
Game_Details static defaultVal();
// generate input for a PostGames endpoint body, according to the flag POST_GAME_BODY_KIND
static std::string genPostGamesBody(POST_GAME_BODY_KIND kind, Game_Details gm);

// test desired response from PostGames vs actual response
static void testAgainstBody(APIEndPoints* api,
                     POST_GAME_BODY_KIND kind,
                     std::string want,
                     Game_Details gm = defaultVal(),
                     int code = 400,
                     bool noBody = false) {
     std::pair <int, std::string> res;
     crow::request req;
     req.add_header("Authorization", "a-very-nice-placeholder-token");
     req.body = genPostGamesBody(kind, gm);
     res = api->postGames(req);
     ASSERT_EQ(code, res.first);
     if (!noBody) {
        LOG(INFO) << "body I want: " << want << std::endl;
        LOG(INFO) << "body I get: " << res.second << std::endl;
        ASSERT_EQ(res.second.compare(want), 0);
     }
}

// test desired response from getGames vs actual response
static void testAgainstBodyGet(APIEndPoints* api,
                               POST_GAME_BODY_KIND kind,
                               std::string want,
                               std::string reqBdy,
                               int code = 400,
                               bool noBody = false) {
     std::pair <int, std::string> res;
     crow::request req;
     req.body = reqBdy;
     req.add_header("Authorization", "a-very-nice-placeholder-token");
     res = api->getGames(req);
     ASSERT_EQ(code, res.first);
     if (!noBody) {
        LOG(INFO) << "body I want: " << want << std::endl;
        LOG(INFO) << "body I get: " << res.second << std::endl;
        ASSERT_EQ(res.second.compare(want), 0);
     }
}

TEST(Get_Post_Games_Suite, Post_Games_Tests) {
  MockDBService DB;
  MockAuthService auth;
  APIEndPoints api = APIEndPoints(&DB, &auth);

  // input value
  Game_Details gm;

  // expected output body
  std::string bdy;

  // mock return values
  Game_Details invalid_game_details;
  Game_Details valid_game_details;
  Game_Details valid_game_details2;
  Developer valid_developer;

  // initialize mock return values
  valid_game_details.is_valid = true;
  valid_game_details.game_id = 5;
  valid_game_details2.is_valid = true;
  valid_game_details2.game_id = 6;
  invalid_game_details.is_valid = false;
  valid_developer.developer_email = "gamedev42@awesomeCardGames.com";
  valid_developer.developer_password = "some_password";
  valid_developer.is_valid = true;

  // specify mocked call behavior

  EXPECT_CALL(auth, decodeAndVerifyJWT(_))
  .WillOnce(Return(std::make_pair(false, "Invalid token")))
  .WillRepeatedly(Return(std::make_pair(true, "gamedev42@awesomeCardGames.com")));

  EXPECT_CALL(DB, get_developer(_))
  .WillRepeatedly(Return(valid_developer));

  EXPECT_CALL(DB, add_game_details(_)).Times(3)
  .WillOnce(Return(valid_game_details))
  .WillOnce(Return(invalid_game_details))
  .WillOnce(Return(valid_game_details2));

  // set input as valid game
  gm.category = "cards";
  gm.game_name = "poker";
  gm.developer_email = "gamedev42@awesomeCardGames.com";
  gm.game_id = 5;
  gm.game_parameter1_name = "wins";
  gm.game_parameter2_name = "losses";
  gm.game_parameter3_name = "totalMoneyWon";
  gm.game_parameter4_name = "";
  gm.game_parameter1_weight = 0.2;
  gm.game_parameter2_weight = 0.1;
  gm.game_parameter3_weight = 0.7;
  gm.game_parameter4_weight = 0;
  gm.players_per_team = 100;
  gm.teams_per_match = 100;
  gm.is_valid = true;


  // test with invalid developer
  bdy = "Invalid token";
  testAgainstBody(&api, COPY, bdy, gm, 401);

  // rest of the tests are with valid developer

  // valid game, so add it.
  bdy = "5";
  testAgainstBody(&api, COPY, bdy, gm, 200, false);

  // DB error, so can't add it.
  bdy = "DB error adding game.";
  testAgainstBody(&api, COPY, bdy, gm);

  bdy = "Empty body.";
  testAgainstBody(&api, EMPTY, bdy);

  bdy = "Malformed json.";
  testAgainstBody(&api, BAD_JSON, bdy);

  bdy = "Root of json is null";
  testAgainstBody(&api, ROOT_NULL, bdy);

  bdy = "Expecting collection of name:value pairs.";
  testAgainstBody(&api, NOT_NAME_VAL_PAIRS, bdy);

  bdy = "invalid name field";
  testAgainstBody(&api, INVALID_NAME, bdy);

  bdy = "invalid params field";
  testAgainstBody(&api, INVALID_PARAMS, bdy);

  bdy = "params elt is invalid string";
  testAgainstBody(&api, INVALID_PARAMS_ELT, bdy);

  bdy = "invalid weights field";
  testAgainstBody(&api, INVALID_WEIGHTS, bdy);

  bdy = "weights elt is invalid float";
  testAgainstBody(&api, INVALID_WEIGHTS_ELT, bdy);

  // valid game with negative weight
  bdy = "6";
  gm.game_id = 6;
  gm.game_parameter1_weight = -0.2;
  testAgainstBody(&api, COPY, bdy, gm, 200, false);

  bdy = "invalid category";
  testAgainstBody(&api, INVALID_CATEGORY, bdy);

  bdy = "teams_per_match must be > 0";
  gm.teams_per_match = -5;
  testAgainstBody(&api, COPY, bdy, gm);

  bdy = "players_per_team must be > 0";
  gm.teams_per_match = 8;
  gm.players_per_team = 0;
  testAgainstBody(&api, COPY, bdy, gm);
}

TEST(Get_Post_Games_Suite, Get_Games_Tests) {
  MockDBService DB;
  MockAuthService auth;
  APIEndPoints api = APIEndPoints(&DB, &auth);

  // input value
  Game_Details gm;

  // expected output body
  std::string bdy;

  // mock return values
  Game_Details invalid_game_details;
  Game_Details valid_game_details;
  Developer valid_developer;
  Developer invalid_developer;
  std::vector<Game_Details> empty;
  std::vector<Game_Details> games;

  // initialize mock return values
  valid_developer.developer_email = "gamedev42@awesomeCardGames.com";
  valid_developer.is_valid = true;
  invalid_developer.is_valid = false;
  empty = std::vector<Game_Details>();
  games.push_back(Game_Details());
  games.push_back(Game_Details());
  games.push_back(Game_Details());
  games.push_back(Game_Details());
  std::string names[4] = {"texas holdem", "blackjack", "5-card draw", "omaha"};
  for (size_t i = 0; i < 4; i++) {
    games[i].category = "cards";
    games[i].game_name = names[i];
    games[i].developer_email = "gamedev42@awesomeCardGames.com";
    games[i].game_id = 5;
    games[i].game_parameter1_name = "wins";
    games[i].game_parameter2_name = "losses";
    games[i].game_parameter3_name = "totalMoneyWon";
    games[i].game_parameter4_name = "";
    games[i].game_parameter1_weight = 0.2;
    games[i].game_parameter2_weight = 0.1;
    games[i].game_parameter3_weight = 0.7;
    games[i].game_parameter4_weight = 0;
    games[i].players_per_team = 1;
    games[i].teams_per_match = 8;
    games[i].is_valid = true;
  }
    // define a lambda function for outputting a game to a json...
    auto toJson = [](Game_Details gm){
        Json::Value gmInfo;
        gmInfo["name"] = gm.game_name;
        gmInfo["id"] = gm.game_id;
        gmInfo["category"] = gm.category;
        gmInfo["parameters"].append(gm.game_parameter1_name);
        gmInfo["parameters"].append(gm.game_parameter2_name);
        gmInfo["parameters"].append(gm.game_parameter3_name);
        gmInfo["parameters"].append(gm.game_parameter4_name);
        gmInfo["weights"].append(gm.game_parameter1_weight);
        gmInfo["weights"].append(gm.game_parameter2_weight);
        gmInfo["weights"].append(gm.game_parameter3_weight);
        gmInfo["weights"].append(gm.game_parameter4_weight);
        gmInfo["teams_per_match"] = gm.teams_per_match;
        gmInfo["players_per_team"] = gm.players_per_team;
        return gmInfo;
    };
    // output games in json array
    Json::Value root;
    root["games"] = Json::Value(Json::arrayValue);
    for (size_t i = 0; i < games.size(); i++) {
        root["games"].append(toJson(games[i]));
    }
    // convert json object to stringstream
    std::stringstream ss;
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    writer->write(root, &ss);

  // specify mocked call behavior
  EXPECT_CALL(auth, decodeAndVerifyJWT(_))
  .WillOnce(Return(std::make_pair(false, "Invalid token")))
  .WillRepeatedly(Return(std::make_pair(true, "gamedev42@awesomeCardGames.com")));

  EXPECT_CALL(DB, get_developer(_))
  .WillOnce(Return(invalid_developer))
  .WillRepeatedly(Return(valid_developer));

  EXPECT_CALL(DB, get_all_games_for_developer(_))
  .WillOnce(Return(empty))
  .WillOnce(Return(games));

  // test with invalid developer
  bdy = "Invalid token";
  testAgainstBodyGet(&api, COPY, bdy, "", 401);

  bdy = "Developer does not exist";
  testAgainstBodyGet(&api, COPY, bdy, "", 404);

  bdy = "Error Accessing Games: none found!";
  testAgainstBodyGet(&api, COPY, bdy, "", 204);

  bdy = ss.str();  // case success
  testAgainstBodyGet(&api, COPY, bdy, "", 200);
}

Game_Details static defaultVal() {
  Game_Details defaultVal;
  defaultVal.category = "cards";
  defaultVal.game_name = "poker";
  defaultVal.developer_email = "gamedev42@awesomeCardGames.com";
  defaultVal.game_id = 5;
  defaultVal.game_parameter1_name = "wins";
  defaultVal.game_parameter2_name = "losses";
  defaultVal.game_parameter3_name = "totalMoneyWon";
  defaultVal.game_parameter4_name = "";
  defaultVal.game_parameter1_weight = 0.2;
  defaultVal.game_parameter2_weight = 0.1;
  defaultVal.game_parameter3_weight = 0.7;
  defaultVal.game_parameter4_weight = 0;
  defaultVal.players_per_team = 881;
  defaultVal.teams_per_match = 89;
  defaultVal.is_valid = true;
  return defaultVal;
}

std::string genPostGamesBody(POST_GAME_BODY_KIND kind = COPY, Game_Details gm = defaultVal()) {
    std::stringstream ss;
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    Json::Value root;
    Json::Value gmInfo;
    switch (kind) {
        case EMPTY:
            return "";
        case BAD_JSON:
            return "ldfgjkdlf309458094%%^/..[";
        case ROOT_NULL:
            break;
        case NOT_NAME_VAL_PAIRS:
            gmInfo["name"] = "poker";
            // since we're generating postGames input
            // WE DO NOT COPY IN A GAME ID.
            gmInfo["category"] = "cards";
            gmInfo["parameters"].append("wins");
            gmInfo["parameters"].append("losses");
            gmInfo["parameters"].append("total money won");
            gmInfo["parameters"].append("");
            gmInfo["weights"].append(0.5);
            gmInfo["weights"].append(0.3);
            gmInfo["weights"].append(0.2);
            gmInfo["weights"].append(0.0);
            gmInfo["teams_per_match"] = 8;
            gmInfo["players_per_team"] = 1;
            root.append(gmInfo);
            break;
        case INVALID_NAME:
            root["name"] = 17;
            root["category"] = gm.category;
            root["developer_email"] = gm.developer_email;
            root["developer_password"] = "some_password";  // better authentication testing comes later
            root["parameters"].append(gm.game_parameter1_name);
            root["parameters"].append(gm.game_parameter2_name);
            root["parameters"].append(gm.game_parameter3_name);
            root["parameters"].append(gm.game_parameter4_name);
            root["weights"].append(gm.game_parameter1_weight);
            root["weights"].append(gm.game_parameter2_weight);
            root["weights"].append(gm.game_parameter3_weight);
            root["weights"].append(gm.game_parameter4_weight);
            root["teams_per_match"] = gm.teams_per_match;
            root["players_per_team"] = gm.players_per_team;
            break;
        case INVALID_PARAMS:
            root["name"] = gm.game_name;
            // since we're generating postGames input
            // WE DO NOT COPY IN A GAME ID.
            // WE DO NOT COPY IN ISVALID FIELD.
            root["category"] = gm.category;
            root["developer_email"] = gm.developer_email;
            root["developer_password"] = "some_password";  // better authentication testing comes later
            root["parameters"] = "hoodle";
            root["weights"].append(gm.game_parameter1_weight);
            root["weights"].append(gm.game_parameter2_weight);
            root["weights"].append(gm.game_parameter3_weight);
            root["weights"].append(gm.game_parameter4_weight);
            root["teams_per_match"] = gm.teams_per_match;
            root["players_per_team"] = gm.players_per_team;
            break;
        case INVALID_PARAMS_ELT:
            root["name"] = gm.game_name;
            // since we're generating postGames input
            // WE DO NOT COPY IN A GAME ID.
            // WE DO NOT COPY IN ISVALID FIELD.
            root["category"] = gm.category;
            root["developer_email"] = gm.developer_email;
            root["developer_password"] = "some_password";  // better authentication testing comes later
            root["parameters"].append(gm.game_parameter1_name);
            root["parameters"].append(gm.game_parameter2_name);
            root["parameters"].append(92.3);
            root["parameters"].append(gm.game_parameter4_name);
            root["weights"].append(gm.game_parameter1_weight);
            root["weights"].append(gm.game_parameter2_weight);
            root["weights"].append(gm.game_parameter3_weight);
            root["weights"].append(gm.game_parameter4_weight);
            root["teams_per_match"] = gm.teams_per_match;
            root["players_per_team"] = gm.players_per_team;
            break;
        case INVALID_WEIGHTS:
            root["name"] = gm.game_name;
            // since we're generating postGames input
            // WE DO NOT COPY IN A GAME ID.
            // WE DO NOT COPY IN ISVALID FIELD.
            root["category"] = gm.category;
            root["developer_email"] = gm.developer_email;
            root["developer_password"] = "some_password";  // better authentication testing comes later
            root["parameters"].append(gm.game_parameter1_name);
            root["parameters"].append(gm.game_parameter2_name);
            root["parameters"].append(gm.game_parameter3_name);
            root["parameters"].append(gm.game_parameter4_name);
            root["weights"] = 85;
            root["teams_per_match"] = gm.teams_per_match;
            root["players_per_team"] = gm.players_per_team;
            break;
        case INVALID_WEIGHTS_ELT:
            root["name"] = gm.game_name;
            // since we're generating postGames input
            // WE DO NOT COPY IN A GAME ID.
            // WE DO NOT COPY IN ISVALID FIELD.
            root["category"] = gm.category;
            root["developer_email"] = gm.developer_email;
            root["developer_password"] = "some_password";  // better authentication testing comes later
            root["parameters"].append(gm.game_parameter1_name);
            root["parameters"].append(gm.game_parameter2_name);
            root["parameters"].append(gm.game_parameter3_name);
            root["parameters"].append(gm.game_parameter4_name);
            root["weights"].append(gm.game_parameter1_weight);
            root["weights"].append(gm.game_parameter2_weight);
            root["weights"].append(gm.game_parameter3_weight);
            root["weights"].append("donut");
            root["teams_per_match"] = gm.teams_per_match;
            root["players_per_team"] = gm.players_per_team;
            break;
        case INVALID_CATEGORY:
            root["name"] = gm.game_name;
            // since we're generating postGames input
            // WE DO NOT COPY IN A GAME ID.
            // WE DO NOT COPY IN ISVALID FIELD.
            root["category"] = 9000;
            root["developer_email"] = gm.developer_email;
            root["developer_password"] = "some_password";  // better authentication testing comes later
            root["parameters"].append(gm.game_parameter1_name);
            root["parameters"].append(gm.game_parameter2_name);
            root["parameters"].append(gm.game_parameter3_name);
            root["parameters"].append(gm.game_parameter4_name);
            root["weights"].append(gm.game_parameter1_weight);
            root["weights"].append(gm.game_parameter2_weight);
            root["weights"].append(gm.game_parameter3_weight);
            root["weights"].append(gm.game_parameter4_weight);
            root["teams_per_match"] = gm.teams_per_match;
            root["players_per_team"] = gm.players_per_team;
            break;
        default:  // COPY
            root["name"] = gm.game_name;
            // since we're generating postGames input
            // WE DO NOT COPY IN A GAME ID.
            // WE DO NOT COPY IN ISVALID FIELD.
            root["category"] = gm.category;
            root["developer_email"] = gm.developer_email;
            root["developer_password"] = "some_password";  // better authentication testing comes later
            root["parameters"].append(gm.game_parameter1_name);
            root["parameters"].append(gm.game_parameter2_name);
            root["parameters"].append(gm.game_parameter3_name);
            root["parameters"].append(gm.game_parameter4_name);
            root["weights"].append(gm.game_parameter1_weight);
            root["weights"].append(gm.game_parameter2_weight);
            root["weights"].append(gm.game_parameter3_weight);
            root["weights"].append(gm.game_parameter4_weight);
            root["teams_per_match"] = gm.teams_per_match;
            root["players_per_team"] = gm.players_per_team;
    }
    writer->write(root, &ss);
    return ss.str();
}
