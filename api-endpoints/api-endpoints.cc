// Copyright [2022] RaisingCanesFanClub
#include "api-endpoints/api-endpoints-lib.h"


/*
    Authenticating developer's token to ensure they are authorized to use the service
*/
std::pair<int, std::string> APIEndPoints::authenticateTokenGetErrorCode(const crow::request& req) {
  std::string header;
  std::string token;
  Developer D;

  // To catch missing token or header
  try {
    header = req.get_header_value("Authorization");
    token = header.substr(7);
  } catch (...) {
    return std::make_pair(401, "Invalid Header");
  }

  // Validation logic
  try {
    std::pair<bool, std::string> decodedToken = auth->decodeAndVerifyJWT(token);
    if (decodedToken.first == false) {
      return std::make_pair(401, decodedToken.second);
    }

    D = DB->get_developer(decodedToken.second);
    if (!D.is_valid) {
      // Developer might have been deleted
      return std::make_pair(404, "Developer does not exist");
    }

    return std::make_pair(200, D.developer_email);
  } catch (...) {
    // Should never be hit unless something really goes wrong
    return std::make_pair(500, "Internal Server Error");
  }
}

/*
    Validate if developer owns requested game
*/
bool APIEndPoints::developerOwnsGame(std::string developer_email, int game_id) {
    Game_Details gd = DB->get_game_details(game_id);
    return gd.is_valid && gd.developer_email == developer_email;
}

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
    std::pair<int, std::string> tokenInfo = authenticateTokenGetErrorCode(req);
    if (tokenInfo.first != 200) {
        return tokenInfo;
    }

    // get games from database
    games = DB->get_all_games_for_developer(tokenInfo.second);
    if (games.empty()) {
        return std::make_pair(204, std::string("Error Accessing Games: none found!"));
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
std::pair <int, std::string> APIEndPoints::postGames(const crow::request& req) {
    Game_Details gmInfo;

    // authentication
    std::pair<int, std::string> tokenInfo = authenticateTokenGetErrorCode(req);
    if (tokenInfo.first != 200) {
        return tokenInfo;
    }

    // parse body into json object
    std::string errs;
    Json::Value root;
    std::string s = req.body;
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

    // unfortunate helper to check if a jsoncpp value is a float
    auto myIsFloat = [](Json::Value v) {
        Json::StreamWriterBuilder builder;
        const std::string s = Json::writeString(builder, v);
        int dotCount = 0;
        size_t i = 0;
        // negative float
        if ((s.size() > 2) && (s[i] == '-')) {
          i = 1;
        }
        // any other case
        for (i; i < s.size(); i++) {
          if (s[i] == '.') {
            dotCount++;
          } else if (!std::isdigit(s[i])) {
            return false;
          }
        }
        return (dotCount <= 1);
    };

    // validity checking
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
    for (unsigned int i = 0; i < root["parameters"].size(); i++) {
      if (!root["parameters"][i].isString()) {
        return std::make_pair(400, std::string("params elt is invalid string"));
      }
    }
    if (root["weights"].isNull() ||
        !root["weights"].isArray() ||
        root["weights"].size() != 4) {
        return std::make_pair(400, std::string("invalid weights field"));
    }
    for (unsigned int i = 0; i < root["weights"].size(); i++) {
      if (!myIsFloat(root["weights"][i])) {
        return std::make_pair(400, std::string("weights elt is invalid float"));
      }
    }
    if (root["category"].isNull() || !root["category"].isString()) {
        return std::make_pair(400, std::string("invalid category"));
    }
    if (root["teams_per_match"].isNull() || !root["teams_per_match"].isInt()) {
        return std::make_pair(400, std::string("invalid teams_per_match field"));
    }
    if (root["players_per_team"].isNull() || !root["players_per_team"].isInt()) {
        return std::make_pair(400, std::string("invalid players_per_team field"));
    }

    // intialize game
    gmInfo.developer_email = tokenInfo.second;
    gmInfo.game_name = root["name"].asString();
    gmInfo.category = root["category"].asString();
    gmInfo.game_parameter1_name = root["parameters"][0].asString();
    gmInfo.game_parameter2_name = root["parameters"][1].asString();
    gmInfo.game_parameter3_name = root["parameters"][2].asString();
    gmInfo.game_parameter4_name = root["parameters"][3].asString();
    gmInfo.game_parameter1_weight = root["weights"][0].asDouble();
    gmInfo.game_parameter2_weight = root["weights"][1].asDouble();
    gmInfo.game_parameter3_weight = root["weights"][2].asDouble();
    gmInfo.game_parameter4_weight = root["weights"][3].asDouble();
    gmInfo.teams_per_match = root["teams_per_match"].asInt();
    gmInfo.players_per_team = root["players_per_team"].asInt();

    // field meaning error check
    if (gmInfo.teams_per_match <= 0) {
      return std::make_pair(400, std::string("teams_per_match must be > 0"));
    }
    if (gmInfo.players_per_team <= 0) {
      return std::make_pair(400, std::string("players_per_team must be > 0"));
    }

    // store game
    Game_Details addedGame = DB->add_game_details(gmInfo);
    if (!addedGame.is_valid) {
        return std::make_pair(400, std::string("DB error adding game."));
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
        std::pair<int, std::string> tokenInfo = authenticateTokenGetErrorCode(req);
        if (tokenInfo.first != 200) {
            return crow::response(tokenInfo.first, tokenInfo.second);
        }

        // Authenticating user has access to game
        if (!developerOwnsGame(tokenInfo.second, game_id)) {
            std::string error = "Do not have access to game with game_id " + std::to_string(game_id);
            return crow::response(403, error);
        }

        joined_players = DB->get_all_player_game_ratings_for_game(game_id);

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
                if (p.game_parameter1_name != "") {
                    players[e][p.game_parameter1_name] = p.game_parameter1_value;
                }
                if (p.game_parameter1_name != "") {
                    players[e][p.game_parameter2_name] = p.game_parameter2_value;
                }
                if (p.game_parameter3_name != "") {
                    players[e][p.game_parameter3_name] = p.game_parameter3_value;
                }
                if (p.game_parameter4_name != "") {
                    players[e][p.game_parameter4_name] = p.game_parameter4_value;
                }
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
        std::pair<int, std::string> tokenInfo = authenticateTokenGetErrorCode(req);
        if (tokenInfo.first != 200) {
            return crow::response(tokenInfo.first, tokenInfo.second);
        }

        // Authenticating user has access to game
        if (!developerOwnsGame(tokenInfo.second, game_id)) {
            std::string error = "Do not have access to game with game_id " + std::to_string(game_id);
            return crow::response(403, error);
        }

        // Making sure the provided stats are in the valid format and can be added
        crow::json::rvalue player_info;
        std::vector<Player_Game_Ratings> pgrs;
        for (std::string pemail : user_req.keys()) {
            Player p = DB->get_player(pemail);
            p.player_email = pemail;
            if (p.is_valid) {
                // If player stats already exists, should be using update player stats function
                Player_Game_Ratings pgr = DB->get_player_game_rating(pemail, game_id);
                if (pgr.is_valid) {
                    std::string error = "No stats were added due to player " + pemail
                        + " which already exists. Please use update stats endpoint instead.";
                    return crow::response(409, error);
                }
            } else {
                // If player does not already exist, add player to database
                p = DB->add_player(p);
                if (!p.is_valid) {
                    std::string error = "Internal Server Error due to player " + pemail;
                    return crow::response(500, error);
                }
            }

            player_info = user_req[pemail];
            Player_Game_Ratings pgr;
            pgr.player_email = pemail;
            pgr.game_id = game_id;

            std::vector<std::pair<int*, std::string>> game_parms;
            game_parms.push_back(std::make_pair(&pgr.game_parameter1_value, "game_parameter1_value"));
            game_parms.push_back(std::make_pair(&pgr.game_parameter2_value, "game_parameter2_value"));
            game_parms.push_back(std::make_pair(&pgr.game_parameter3_value, "game_parameter3_value"));
            game_parms.push_back(std::make_pair(&pgr.game_parameter4_value, "game_parameter4_value"));

            for (std::pair<int*, std::string> game_parm : game_parms) {
                if (player_info.has(game_parm.second)) {
                    try {
                        *game_parm.first = player_info[game_parm.second].i();
                    } catch(...) {
                        std::string error = "No stats were added due to incorrect format of player value for player " +
                            pemail + " and parameter " + game_parm.second;
                        return crow::response(417, error);
                    }
                } else {
                    *game_parm.first = 0;
                }
            }

            pgrs.push_back(pgr);
        }

        // Adding stats that we know they are in the correct format
        for (Player_Game_Ratings pgr : pgrs) {
            pgr = DB->add_player_rating(pgr);
            if (!pgr.is_valid) {
                std::string error = "Internal Server Error due to player " + pgr.player_email;
                return crow::response(500, error);
            }
        }

        if (pgrs.size() > 0) {
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
        player_email [String] REQUIRED
    Return Body:
        players [JSON with PlayerGameRating Objects]
*/
crow::response APIEndPoints::getPlayerStats(const crow::request& req, int game_id, std::string player_email) {
    try {
      Player_Game_Ratings pgr;
      crow::json::wvalue stats;
      std::string pemail;

      // Authenticating user
      std::pair<int, std::string> tokenInfo = authenticateTokenGetErrorCode(req);
      if (tokenInfo.first != 200) {
        return crow::response(tokenInfo.first, tokenInfo.second);
      }

      // Authenticating user has access to game
      if (!developerOwnsGame(tokenInfo.second, game_id)) {
        std::string error = "Do not have access to game with game_id " + std::to_string(game_id);
        return crow::response(403, error);
      }

      pgr = DB->get_player_game_rating(player_email, game_id);
      if (!pgr.is_valid) {
        return crow::response(404, "The given player does not have ratings for this game");
      } else {
        // Converting players' stats to json object
        stats["game_parameter1_value"] = pgr.game_parameter1_value;
        stats["game_parameter2_value"] = pgr.game_parameter2_value;
        stats["game_parameter3_value"] = pgr.game_parameter3_value;
        stats["game_parameter4_value"] = pgr.game_parameter4_value;
      }
      return crow::response(200, stats);
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
}

/*
    Delete requested players' stats for a given game
    Request Parameters:
        game_id [Integer] REQUIRED
        player_email [String] REQUIRED
    Return Body:
        return_message [String]
*/
crow::response APIEndPoints::deletePlayerStats(const crow::request& req, int game_id, std::string player_email) {
  try {
    Player_Game_Ratings pgr;
    // Authenticating user
    std::pair<int, std::string> tokenInfo = authenticateTokenGetErrorCode(req);
    if (tokenInfo.first != 200) {
        return crow::response(tokenInfo.first, tokenInfo.second);
    }

    // Authenticating user has access to game
    if (!developerOwnsGame(tokenInfo.second, game_id)) {
        std::string error = "Do not have access to game with game_id " + std::to_string(game_id);
        return crow::response(403, error);
    }

    pgr = DB->get_player_game_rating(player_email, game_id);
    if (!pgr.is_valid) {
      return crow::response(404, "The given player does not have ratings for this game");
    }

    pgr = DB->remove_player_rating(player_email, game_id);
    if (!pgr.is_valid) {
      return crow::response(500, "Internal Server Error");
    }

    return crow::response(200, "Player stats were removed");
  } catch (...) {
    return crow::response(400, "Invalid request body");
  }
}

/*
    Update requested players' stats for a given game
    Request Parameters:
        game_id [Integer] REQUIRED
        player_email [String] REQUIRED
    Request Body:
        stats [Json with PlayerGameRating Objects] REQUIRED
    Return Body:
        return_message [String]
*/
crow::response APIEndPoints::updatePlayerStats(const crow::request& req, int game_id, std::string player_email) {
  try {
    crow::json::rvalue user_req = crow::json::load(req.body);
    Player_Game_Ratings pgr;

    // Authenticating user
    std::pair<int, std::string> tokenInfo = authenticateTokenGetErrorCode(req);
    if (tokenInfo.first != 200) {
        return crow::response(tokenInfo.first, tokenInfo.second);
    }

    // Authenticating user has access to game
    if (!developerOwnsGame(tokenInfo.second, game_id)) {
        std::string error = "Do not have access to game with game_id " + std::to_string(game_id);
        return crow::response(403, error);
    }

    std::vector<std::pair<int*, std::string>> game_parms;
    game_parms.push_back(std::make_pair(&pgr.game_parameter1_value, "game_parameter1_value"));
    game_parms.push_back(std::make_pair(&pgr.game_parameter2_value, "game_parameter2_value"));
    game_parms.push_back(std::make_pair(&pgr.game_parameter3_value, "game_parameter3_value"));
    game_parms.push_back(std::make_pair(&pgr.game_parameter4_value, "game_parameter4_value"));

    for (std::pair<int*, std::string> game_parm : game_parms) {
        if (user_req.has(game_parm.second)) {
            try {
                *game_parm.first = user_req[game_parm.second].i();
            } catch(...) {
                std::string error = "No stats were updated due to incorrect format of player value for player "
                    + player_email + " and parameter " + game_parm.second;
                return crow::response(417, error);
            }
        } else {
            *game_parm.first = 0;
        }
    }

    pgr.player_email = player_email;
    pgr.game_id = game_id;

    Player_Game_Ratings pgr_existing_player = DB->get_player_game_rating(player_email, game_id);
    if (pgr_existing_player.is_valid) {
        pgr = DB->update_player_rating(player_email, game_id, pgr);

        if (!pgr.is_valid) {
            std::string error = "Internal Server Error due to player " + pgr.player_email;
            return crow::response(500, error);
        }
    } else {
        return crow::response(404, "The given player does not have ratings for this game");
    }

    return crow::response(200, "Player stats were updated");
  } catch(...) {
      return crow::response(400, "Invalid request body");
  }
}

crow::response APIEndPoints::postSignUp(const crow::request& req) {
  crow::json::rvalue x = crow::json::load(req.body);
  Developer D;
  try {
    D.developer_email = x["developer_email"].s();
    D.developer_password = x["developer_password"].s();
    D.developer_password = auth->encryptPassword(D.developer_password);

    D = DB->add_developer(D);
    if (!D.is_valid) {
      return crow::response(409, "Developer already exists");
    }
    return crow::response(200, "Succesfully signed up, please login to get your API key.");
  } catch(...) {
    return crow::response(400, "Invalid request body");
  }
}

crow::response APIEndPoints::postLogin(const crow::request& req) {
  crow::json::rvalue x = crow::json::load(req.body);
  std::string developer_email;
  std::string developer_password;
  Developer D;

  try {
    developer_email = x["developer_email"].s();
    developer_password = x["developer_password"].s();
    D = DB->get_developer(developer_email);
    if (!D.is_valid) {
      return crow::response(404, "Developer does not exist");
    }

    if (!auth->validatePassword(developer_password, D.developer_password)) {
      return crow::response(401, "Invalid credentials");
    }

    std::string token = auth->createJWT(D.developer_email);
    return crow::response(200,
    "Success please put the following in the authorization header of you requests: Bearer " + token);
  } catch(...) {
    return crow::response(400, "Invalid request body");
  }
}

crow::response APIEndPoints::deleteLogin(const crow::request& req) {
  crow::json::rvalue x = crow::json::load(req.body);
  std::string developer_email;
  Developer D;

  try {
    std::pair<int, std::string> tokenInfo = authenticateTokenGetErrorCode(req);
    if (tokenInfo.first != 200) {
      return crow::response(tokenInfo.first, tokenInfo.second);
    }

    std::string developer_email = tokenInfo.second;

    D = DB->remove_developer(developer_email);
    if (!D.is_valid) {
      return crow::response(404, "Developer does not exist");
    }

    return crow::response(200, "Succesfully deleted account");
  } catch(...) {
    return crow::response(500, "Internal Server Error");
  }
}

crow::response APIEndPoints::matchmake(const crow::request& req, Matchmaking* M) {
    crow::json::rvalue request_body = crow::json::load(req.body);

    int game_id;
    crow::json::rvalue input_player_emails_rvalue;
    std::vector<crow::json::rvalue> input_player_emails;
    std::string developer_email;

    try {
      crow::json::wvalue json_result;

      // Authentication
      std::pair<int, std::string> tokenInfo = authenticateTokenGetErrorCode(req);
      if (tokenInfo.first != 200) {
        return crow::response(tokenInfo.first, tokenInfo.second);
      }

      developer_email = tokenInfo.second;

      // Check to ensure that the developer "owns" the given game
      game_id = request_body["game_id"].i();
      std::vector<Game_Details> developer_games = DB->get_all_games_for_developer(developer_email);
      bool game_found = false;

      for (uint64_t i = 0; i < developer_games.size(); i++) {
        int g_id = developer_games.at(i).game_id;
        if (game_id == g_id)
          game_found = true;
      }

      if (game_found == false)
        return crow::response(400, "Given Game ID does not belong to the Given Developer.\n");

      // Check the Passed Player Emails for Two Reasons:
      // 1) All passed players exist with our database
      // 2) All passed players are unique
      // 3) All players have stats for the given game
      input_player_emails_rvalue = request_body["player_emails"];
      input_player_emails = input_player_emails_rvalue.lo();

      std::vector<std::string> player_emails;
      std::vector<std::string> nonexistent_emails;
      for (crow::json::rvalue email : input_player_emails) {
        Player input_email = DB->get_player(email.s());
        if (input_email.is_valid == true)
            player_emails.push_back(email.s());
        else
            nonexistent_emails.push_back(email.s());
      }

      if (nonexistent_emails.size() > 0) {
        std::string error_message = "The following player IDs were not found: ";
        for (uint64_t i = 0; i < nonexistent_emails.size(); i++) {
            if (i == nonexistent_emails.size() - 1)
                error_message += nonexistent_emails.at(i) + "\n";
            else
                error_message += nonexistent_emails.at(i) + ", ";
        }
        return crow::response(400, error_message);
      }

      std::set<std::string> duplicate_emails_set;
      for (uint64_t i = 0; i < player_emails.size(); i++) {
        for (uint64_t j = 0; j < player_emails.size(); j++) {
            if ((i != j) && (player_emails.at(i) == player_emails.at(j)))
                duplicate_emails_set.insert(player_emails.at(i));
        }
      }

      uint64_t duplicate_iterator = 0;
      if (duplicate_emails_set.size() > 0) {
        std::string error_message = "The following player IDs were found multiple times in the input: ";
        for (std::set<std::string>::iterator i = duplicate_emails_set.begin(); i != duplicate_emails_set.end(); i++) {
            if ((duplicate_iterator == duplicate_emails_set.size() - 1) || (duplicate_emails_set.size() == 1))
                error_message += *i + "\n";
            else
                error_message += *i + ", ";
            duplicate_iterator++;
        }
        return crow::response(400, error_message);
      }

      std::string matchmaking_type = request_body["matchmaking_type"].s();

      std::tuple<
      std::vector<std::vector<std::vector<std::string> > >,
      std::vector<std::string> > result;

      if (matchmaking_type == "basic")
        result = M->matchmakingBackendBasic(game_id, player_emails, DB);
      else if (matchmaking_type == "advanced")
        result = M->matchmakingBackendAdvanced(game_id, player_emails, DB);
      else
        return crow::response(400, "Matchmaking Type Not Found.\n");

      std::vector<std::vector<std::vector<std::string> > > games = std::get<0>(result);
      std::vector<std::string> overflow = std::get<1>(result);

      for (uint64_t i = 0; i < games.size(); i++) {
        json_result["Game_" + std::to_string(i + 1)];
        for (uint64_t j = 0; j < games.at(i).size(); j++) {
          std::vector<std::string> player_names;

          for (uint64_t k = 0; k < games.at(i).at(j).size(); k++) {
            player_names.push_back(games.at(i).at(j).at(k));
          }

          json_result["Game_" + std::to_string(i + 1)]["Team_" + std::to_string(j + 1)] = player_names;
        }
      }

      json_result["overflow"] = overflow;

      return crow::response(200, json_result);
    }
    catch(...) {
      // Return a response to indicate an invalid request body
      crow::json::wvalue json_result;
      return crow::response(400, "Incorrect Request Format.\n");
    }
}

// return everything about the game details to the user except game_id and developer_email
crow::response APIEndPoints::getGame(const crow::request& req, int game_id) {
    // Authentication
    std::pair<int, std::string> tokenInfo = authenticateTokenGetErrorCode(req);
    if (tokenInfo.first != 200) {
        return crow::response(tokenInfo.first, tokenInfo.second);
    }

    std::string developer_email = tokenInfo.second;
    Game_Details game_details;
    try {
      crow::json::rvalue user = crow::json::load(req.body);
      if (!developerOwnsGame(developer_email, game_id)) {
        return crow::response(403, "Invalid authentication");
      }


      game_details = DB->get_game_details(game_id);
      crow::json::wvalue game_json;
      if (game_details.is_valid) {
        game_json["game_id"] = game_details.game_id;
        game_json["game_name"] = game_details.game_name;
        game_json["game_parameter1_name"] = game_details.game_parameter1_name;
        game_json["game_parameter1_weight"] = game_details.game_parameter1_weight;
        game_json["game_parameter2_name"] = game_details.game_parameter2_name;
        game_json["game_parameter2_weight"] = game_details.game_parameter2_weight;
        game_json["game_parameter3_name"] = game_details.game_parameter3_name;
        game_json["game_parameter3_weight"] = game_details.game_parameter3_weight;
        game_json["game_parameter4_name"] = game_details.game_parameter4_name;
        game_json["game_parameter4_weight"] = game_details.game_parameter4_weight;
        game_json["category"] = game_details.category;
        game_json["players_per_team"] = game_details.players_per_team;
        game_json["teams_per_match"] = game_details.teams_per_match;

      } else {
        return crow::response(404, "Game not found");
      }

      return crow::response(200, game_json);
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
}




// create set for PUT request to not reset all elements and to catch errors
crow::response APIEndPoints::putGame(const crow::request& req, int game_id) {
    // Authentication
    std::pair<int, std::string> tokenInfo = authenticateTokenGetErrorCode(req);
    if (tokenInfo.first != 200) {
        return crow::response(tokenInfo.first, tokenInfo.second);
    }

    std::string developer_email = tokenInfo.second;
    try {
        crow::json::rvalue game_body = crow::json::load(req.body);
        if (!developerOwnsGame(developer_email, game_id)) {
            return crow::response(403, "Invalid authentication");
        }
        std::set<std::string> all_keys = {"game_name",
        "game_parameter1_name", "game_parameter1_weight",
        "game_parameter2_name", "game_parameter2_weight",
        "game_parameter3_name", "game_parameter3_weight",
        "game_parameter4_name", "game_parameter4_weight",
        "category", "players_per_team", "teams_per_match"};

        Game_Details game_details = DB->get_game_details(game_id);
        for (std::string p : game_body.keys()) {
            game_details.game_id = game_id;
            if (p == "game_name") {
            game_details.game_name = game_body["game_name"].s();
            } else if (p == "game_parameter1_name") {
            game_details.game_parameter1_name = game_body["game_parameter1_name"].s();
            } else if (p == "game_parameter1_weight") {
            game_details.game_parameter1_weight = game_body["game_parameter1_weight"].d();
            } else if (p == "game_parameter2_name") {
            game_details.game_parameter2_name = game_body["game_parameter2_name"].s();
            } else if (p == "game_parameter2_weight") {
            game_details.game_parameter2_weight = game_body["game_parameter2_weight"].d();
            } else if (p == "game_parameter3_name") {
            game_details.game_parameter3_name = game_body["game_parameter3_name"].s();
            } else if (p == "game_parameter3_weight") {
            game_details.game_parameter3_weight = game_body["game_parameter3_weight"].d();
            } else if (p == "game_parameter4_name") {
            game_details.game_parameter4_name = game_body["game_parameter4_name"].s();
            } else if (p == "game_parameter4_weight") {
            game_details.game_parameter4_weight = game_body["game_parameter4_weight"].d();
            } else if (p == "category") {
            game_details.category = game_body["category"].s();
            } else if (p == "players_per_team") {
            game_details.players_per_team = game_body["players_per_team"].i();
            } else if (p == "teams_per_match") {
            game_details.teams_per_match = game_body["teams_per_match"].i();
            }
        }
        game_details.developer_email = developer_email;
        DB->update_game_details(game_id, game_details);
        return crow::response(200, "Added requested game details");
    } catch(...) {
    return crow::response(400, "Invalid request body");
    }
}


crow::response APIEndPoints::deleteGame(const crow::request& req, int game_id) {
    // Authentication
    std::pair<int, std::string> tokenInfo = authenticateTokenGetErrorCode(req);
    if (tokenInfo.first != 200) {
        return crow::response(tokenInfo.first, tokenInfo.second);
    }

    std::string developer_email = tokenInfo.second;
    Game_Details game_details;
    try {
      crow::json::rvalue x = crow::json::load(req.body);
      if (!developerOwnsGame(developer_email, game_id)) {
        return crow::response(403, "Invalid authentication");
      }
      game_details = DB->get_game_details(game_id);
      if (game_details.is_valid) {
        DB->remove_game_details(game_details.game_id);
      } else {
        return crow::response(400, "Invalid request body");
      }
      return crow::response(200, "Succesfully deleted game details");
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
}



