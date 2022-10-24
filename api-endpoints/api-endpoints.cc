// Copyright [2022] RaisingCanesFanClub
#include "api-endpoints/api-endpoints-lib.h"

// helper function
// Checks whether supplied username and password are valid; insecure!
// helper for all API calls that require authentication before proceeding
std::pair<int, std::string> APIEndPoints::authenticateBadly(const crow::request& req) {
    std::string errs;
    Json::Value root;
    std::string s = req.body;
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

    // json validity checking
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
    if (root["developer_email"].isNull() || !root["developer_email"].isString()) {
        return std::make_pair(400, std::string("invalid developer_email field"));
    }
    if (root["developer_password"].isNull() || !root["developer_password"].isString()) {
        return std::make_pair(400, std::string("invalid developer_password field"));
    }

    // check credentials
    Developer d = DB.get_developer(root["developer_email"].asString());
    if (!d.is_valid) {
        return std::make_pair(401, std::string("developer not found."));
    }
    if (d.developer_password.compare(root["developer_password"].asString()) != 0) {
        return std::make_pair(401, std::string("password doesn't match."));
    }

    return std::make_pair(200, root["developer_email"].asString());
}

// // helper function
// // Checks whether supplied token is valid for client
// // helper for all API calls (other than login)
// // that require authentication of a token before proceeding
// std::pair<bool, std::string> APIEndPoints::authenticateToken(const crow::request& req) {
//     std::string myauth = req.get_header_value("Authorization");
//     // in future, should find user email from their stored token
//     if (myauth.compare("test-token") == 0) {
//         // for now, just return one valid user email
//         return std::make_pair(true, std::string("techlead@apex.com"));
//     } else {
//         // return failure; token not found, or maybe expired!
//         return std::make_pair(false, std::string(""));
//     }
// }

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
    std::vector<Game_Details> games;

    // authentication
    std::pair<int, std::string> tokenInfo = authenticateBadly(req);
    if (tokenInfo.first != 200) {
        return tokenInfo;
    }

    // get games from database
    games = DB.get_all_games_for_developer(tokenInfo.second);
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
    std::cout << "before empty string check" << std::endl;
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
    Game_Details addedGame = DB.add_game_details(gmInfo);
    if (!addedGame.is_valid) {
        return std::make_pair(400, std::string("error adding game."));
    }

    // return response body as string
    return std::make_pair(200, std::to_string(addedGame.game_id));
}
