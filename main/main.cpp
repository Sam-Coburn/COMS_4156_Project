// "Copyright [2022] <Copyright Owner>"

// Includes
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "crow/crow_all.h"
#include "main/db-service.h"
#include "api-endpoints/api-endpoints-lib.h"

bool valid_user(std::string email, std::string password) {
  DBService DB = DBService();
  Developer d = DB.get_developer(email);
  return d.is_valid && d.developer_password == password;
}

bool valid_user_gameid(std::string email, std::string password, int game_id) {
  if (!valid_user(email, password)) {
    return false;
  }

  DBService DB = DBService();
  Game_Details gd = DB.get_game_details(game_id);
  return gd.is_valid && gd.developer_email == email;
}

int main(void) {

  crow::SimpleApp app;  // define your crow application
  // define your endpoint at the root directory
  CROW_ROUTE(app, "/")([](){
    return "Hello world";
  });

  CROW_ROUTE(app, "/signup").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req){
    APIEndPoints api = APIEndPoints();
    DBService DB = DBService();
    return api.postSignUp(req, &DB);
  });

  // Login doesn't do much since we are always expecting to get the
  // email and password with every request
  CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req){
    APIEndPoints api = APIEndPoints();
    DBService DB = DBService();
    return api.postLogin(req, &DB);
  });

  CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::DELETE)
  ([](const crow::request& req){
    APIEndPoints api = APIEndPoints();
    DBService DB = DBService();
    return api.deleteLogin(req, &DB);
  });

  CROW_ROUTE(app, "/games").methods(crow::HTTPMethod::GET)
  ([](const crow::request& req){
    APIEndPoints api = APIEndPoints();
    std::pair<int, std::string> rsp = api.getGames(req);
    return crow::response(rsp.first, rsp.second);
       return crow::response(400, "Emily needs to use crow jsons.");
  });

  CROW_ROUTE(app, "/games").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req){
    APIEndPoints api = APIEndPoints();
    std::pair<int, std::string> rsp = api.postGame(req);
    return crow::response(rsp.first, rsp.second);
  });

  // Get all players in a certain game
  CROW_ROUTE(app, "/game/<int>/players").methods(crow::HTTPMethod::GET)
  ([](const crow::request& req, int game_id){
    crow::json::rvalue user_info = crow::json::load(req.body);
    std::vector<Joined_Player_Game_Ratings> joined_players;

    try {
      // Authenticating user
      std::string dev_email = user_info["developer_email"].s();
      std::string dev_password = user_info["developer_password"].s();
      if (!valid_user_gameid(dev_email, dev_password, game_id)) {
        return crow::response(401, "Invalid authentication");
      }

      DBService DB = DBService();
      joined_players = DB.get_all_player_game_ratings_for_game(game_id);

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
  });

// return everything about the game details to the user except game_id and developer_email
  CROW_ROUTE(app, "/game/<int>").methods(crow::HTTPMethod::GET)
  ([](const crow::request& req, int game_id){
    Game_Details game_details;
    try {
      // Authentication
      crow::json::rvalue user = crow::json::load(req.body);
      std::string email = user["developer_email"].s();
      std::string password = user["developer_password"].s();
      if (!valid_user_gameid(email, password, game_id)) {
        return crow::response(401, "Invalid authentication");
      }

      DBService DB = DBService();
      game_details = DB.get_game_details(game_id);
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
  });

  // Add given player stats for a certain game
  CROW_ROUTE(app, "/game/<int>/players").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req, int game_id){
    try {
      DBService DB = DBService();
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

          pgr = DB.add_player_rating(pgr);
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
  });

  // create set for PUT request to not reset all elements and to catch errors
  CROW_ROUTE(app, "/game/<int>").methods(crow::HTTPMethod::PUT)
  ([](const crow::request& req, int game_id){
    try {
      crow::json::rvalue game_body = crow::json::load(req.body);
      // Authentication
      std::string email = game_body["developer_email"].s();
      std::string password = game_body["developer_password"].s();
      if (!valid_user_gameid(email, password, game_id)) {
        return crow::response(401, "Invalid authentication");
      }
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
  });

  // Get a specific player's stats for a specific game
  CROW_ROUTE(app, "/game/<int>/players/<string>").methods(crow::HTTPMethod::GET)
  ([](const crow::request& req, int game_id, std::string player_email) {
    crow::json::rvalue user_req = crow::json::load(req.body);
    Player_Game_Ratings pgr;

    try {
      // Authenticating user
      std::string dev_email = user_req["developer_email"].s();
      std::string dev_password = user_req["developer_password"].s();
      if (!valid_user_gameid(dev_email, dev_password, game_id)) {
        return crow::response(401, "Invalid authentication");
      }

      DBService DB = DBService();
      pgr = DB.get_player_game_rating(player_email, game_id);

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
  });

  CROW_ROUTE(app, "/game/<int>").methods(crow::HTTPMethod::DELETE)
  ([](const crow::request& req, int game_id){
    Game_Details game_details;
    try {
      // Authentication
      crow::json::rvalue x = crow::json::load(req.body);
      std::string email = x["developer_email"].s();
      std::string password = x["developer_password"].s();
      if (!valid_user_gameid(email, password, game_id)) {
        return crow::response(401, "Invalid authentication");
      }
      DBService DB = DBService();
      game_details = DB.get_game_details(game_id);
      if (game_details.is_valid) {
        DB.remove_game_details(game_details.game_id);
      } else {
        return crow::response(400, "Invalid request body");
      }
      return crow::response(200, "Succesfully deleted game details");
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
  });

  // Remove given player stats for a certain game
  CROW_ROUTE(app, "/game/<int>/players").methods(crow::HTTPMethod::DELETE)
  ([](const crow::request& req, int game_id){
    crow::json::rvalue user_req = crow::json::load(req.body);
    crow::json::rvalue player_emails_rvalue;
    std::vector<crow::json::rvalue> player_emails;
    try {
      // Authenticating user
      std::string dev_email = user_req["developer_email"].s();
      std::string dev_password = user_req["developer_password"].s();
      if (!valid_user_gameid(dev_email, dev_password, game_id)) {
        return crow::response(401, "Invalid authentication");
      }

      DBService DB = DBService();
      player_emails_rvalue = user_req["player_emails"];
      player_emails = player_emails_rvalue.lo();
      for (crow::json::rvalue email_rvalue : player_emails) {
        std::string email = email_rvalue.s();
        Player_Game_Ratings pgr;
        pgr = DB.remove_player_rating(email, game_id);
        if (!pgr.is_valid) {
          return crow::response(500,
          "Internal Server Error due to player " + email);
        }
      }

      return crow::response(200, "Player stats were removed");
    } catch (...) {
      return crow::response(400, "Invalid request body");
    }
  });

  CROW_ROUTE(app, "/matchmake").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req) {
    APIEndPoints api = APIEndPoints();
    DBService DB = DBService();
    Matchmaking M = Matchmaking();
    return api.matchmake(req, &DB, &M);
  });

  // set the port, set the app to run on multiple threads, and run the app
  app.port(18080).multithreaded().run();

  return 0;
}
