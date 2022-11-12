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
    Developer d = APIEndPoints::DB.get_developer(root["developer_email"].asString());
    if (!d.is_valid) {
        return std::make_pair(401, std::string("developer not found."));
    }
    if (d.developer_password.compare(root["developer_password"].asString()) != 0) {
        return std::make_pair(401, std::string("password doesn't match."));
    }

    return std::make_pair(200, root["developer_email"].asString());
}

/*
    Validate user using email and password
    ** Only for first iteration **
*/
bool APIEndPoints::valid_user(std::string email, std::string password) {
  Developer d = DB.get_developer(email);
  return d.is_valid && d.developer_password == password;
}

/*
    Validate user is authorized to access requested game
*/
bool APIEndPoints::valid_user_gameid(std::string email, std::string password, int game_id) {
  if (!valid_user(email, password)) {
    return false;
  }

  Game_Details gd = DB.get_game_details(game_id);
  return gd.is_valid && gd.developer_email == email;
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

/*
    --------------------------------------------------------------------------------
    API Endpoint Functions
    --------------------------------------------------------------------------------
*/

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

/*
    Get all players for a requested game
    Request Parameters:
        game_id [Integer] REQUIRED
    Request Body:
        developer_email [String] REQUIRED
        developer_password [String] REQUIRED
    Return Body:
        players [JSON with PlayerGameRating Objects]
*/
crow::response APIEndPoints::getGamePlayers(const crow::request& req, int game_id) {
    try {
        crow::json::rvalue user_info = crow::json::load(req.body);
        std::vector<Joined_Player_Game_Ratings> joined_players;

        // Authenticating user
        std::string dev_email = user_info["developer_email"].s();
        std::string dev_password = user_info["developer_password"].s();
        if (!valid_user_gameid(dev_email, dev_password, game_id)) {
            return crow::response(401, "Invalid authentication");
        }

        joined_players = DB.get_all_player_game_ratings_for_game(game_id);

        if (joined_players.empty()) {
            std::string error = "No players found for game_id " + std::to_string(game_id);
            return crow::response(204, error);
        }

        // Converting vector of players to json object
        crow::json::wvalue players;
        for (Joined_Player_Game_Ratings p : joined_players) {
            if (p.is_valid) {
                std::string e = p.player_email;
                players[e]["game_id"] = p.game_id;
                players[e][p.game_parameter1_name] = p.game_parameter1_value;
                players[e][p.game_parameter2_name] = p.game_parameter2_value;
                players[e][p.game_parameter3_name] = p.game_parameter3_value;
                players[e][p.game_parameter4_name] = p.game_parameter4_value;
            } else {
                std::string error = "Internal Server Error due to player " + p.player_email;
                return crow::response(500, error);
            }
        }

        return crow::response(200, players);
    } catch(...) {
        return crow::response(400, "Invalid request body");
    }
}

/*
    Add player stats for a game
    Request Parameters:
        game_id [Integer] REQUIRED
    Request Body:
        developer_email [String] REQUIRED
        developer_password [String] REQUIRED
        players [Json with PlayerGameRating Objects] REQUIRED (Key: Player email)
    Return Body:
        return_message [String]    
*/
crow::response APIEndPoints::addPlayersStats(const crow::request& req, int game_id) {
    try {
        crow::json::rvalue user_req = crow::json::load(req.body);

        // Authenticating user
        std::string dev_email = user_req["developer_email"].s();
        std::string dev_password = user_req["developer_password"].s();
        if (!valid_user_gameid(dev_email, dev_password, game_id)) {
            return crow::response(401, "Invalid authentication");
        }

        crow::json::rvalue player_info;
        bool added = false;
        for (std::string pemail : user_req.keys()) {
            if (pemail != "developer_email" && pemail != "developer_password") {
                // If player does not already exist, add player to database
                Player p = DB.get_player(pemail);
                p.player_email = pemail;
                if (!p.is_valid) {
                    p = DB.add_player(p);
                    if (!p.is_valid) {
                        std::string error = "Internal Server Error due to player " + pemail;
                        return crow::response(500, error);
                    }
                }

                player_info = user_req[pemail];
                Player_Game_Ratings pgr;
                pgr.player_email = pemail;
                pgr.game_id = game_id;
                pgr.game_parameter1_value = player_info["game_parameter1_value"].i();
                pgr.game_parameter2_value = player_info["game_parameter2_value"].i();
                pgr.game_parameter3_value = player_info["game_parameter3_value"].i();
                pgr.game_parameter4_value = player_info["game_parameter4_value"].i();

                pgr = DB.add_player_rating(pgr);
                if (!pgr.is_valid) {
                    std::string error = "Internal Server Error due to player " + pemail;
                    return crow::response(500, error);
                } else {
                    added = true;
                }
            }
        }

        if (added) {
            return crow::response(200, "Player stats were added");
        } else {
            return crow::response(200, "No player stats added due to empty request");
        }
    } catch(...) {
        return crow::response(400, "Invalid request body");
    }
}

/*
    Get specific players' stats for a game
    Request Parameters:
        game_id [Integer] REQUIRED
    Request Body:
        developer_email [String] REQUIRED
        developer_password [String] REQUIRED
        player_emails [List[String]] REQUIRED
    Return Body:
        players [JSON with PlayerGameRating Objects]
*/
crow::response APIEndPoints::getPlayersStats(const crow::request& req, int game_id) {
    try {
        crow::json::rvalue user_req = crow::json::load(req.body);
        crow::json::rvalue input_player_emails_rvalue;
        std::vector<crow::json::rvalue> input_player_emails;
        Player_Game_Ratings pgr;
        crow::json::wvalue stats;
        std::string pemail;

        // Authenticating user
        std::string dev_email = user_req["developer_email"].s();
        std::string dev_password = user_req["developer_password"].s();
        if (!valid_user_gameid(dev_email, dev_password, game_id)) {
            return crow::response(401, "Invalid authentication");
        }

        input_player_emails_rvalue = user_req["player_emails"];
        input_player_emails = input_player_emails_rvalue.lo();
        std::string missing_players = "";
        for (crow::json::rvalue p_email : input_player_emails) {
            pemail = p_email.s();
            pgr = DB.get_player_game_rating(pemail, game_id);

            if (!pgr.is_valid) {
                if (missing_players == "") {
                    missing_players = pemail;
                } else {
                    missing_players = missing_players + "," + pemail;
                }
            }

            // Converting players' stats to json object
            if (pgr.is_valid) {
                stats[pemail]["game_parameter1_value"] = pgr.game_parameter1_value;
                stats[pemail]["game_parameter2_value"] = pgr.game_parameter2_value;
                stats[pemail]["game_parameter3_value"] = pgr.game_parameter3_value;
                stats[pemail]["game_parameter4_value"] = pgr.game_parameter4_value;
            } else {
                return crow::response(500, "Internal Server Error");
            }
        }

        if (missing_players == "") {
            return crow::response(200, stats);
        } else {
            std::string error = "The folllowing players do not exist " + missing_players;
            return crow::response(204, error);
        }
    } catch(...) {
        crow::json::wvalue error;
        error["Error"] = "Invalid request body";
        return crow::response(400, error);
    }
}

/*
    Delete requested players' stats for a given game
    Request Parameters:
        game_id [Integer] REQUIRED
    Request Body:
        developer_email [String] REQUIRED
        developer_password [String] REQUIRED
        player_emails [List[String]] REQUIRED
    Return Body:
        return_message [String]
*/
crow::response APIEndPoints::deletePlayersStats(const crow::request& req, int game_id) {
    try {
        crow::json::rvalue user_req = crow::json::load(req.body);
        crow::json::rvalue player_emails_rvalue;
        std::vector<crow::json::rvalue> player_emails;

        // Authenticating user
        std::string dev_email = user_req["developer_email"].s();
        std::string dev_password = user_req["developer_password"].s();
        if (!valid_user_gameid(dev_email, dev_password, game_id)) {
            return crow::response(401, "Invalid authentication");
        }

        std::string missing_players = "";
        player_emails_rvalue = user_req["player_emails"];
        player_emails = player_emails_rvalue.lo();
        for (crow::json::rvalue email_rvalue : player_emails) {
            std::string email = email_rvalue.s();
            Player_Game_Ratings pgr;
            pgr = DB.remove_player_rating(email, game_id);
            if (!pgr.is_valid) {
                if (missing_players == "") {
                    missing_players = email;
                } else {
                    missing_players = missing_players + "," + email;
                }
            }
        }

        if (missing_players == "") {
            return crow::response(200, "Player stats were removed");
        } else {
            std::string error = "The following players do not exist: " + missing_players;
            return crow::response(500, error);
        }
        } catch (...) {
        return crow::response(400, "Invalid request body");
    }
}

/*
    Update requested players' stats for a given game
    Request Parameters:
        game_id [Integer] REQUIRED
    Request Body:
        developer_email [String] REQUIRED
        developer_password [String] REQUIRED
        players [Json with PlayerGameRating Objects] REQUIRED (Key: Player email)
    Return Body:
        return_message [String]
*/
crow::response APIEndPoints::updatePlayersStats(const crow::request& req, int game_id) {
    try {
        crow::json::rvalue user_req = crow::json::load(req.body);

        // Authenticating user
        std::string dev_email = user_req["developer_email"].s();
        std::string dev_password = user_req["developer_password"].s();
        if (!valid_user_gameid(dev_email, dev_password, game_id)) {
            return crow::response(401, "Invalid authentication");
        }

        std::vector<std::string> req_emails = user_req.keys();
        remove(req_emails.begin(), req_emails.end(), "developer_email");
        remove(req_emails.begin(), req_emails.end(), "developer_password");

        std::string players_invalid_stats = "";
        crow::json::rvalue player_info;
        for (std::string pemail : req_emails) {
            player_info = user_req[pemail];
            try {
                player_info["game_parameter1_value"].i();
                player_info["game_parameter2_value"].i();
                player_info["game_parameter3_value"].i();
                player_info["game_parameter4_value"].i();
            } catch (...) {
                if (players_invalid_stats == "") {
                    players_invalid_stats = pemail;
                } else {
                    players_invalid_stats = players_invalid_stats + ", " + pemail;
                }
            }
        }

        if (players_invalid_stats != "") {
            std::string error = "The following players have invalid stats: " + players_invalid_stats;
            return crow::response(204, error);
        }

        bool added = false;
        for (std::string pemail : req_emails) {
            // If player does not already exist, add player to database
            Player p = DB.get_player(pemail);
            p.player_email = pemail;
            if (!p.is_valid) {
                p = DB.add_player(p);
                if (!p.is_valid) {
                    std::string error = "Internal Server Error due to player " + pemail;
                    return crow::response(500, error);
                }
            }

            player_info = user_req[pemail];
            Player_Game_Ratings pgr;
            pgr.player_email = pemail;
            pgr.game_id = game_id;
            pgr.game_parameter1_value = player_info["game_parameter1_value"].i();
            pgr.game_parameter2_value = player_info["game_parameter2_value"].i();
            pgr.game_parameter3_value = player_info["game_parameter3_value"].i();
            pgr.game_parameter4_value = player_info["game_parameter4_value"].i();

            pgr = DB.update_player_rating(pemail, game_id, pgr);
            if (!pgr.is_valid) {
                std::string error = "Internal Server Error due to player " + pemail;
                return crow::response(500, error);
            } else {
                added = true;
            }
        }

        if (added) {
            return crow::response(200, "Player stats were added");
        } else {
            return crow::response(200, "No player stats added due to empty request");
        }
    } catch(...) {
        return crow::response(400, "Invalid request body");
    }
}