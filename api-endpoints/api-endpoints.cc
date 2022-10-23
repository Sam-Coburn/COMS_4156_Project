// Copyright [2022] RaisingCanesFanClub
#include "api-endpoints/api-endpoints-lib.h"

// helper function
std::pair<bool, std::string> APIEndPoints::authenticateToken(const crow::request& req) {
    std::string myauth = req.get_header_value("Authorization");
    // in future, should find user email from their stored token
    if (myauth.compare("test-token") == 0) {
        // for now, just return one valid user email
        return std::make_pair(true, std::string("techlead@apex.com"));
    } else {
        // return failure; token not found, or maybe expired!
        return std::make_pair(false, std::string(""));
    }
}

// Gets all client's games
// Response Body Details:
// Games [Array<Game>]
// Where a Game object looks as such
// game.id [String]
// game.name [String]
// game.category [String]
// game.parameters [Array<String>]
// game.weights [Array<Float>]
// game.teamsPerMatch [Integer]
// game.players_per_team; [Integer]
// SOMEDAY, ADD: game.minPlayersPerTeam [Integer] ??
// SOMEDAY, ADD: game.maxPlayersPerTeam [Integer] ??
std::pair <int, std::string> APIEndPoints::getGames(const crow::request& req) {
    DBService DB = DBService();
    std::vector<Game_Details> games;
    // authentication
    std::pair<bool, std::string> tokenInfo = authenticateToken(req);
    if (!tokenInfo.first) {
        return std::make_pair(401, std::string("Bad token"));
    }
    // get games from database
    games = DB.get_all_games_for_developer(tokenInfo.second);
    if (games.empty()) {
        return std::make_pair(401, std::string("Error Accessing Games: none found!"));
    }
    // define a lambda function for outputting a game to a json...
    auto toJson = [](Game_Details gm){
// game.id [String]
// game.name [String]
// game.category [String]
// game.parameters [Array<String>]
// game.weights [Array<Float>]
// game.teams_per_match [Integer]
// game.players_per_team; [Integer]
        Json::Value gmInfo;
        gmInfo["name"] = gm.game_name;
        gmInfo["id"] = gm.game_id;
        gmInfo["category"] = gm.category;
        gmInfo["parameters"].append(gm.game_parameter1_name);
        gmInfo["parameters"].append(gm.game_parameter2_name);
        gmInfo["parameters"].append(gm.game_parameter3_name);
        gmInfo["weights"].append(gm.game_parameter1_weight);
        gmInfo["weights"].append(gm.game_parameter2_weight);
        gmInfo["weights"].append(gm.game_parameter3_weight);
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
std::pair <int, std::string> APIEndPoints::postGames(const crow::request& req) {
    DBService DB = DBService();
    Game_Details gmInfo;
    // authentication
    std::pair<bool, std::string> tokenInfo = authenticateToken(req);
    if (!tokenInfo.first) {
        return std::make_pair(401, std::string("Bad token"));
    }
    // parse body into json object
    Json::CharReaderBuilder rbuilder;
    rbuilder["collectComments"] = false;
    std::string errs;
    std::stringstream body(req.body);
    Json::Value root;
    // validity checking
    if (req.body.length() == 0) {
        return std::make_pair(400, std::string("Empty body."));
    }
    try {
        crow::json::rvalue request_body = crow::json::load(req.body);
        body >> root;
    } catch (...) {
        return std::make_pair(400, std::string("Invalid json in body."));
    }
    if (root.isNull()) {
        return std::make_pair(400, std::string("Root of json is null"));
    }
    if (root["name"].isNull() || !root["name"].isString()) {
        return std::make_pair(400, std::string("invalid name field"));
    }
    if (root["parameters"].isNull() ||
        !root["parameters"].isArray() ||
        root["parameters"].size() != 3 ) {
        return std::make_pair(400, std::string("invalid params field"));
    }
    if (root["weights"].isNull() ||
        !root["weights"].isArray() ||
        root["weights"].size() != 3) {
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
    gmInfo.game_parameter1_weight = root["weights"][0].asFloat();
    gmInfo.game_parameter2_name = root["parameters"][1].asString();
    gmInfo.game_parameter2_weight = root["weights"][1].asFloat();
    gmInfo.game_parameter3_name = root["parameters"][2].asString();
    gmInfo.game_parameter3_weight = root["weights"][2].asFloat();
    gmInfo.teams_per_match = root["teams_per_match"].asInt();
    gmInfo.players_per_team = root["players_per_team"].asInt();

    // store game
    Game_Details addedGame = DB.add_game_details(gmInfo);
    if (!addedGame.is_valid) {
        return std::make_pair(400, std::string("error adding game."));
    }
    return std::make_pair(200, std::to_string(addedGame.game_id));
}
