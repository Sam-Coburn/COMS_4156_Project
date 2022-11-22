// Copyright [2022] RaisingCanesFanClub
#include "api-endpoints/api-endpoints-bandaid-lib.h"

bool APIEndPointsBandaid::valid_user(std::string email, std::string password = "never used") {
  // assume bandaid check already ran
  return true;
}

bool APIEndPointsBandaid::valid_user_gameid(std::string email, int game_id) {
  if (!valid_user(email)) {
    return false;
  }


  Game_Details gd = DB->get_game_details(game_id);
  return gd.is_valid && gd.developer_email == email;
}

crow::response APIEndPointsBandaid::matchmake(const crow::request& req, Matchmaking *M) {
  // authentication from bandaid
  std::pair<int, std::string> tokenInfo = authenticateTokenGetErrorCode(req);
  if (tokenInfo.first != 200) {
    return crow::response(tokenInfo.first, tokenInfo.second);
  }
  // ^^ added by bandaid ^^

    crow::json::rvalue request_body = crow::json::load(req.body);

    std::string developer_email;
    // std::string developer_password;
    int game_id;
    crow::json::rvalue input_player_emails_rvalue;
    std::vector<crow::json::rvalue> input_player_emails;

    try {
      crow::json::wvalue json_result;

      // // Authentication
      // developer_email = request_body["developer_email"].s();
      // developer_password = request_body["developer_password"].s();

      // Developer d = DB.get_developer(developer_email);

      // // Check given password against password in DB
      // if (d.developer_password != developer_password)
      //   return crow::response(400, "Incorrect Credentials Given.\n");

      game_id = request_body["game_id"].i();
      std::vector<Game_Details> developer_games = DB->get_all_games_for_developer(developer_email);
      bool game_found = false;

      // Check to ensure that the developer "owns" the given game
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
            if ((i != j) && (player_emails.at(i) == player_emails.at(j))) {
                duplicate_emails_set.insert(player_emails.at(i));
            }
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

      std::tuple<
      std::vector<std::vector<std::vector<std::string> > >,
      std::vector<std::string> > result = M->matchmakingBackendAdvanced(game_id, player_emails, DB);

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
  // Get all players in a certain game
crow::response APIEndPointsBandaid::getGamePlayers(const crow::request& req, int game_id) {
  // authentication from bandaid
  std::pair<int, std::string> tokenInfo = authenticateTokenGetErrorCode(req);
  if (tokenInfo.first != 200) {
    return crow::response(tokenInfo.first, tokenInfo.second);
  }
  // ^^ added by bandaid ^^
      crow::json::rvalue user_info = crow::json::load(req.body);
    std::vector<Joined_Player_Game_Ratings> joined_players;

    try {
      // Authenticating user
      // std::string dev_email = user_info["developer_email"].s();
      // std::string dev_password = user_info["developer_password"].s();
      // if (!valid_user_gameid(dev_email, dev_password, game_id)) {
      //   return crow::response(401, "Invalid authentication");
      // }


      joined_players = DB->get_all_player_game_ratings_for_game(game_id);

      if (joined_players.empty()) {
        return crow::response(204,
        "No players found for game_id " + std::to_string(game_id));
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
          return crow::response(500,
          "Internal Server Error due to player " + p.player_email);
        }
      }

      return crow::response(200, players);
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
}

// return everything about the game details to the user except game_id and developer_email
crow::response APIEndPointsBandaid::getGame(const crow::request& req, int game_id) {
  // authentication from bandaid
  std::pair<int, std::string> tokenInfo = authenticateTokenGetErrorCode(req);
  if (tokenInfo.first != 200) {
    return crow::response(tokenInfo.first, tokenInfo.second);
  }
  // ^^ added by bandaid ^^
      Game_Details game_details;
    try {
      // Authentication
      // crow::json::rvalue user = crow::json::load(req.body);
      // std::string email = user["developer_email"].s();
      // std::string password = user["developer_password"].s();
      // if (!valid_user_gameid(email, password, game_id)) {
      //   return crow::response(401, "Invalid authentication");
      // }


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
        return crow::response(400, "Invalid request body");
      }

      return crow::response(200, game_json);
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
}

// add player stats for a certain game
crow::response APIEndPointsBandaid::postGamePlayers(const crow::request& req, int game_id) {
  // authentication from bandaid
  std::pair<int, std::string> tokenInfo = authenticateTokenGetErrorCode(req);
  if (tokenInfo.first != 200) {
    return crow::response(tokenInfo.first, tokenInfo.second);
  }
  // ^^ added by bandaid ^^
      try {
      crow::json::rvalue user_req = crow::json::load(req.body);

      // Authenticating user
      // std::string dev_email = user_req["developer_email"].s();
      // std::string dev_password = user_req["developer_password"].s();
      // if (!valid_user_gameid(dev_email, dev_password, game_id)) {
      //   return crow::response(401, "Invalid authentication");
      // }

      crow::json::rvalue player_info;
      bool added = false;
      for (std::string pemail : user_req.keys()) {
        if (pemail != "developer_email" && pemail != "developer_password") {
          // If player does not already exist, add player to database
          Player p = DB->get_player(pemail);
          p.player_email = pemail;
          if (!p.is_valid) {
            p = DB->add_player(p);
            if (!p.is_valid) {
              return crow::response(500,
              "Internal Server Error due to player " + pemail);
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

          pgr = DB->add_player_rating(pgr);
          if (!pgr.is_valid) {
            return crow::response(500,
            "Internal Server Error due to player " + pemail);
          } else {
            added = true;
          }
        }
      }

      if (added) {
        return crow::response(200, "Player stats were added");
      } else {
        return crow::response(200,
        "No player stats added due to empty request");
      }
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
}

// create set for PUT request to not reset all elements and to catch errors
crow::response APIEndPointsBandaid::putGame(const crow::request& req, int game_id) {
  // authentication from bandaid
  std::pair<int, std::string> tokenInfo = authenticateTokenGetErrorCode(req);
  if (tokenInfo.first != 200) {
    return crow::response(tokenInfo.first, tokenInfo.second);
  }
  // ^^ added by bandaid ^^
  try {
      crow::json::rvalue game_body = crow::json::load(req.body);
      // Authentication
     // std::string email = game_body["developer_email"].s();
    //  std::string password = game_body["developer_password"].s();
      // if (!valid_user_gameid(email, game_id)) {
      //   return crow::response(401, "Invalid authentication");
      // }
      std::set<std::string> all_keys = {"game_name", "developer_email"
      "game_parameter1_name", "game_parameter1_weight",
      "game_parameter2_name", "game_parameter2_weight",
      "game_parameter3_name", "game_parameter3_weight",
      "game_parameter4_name", "game_parameter4_weight",
      "category", "players_per_team", "teams_per_match",
      "developer_email", "developer_password"};

      Game_Details game_details;
      for (std::string p : game_body.keys()) {
        if (all_keys.count(p)) {
            game_details.game_id = game_id;
            if (p == "game_name") {
              game_details.game_name = game_body["game_name"].s();
            } else if (p == "developer_email") {
              game_details.developer_email = game_body["developer_email"].s();
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
            } else {
              return crow::response(400, "Invalid request body");
            }

        } else {
          return crow::response(400, "Invalid request body");
        }
      }

      return crow::response(200, "Added requested game details");
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
}

// Get a specific player's stats for a specific game
crow::response APIEndPointsBandaid::getGamePlayer(const crow::request& req, int game_id, std::string player_email) {
  // authentication from bandaid
  std::pair<int, std::string> tokenInfo = authenticateTokenGetErrorCode(req);
  if (tokenInfo.first != 200) {
    return crow::response(tokenInfo.first, tokenInfo.second);
  }
  // ^^ added by bandaid ^^
  crow::json::rvalue user_req = crow::json::load(req.body);
  Player_Game_Ratings pgr;
  try {
      // Authenticating user
    //  std::string dev_email = user_req["developer_email"].s();
    //  std::string dev_password = user_req["developer_password"].s();
      // if (!valid_user_gameid(dev_email, game_id)) {
      //   return crow::response(401, "Invalid authentication");
      // }
      pgr = DB->get_player_game_rating(player_email, game_id);

      if (!pgr.is_valid) {
        return crow::response(204,
        "Player " + player_email + " not found for game_id " +
        std::to_string(game_id));
      }

      // Converting players' stats to json object
      crow::json::wvalue stats;
      if (pgr.is_valid) {
        stats["player_email"] = pgr.player_email;
        stats["game_parameter1_value"] = pgr.game_parameter1_value;
        stats["game_parameter2_value"] = pgr.game_parameter2_value;
        stats["game_parameter3_value"] = pgr.game_parameter3_value;
        stats["game_parameter4_value"] = pgr.game_parameter4_value;
      } else {
        return crow::response(500, "Internal Server Error");
      }

      return crow::response(200, stats);
    }
  catch(...) {
      return crow::response(400, "Invalid request body");
    }
}

crow::response APIEndPointsBandaid::deleteGame(const crow::request& req, int game_id) {
  // authentication from bandaid
  std::pair<int, std::string> tokenInfo = authenticateTokenGetErrorCode(req);
  if (tokenInfo.first != 200) {
    return crow::response(tokenInfo.first, tokenInfo.second);
  }
  // ^^ added by bandaid ^^
      Game_Details game_details;
    try {
      // Authentication
      crow::json::rvalue x = crow::json::load(req.body);
    //  std::string email = x["developer_email"].s();
     // std::string password = x["developer_password"].s();
      // if (!valid_user_gameid(email, game_id)) {
      //   return crow::response(401, "Invalid authentication");
      // }
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

// Remove given player stats for a certain game
crow::response APIEndPointsBandaid::deleteGamePlayer(const crow::request& req, int game_id) {
  // authentication from bandaid
  std::pair<int, std::string> tokenInfo = authenticateTokenGetErrorCode(req);
  if (tokenInfo.first != 200) {
    return crow::response(tokenInfo.first, tokenInfo.second);
  }
  // ^^ added by bandaid ^^
    crow::json::rvalue user_req = crow::json::load(req.body);
    crow::json::rvalue player_emails_rvalue;
    std::vector<crow::json::rvalue> player_emails;
    try {
      // Authenticating user
      std::string dev_email = user_req["developer_email"].s();
     // std::string dev_password = user_req["developer_password"].s();
      if (!valid_user_gameid(dev_email, game_id)) {
        return crow::response(401, "Invalid authentication");
      }
      player_emails_rvalue = user_req["player_emails"];
      player_emails = player_emails_rvalue.lo();
      for (crow::json::rvalue email_rvalue : player_emails) {
        std::string email = email_rvalue.s();
        Player_Game_Ratings pgr;
        pgr = DB->remove_player_rating(email, game_id);
        if (!pgr.is_valid) {
          return crow::response(500,
          "Internal Server Error due to player " + email);
        }
      }

      return crow::response(200, "Player stats were removed");
    } catch (...) {
      return crow::response(400, "Invalid request body");
    }
}

// base class stubs

crow::response APIEndPoints::getGamePlayers(const crow::request& req, int game_id) {
  return crow::response(400, "I'm a stub that exists to get overrided by bandaid.\n");
}
crow::response APIEndPoints::getGame(const crow::request& req, int game_id) {
  return crow::response(400, "I'm a stub that exists to get overrided by bandaid.\n");
}
crow::response APIEndPoints::postGamePlayers(const crow::request& req, int game_id) {
  return crow::response(400, "I'm a stub that exists to get overrided by bandaid.\n");
}
crow::response APIEndPoints::putGame(const crow::request& req, int game_id) {
  return crow::response(400, "I'm a stub that exists to get overrided by bandaid.\n");
}
crow::response APIEndPoints::getGamePlayer(const crow::request& req, int game_id, std::string player_email) {
  return crow::response(400, "I'm a stub that exists to get overrided by bandaid.\n");
}
crow::response APIEndPoints::deleteGame(const crow::request& req, int game_id) {
  return crow::response(400, "I'm a stub that exists to get overrided by bandaid.\n");
}
crow::response APIEndPoints::deleteGamePlayer(const crow::request& req, int game_id) {
  return crow::response(400, "I'm a stub that exists to get overrided by bandaid.\n");
}