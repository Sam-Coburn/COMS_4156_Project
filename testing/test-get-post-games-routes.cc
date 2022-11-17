// "Copyright [year] <Copyright Owner>"

#include "api-endpoints/api-endpoints-lib.h"
#include "MockDBService.h"

using ::testing::_;
using ::testing::Return;

/*
std::pair <int, std::string> getGames(const crow::request& req);

std::pair <int, std::string> postGame(const crow::request& req);
*/

/*
{
    "developer_email":"developer@chess.com",
    "developer_password":"super_secret_password",

			"category" : "casual",
			"name" : "poker",
			"parameters" : 
			[
				"rating",
				"don't need 'em",
				"nope",
				""
			],
			"players_per_team" : 1,
			"teams_per_match" : 4,
			"weights" : 
			[
				2.0,
				1.0,
				1.0,
				0.0
			]
}
*/

/*
std::pair <int, std::string> APIEndPoints::getGames(const crow::request& req) {
    std::vector<Game_Details> games;

    // authentication
    std::pair<int, std::string> tokenInfo = authenticateBadly(req);
    if (tokenInfo.first != 200) {
        return tokenInfo;
    }

    // get games from database
    games = DB->get_all_games_for_developer(tokenInfo.second);
    if (games.empty()) {
        return std::make_pair(401, std::string("Error Accessing Games: none found!"));
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

    // return response body as string
    return std::make_pair(200, ss.str());
}

// Adds a game to client's account
// Request Parameters:
// name [String] REQUIRED
// parameters [Array<String>] REQUIRED
// weights [Array<Integer>] REQUIRED
// category [String] OPTIONAL
// teams_per_match [Integer]
// players_per_team; [Integer]
// SOMEDAY, ADD: Min-players-per-team [Integer] ??
// SOMEDAY, ADD: Max-players-per-team [Integer] ??
std::pair <int, std::string> APIEndPoints::postGame(const crow::request& req) {
    Game_Details gmInfo;

    // authentication
    std::pair<int, std::string> tokenInfo = authenticateBadly(req);
    if (tokenInfo.first != 200) {
        return tokenInfo;
    }

    // parse body into json object
    std::string errs;
    Json::Value root;
    std::string s = req.body;
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

    // validity checking
    LOG(INFO) << "before empty string check" << std::endl;
    if (req.body.length() == 0) {
        return std::make_pair(400, std::string("Empty body."));
    }
    if (!reader->parse(s.c_str(), s.c_str() + s.length(), &root,
                       &errs)) {
      return std::make_pair(400, std::string("Malformed json."));
    }
    if (root.isNull()) {
        return std::make_pair(400, std::string("Root of json is null"));
    }
    if (root.type() != Json::objectValue) {
        return std::make_pair(400, std::string("Expecting collection of name:value pairs."));
    }
    if (root["name"].isNull() || !root["name"].isString()) {
        return std::make_pair(400, std::string("invalid name field"));
    }
    if (root["parameters"].isNull() ||
        !root["parameters"].isArray() ||
        root["parameters"].size() != 4 ) {
        return std::make_pair(400, std::string("invalid params field"));
    }
    if (root["weights"].isNull() ||
        !root["weights"].isArray() ||
        root["weights"].size() != 4) {
        return std::make_pair(400, std::string("invalid weights field"));
    }
    if (root["category"].isNull() || !root["category"].isString()) {
        return std::make_pair(400, std::string("invalid category"));
    }
    if (root["teams_per_match"].isNull() || !root["teams_per_match"].isInt()) {
        return std::make_pair(400, std::string("invalid teams_per_match field"));
    }
    if (root["players_per_team"].isNull() || !root["players_per_team"].isInt()) {
        return std::make_pair(400, std::string("invalid players_per_team field."));
    }

    // intialize game
    gmInfo.developer_email = tokenInfo.second;
    gmInfo.game_name = root["name"].asString();
    gmInfo.category = root["category"].asString();
    gmInfo.game_parameter1_name = root["parameters"][0].asString();
    gmInfo.game_parameter2_name = root["parameters"][1].asString();
    gmInfo.game_parameter3_name = root["parameters"][2].asString();
    gmInfo.game_parameter4_name = root["parameters"][3].asString();
    gmInfo.game_parameter1_weight = root["weights"][0].asFloat();
    gmInfo.game_parameter2_weight = root["weights"][1].asFloat();
    gmInfo.game_parameter3_weight = root["weights"][2].asFloat();
    gmInfo.game_parameter4_weight = root["weights"][3].asFloat();
    gmInfo.teams_per_match = root["teams_per_match"].asInt();
    gmInfo.players_per_team = root["players_per_team"].asInt();

    // store game
    Game_Details addedGame = DB->add_game_details(gmInfo);
    if (!addedGame.is_valid) {
        return std::make_pair(400, std::string("error adding game."));
    }

    // return response body as string
    return std::make_pair(200, std::to_string(addedGame.game_id));
}
*/

enum POST_GAME_BODY_KIND { COPY, EMPTY, BAD_JSON, ROOT_NULL, NOT_NAME_VAL_PAIRS,
                           INVALID_NAME, INVALID_PARAMS, INVALID_PARAMS_ELT,
                           INVALID_WEIGHTS, INVALID_WEIGHTS_ELT, INVALID_CATEGORY };
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

void testAgainstBody(APIEndPoints* api,
                     POST_GAME_BODY_KIND kind,
                     std::string err,
                     Game_Details gm = defaultVal(),
                     int code = 400,
                     bool noBody = false) {
     std::pair <int, std::string> res;
     crow::request req;
     req.body = genPostGamesBody(kind, gm);
     res = api->postGames(req);
     ASSERT_EQ(code, res.first);
     if (!noBody) {
        LOG(INFO) << "body I want: " << err << std::endl;
        LOG(INFO) << "body I get: " << res.second << std::endl;
        ASSERT_EQ(res.second.compare(err), 0);
     }
}

TEST(Get_Post_Games_Suite, Post_Games_Tests) {
  MockDBService DB;
  APIEndPoints api = APIEndPoints(&DB);

  std::string err;
  Game_Details invalid_game_details;
  invalid_game_details.is_valid = false;
  Game_Details valid_game_details;
  valid_game_details.is_valid = true;
  valid_game_details.game_id = 5;
  Developer valid_developer;
  valid_developer.developer_email = "gamedev42@awesomeCardGames.com";
  valid_developer.developer_password = "some_password";
  valid_developer.is_valid = true;
  Developer invalid_developer;
  invalid_developer.is_valid = false;

  // a valid game
  Game_Details gm;
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

  EXPECT_CALL(DB, get_developer(_))
  .WillRepeatedly(Return(valid_developer));

  EXPECT_CALL(DB, add_game_details(_)).Times(2)
  .WillOnce(Return(valid_game_details))
  .WillOnce(Return(invalid_game_details));

  // valid game, so add it.
  err = "5";
  testAgainstBody(&api, COPY, err, gm, 200, false);

  // DB error, so can't add it.
  err = "DB error adding game.";
  testAgainstBody(&api, COPY, err, gm);

  err = "Empty body.";
  testAgainstBody(&api, EMPTY, err);

  err = "Malformed json.";
  testAgainstBody(&api, BAD_JSON, err);

  err = "Root of json is null";
  testAgainstBody(&api, ROOT_NULL, err);

  err = "Expecting collection of name:value pairs.";
  testAgainstBody(&api, NOT_NAME_VAL_PAIRS, err);

  err = "invalid name field";
  testAgainstBody(&api, INVALID_NAME, err);

  err = "invalid params field";
  testAgainstBody(&api, INVALID_PARAMS, err);

  err = "params elt is invalid string";
  testAgainstBody(&api, INVALID_PARAMS_ELT, err);

  err = "invalid weights field";
  testAgainstBody(&api, INVALID_WEIGHTS, err);

  err = "weights elt is invalid float";
  testAgainstBody(&api, INVALID_WEIGHTS_ELT, err);

  err = "invalid category";
  testAgainstBody(&api, INVALID_CATEGORY, err);

  err = "teams_per_match must be > 0";
  gm.teams_per_match = -5;
  testAgainstBody(&api, COPY, err, gm);

  err = "players_per_team must be > 0";
  gm.teams_per_match = 8;
  gm.players_per_team = 0;
  testAgainstBody(&api, COPY, err, gm);
}

TEST(Get_Post_Games_Suite, Get_Games_Tests) {
  MockDBService DB;
  APIEndPoints api = APIEndPoints(&DB);

  ASSERT_EQ(5, 5);
}
