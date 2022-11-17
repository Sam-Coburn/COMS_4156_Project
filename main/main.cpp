// "Copyright [2022] <Copyright Owner>"

// Includes
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "crow/crow_all.h"
#include "main/db-service.h"
#include "api-endpoints/api-endpoints-lib.h"

APIEndPoints api = APIEndPoints();

/*
    Validate user using email and password
    ** Only for first iteration **
*/
bool valid_user(std::string email, std::string password) {
  DBService DB = DBService();
  Developer d = DB.get_developer(email);
  return d.is_valid && d.developer_password == password;
}

/*
    Validate user is authorized to access requested game
*/
bool valid_user_gameid(std::string email, std::string password, int game_id) {
  DBService DB = DBService();
  if (!valid_user(email, password)) {
    return false;
  }

  Game_Details gd = DB.get_game_details(game_id);
  return gd.is_valid && gd.developer_email == email;
}

int main(int argc, char* argv[]) {
  crow::SimpleApp app;  // define your crow application
  // define your endpoint at the root directory
  CROW_ROUTE(app, "/")([](){
    return "Hello world";
  });

  CROW_ROUTE(app, "/signup").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req){
    return api.postSignUp(req);
  });

  // Login doesn't do much since we are always expecting to get the
  // email and password with every request
  CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req){
    return api.postLogin(req);
  });

  CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::DELETE)
  ([](const crow::request& req){
    return api.deleteLogin(req);
  });

  CROW_ROUTE(app, "/games").methods(crow::HTTPMethod::GET)
  ([](const crow::request& req){
    std::pair<int, std::string> rsp = api.getGames(req);
    return crow::response(rsp.first, rsp.second);
       return crow::response(400, "Emily needs to use crow jsons.");
  });

  CROW_ROUTE(app, "/games").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req){
    std::pair<int, std::string> rsp = api.postGame(req);
    return crow::response(rsp.first, rsp.second);
  });

  /*
    Get all players for the requested game
  */
  CROW_ROUTE(app, "/game/<int>/players").methods(crow::HTTPMethod::GET)
  ([](const crow::request& req, int game_id){
    return api.getGamePlayers(req, game_id);
  });

  /*
    Add player stats for a game
  */
  CROW_ROUTE(app, "/game/<int>/players").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req, int game_id){
    return api.addPlayersStats(req, game_id);
  });

  /*
    Get specific players' stats for a game
  */
  CROW_ROUTE(app, "/game/<int>/players/<string>").methods(crow::HTTPMethod::GET)
  ([](const crow::request& req, int game_id, std::string player_email) {
    return api.getPlayersStats(req, game_id);
  });

  /*
    Delete requested players' stats for a given game
  */
  CROW_ROUTE(app, "/game/<int>/players").methods(crow::HTTPMethod::DELETE)
  ([](const crow::request& req, int game_id){
    return api.deletePlayersStats(req, game_id);
  });

  /*
    Update requested players' stats for a given game
  */
  CROW_ROUTE(app, "/game/<int>/players").methods(crow::HTTPMethod::PUT)
  ([](const crow::request& req, int game_id){
    return api.updatePlayersStats(req, game_id);
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

  CROW_ROUTE(app, "/matchmake").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req) {
    DBService DB = DBService();
    return api.matchmake(req, DB);
  });

  // set the port, set the app to run on multiple threads, and run the app
  app.port(18080).multithreaded().run();

  return 0;
}
