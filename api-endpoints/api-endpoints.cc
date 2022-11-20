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
    Developer d = DB->get_developer(root["developer_email"].asString());
    if (!d.is_valid) {
        return std::make_pair(401, std::string("developer not found."));
    }
    if (d.developer_password.compare(root["developer_password"].asString()) != 0) {
        return std::make_pair(401, std::string("password doesn't match."));
    }

    return std::make_pair(200, root["developer_email"].asString());
}


std::pair<bool, std::string> APIEndPoints::authenticateToken(const crow::request& req) {
  std::string header;
  std::string token;
  Developer D;

  // To catch missing token or header
  try {
    header = req.get_header_value("Authorization");
    token = header.substr(7);
  } catch (...) {
    return std::make_pair(false, "Invalid Header");
  }

  // Validation logic
  try {
    std::pair<bool, std::string> decodedToken = auth->decodeAndVerifyJWT(token);
    if (decodedToken.first == false) {
      return std::make_pair(false, decodedToken.second);
    }

    D = DB->get_developer(decodedToken.second);
    if (!D.is_valid) {
      // Developer might have been deleted
      return std::make_pair(false, "Developer does not exist");
    }

    return std::make_pair(true, D.developer_email);
  } catch (...) {
    // Should never be hit unless something really goes wrong
    return std::make_pair(false, "Internal Server Error");
  }
}

std::pair<int, std::string> APIEndPoints::authenticateTokenGetErrorCode(const crow::request& req) {
  std::string header;
  std::string token;
  Developer D;

  // To catch missing token or header
  try {
    header = req.get_header_value("Authorization");
    token = header.substr(7);
  } catch (...) {
    return std::make_pair(400, "Invalid Header");
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
      return std::make_pair(401, "Developer does not exist");
    }

    return std::make_pair(200, D.developer_email);
  } catch (...) {
    // Should never be hit unless something really goes wrong
    return std::make_pair(500, "Internal Server Error");
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
    std::vector<Game_Details> games;

    // authentication
    std::pair<int, std::string> tokenInfo = authenticateTokenGetErrorCode(req);
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

crow::response APIEndPoints::postSignUp(const crow::request& req) {
  crow::json::rvalue x = crow::json::load(req.body);
  Developer D;
  try {
    D.developer_email = x["developer_email"].s();
    D.developer_password = x["developer_password"].s();
    D.developer_password = auth->encryptPassword(D.developer_password);

    D = DB->add_developer(D);
    if (!D.is_valid) {
      return crow::response(400, "Developer already exists");
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
      return crow::response(400, "Developer does not exist");
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
    std::pair<int, std::string> tokenInfo = authenticateToken(req);
    if (tokenInfo.first == false) {
      return crow::response(401, tokenInfo.second);
    }

    std::string developer_email = tokenInfo.second;

    D = DB->remove_developer(developer_email);
    if (!D.is_valid) {
      return crow::response(500, "Developer does not exist");
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
      std::pair<bool, std::string> tokenInfo = authenticateToken(req);
      if (tokenInfo.first == false) {
        return crow::response(401, tokenInfo.second);;
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
