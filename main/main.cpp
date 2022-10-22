// "Copyright [2022] <Copyright Owner>"

#include "crow/crow_all.h"
#include "main/db-service.h"

int main(void) {
  DBService DB = DBService();
  DB.get_player("apex_player@gmail.com");
  std::cout << "\n";
  DB.get_developer("developer@chess.com");
  std::cout << "\n";
  DB.get_game_details(2);
  std::cout << "\n";
  DB.get_player_game_rating("apex_player@gmail.com", 1);
  std::cout << "\n";
  DB.get_joined_player_game_rating("apex_player@gmail.com", 1);
  std::cout << "\n";
  DB.get_all_players();
  std::cout << "\n";
  DB.get_all_developers();
  std::cout << "\n";
  DB.get_all_games();
  std::cout << "\n";
  DB.get_all_player_game_ratings_for_game(1);
  std::cout << "\n";
  DB.get_all_games_for_developer("developer@chess.com");
  std::cout << "\n";

  crow::SimpleApp app;  // define your crow application

  // define your endpoint at the root directory
  CROW_ROUTE(app, "/")([](){
    return "Hello world";
  });

  // Login doesn't do much since we are always expecting to get the
  // email and password with every request
  CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req){
    crow::json::rvalue x = crow::json::load(req.body);
    std::string developer_email;
    std::string developer_password;
    Developer D;

    try {
      developer_email = x["developer_email"].s();
      developer_password = x["developer_password"].s();
      DBService DB = DBService();
      D = DB.get_developer(developer_email);
      if (!D.is_valid || D.developer_password != developer_password) {
        return crow::response(400, "Invalid credentials");
      }
      return crow::response(200, "Succesfully logged in");
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
  });

  CROW_ROUTE(app, "/signup").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req){
    crow::json::rvalue x = crow::json::load(req.body);
    Developer D;
    try {
      D.developer_email = x["developer_email"].s();
      D.developer_password = x["developer_password"].s();
      DBService DB = DBService();
      D = DB.add_developer(D);
      if (!D.is_valid) {
        return crow::response(400, "Developer already exists");
      }
      return crow::response(200, "Succesfully signed up");
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
  });

  CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::DELETE)
  ([](const crow::request& req){
    crow::json::rvalue x = crow::json::load(req.body);
    std::string developer_email;
    std::string developer_password;
    Developer D;

    try {
      developer_email = x["developer_email"].s();
      developer_password = x["developer_password"].s();
      DBService DB = DBService();
      D = DB.get_developer(developer_email);
      if (!D.is_valid) {
        return crow::response(400, "User not found");
      }

      if (D.developer_password != developer_password) {
        return crow::response(400, "Invalid credentials");
      }

      D = DB.remove_developer(developer_email);
      if (!D.is_valid) {
        return crow::response(500, "Internal Server Error");
      }
      return crow::response(200, "Succesfully deleted account");
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
  });

//return everything about the game details to the user except game_id and developer_email
  CROW_ROUTE(app, "/game/<int>").methods(crow::HTTPMethod::GET)
  ([](int game_id){
    Game_Details game_details;
    try {
      game_details = get_game_details(game_id);
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

  //create set for PUT request to not reset all elements and to catch errors
  CROW_ROUTE(app, "/game/<int>").methods(crow::HTTPMethod::PUT)
  ([](const crow::request& req, int game_id){
    try {
      crow::json::rvalue game_body = crow::json::load(req.body);
      std::set<std::string> all_keys = {"game_name", "developer_email"
      "game_parameter1_name", "game_parameter1_weight", 
      "game_parameter2_name", "game_parameter2_weight",
      "game_parameter3_name", "game_parameter3_weight",
      "game_parameter4_name", "game_parameter4_weight",
      "category", "players_per_team", "teams_per_match"};

      Game_Details game_details;
      for (std::string p : game_body.keys()) {
        if (all_keys.count(p)) {
            game_details.game_id = game_id;
            if (p == "game_name") {
              game_details.game_name = game_body["game_name"].s();
            }
            else if (p == "developer_email") {
              game_details.developer_email = game_body["developer_email"].s();
            }
            else if (p == "game_parameter1_name") {
              game_details.game_parameter1_name = game_body["game_parameter1_name"].s();
            }
            else if (p == "game_parameter1_weight") {
              game_details.game_parameter1_weight = game_body["game_parameter1_weight"].d();
            }
            else if (p == "game_parameter2_name") {
              game_details.game_parameter2_name = game_body["game_parameter2_name"].s();
            }
            else if (p == "game_parameter2_weight") {
              game_details.game_parameter2_weight = game_body["game_parameter2_weight"].d();
            }
            else if (p == "game_parameter3_name") {
              game_details.game_parameter3_name = game_body["game_parameter3_name"].s();
            }
            else if (p == "game_parameter3_weight") {
              game_details.game_parameter3_weight = game_body["game_parameter3_weight"].d();
            }
            else if (p == "game_parameter4_name") {
              game_details.game_parameter4_name = game_body["game_parameter4_name"].s();
            }
            else if (p == "game_parameter4_weight") {
              game_details.game_parameter4_weight = game_body["game_parameter4_weight"].d();
            }
            else if (p == "category") {
              game_details.category = game_body["category"].s();
            }
            else if (p == "players_per_team") {
              game_details.players_per_team = game_body["players_per_team"].i();
            }
            else if (p == "teams_per_match") {
              game_details.teams_per_match = game_body["teams_per_match"].i();
            }
            else {
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
    crow::json::rvalue x = crow::json::load(req.body);
    Game_Details game_details;
    try {
      game_details = get_game_details(game_id);
      if (game_details.is_valid) {
        remove_game_details(game_details.game_id);
      }
      else {
        return crow::response(400, "Invalid request body");
      }
      return crow::response(200, "Succesfully deleted game details");
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
  });

  // set the port, set the app to run on multiple threads, and run the app
  app.port(18080).multithreaded().run();
  return 0;
}
