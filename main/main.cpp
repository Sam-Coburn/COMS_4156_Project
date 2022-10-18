// "Copyright [2022] <Copyright Owner>"

#include "crow/crow_all.h"
#include "main/db-service.h"
#include "api-endpoints/api-endpoints-lib.h"

int main(void) {
  get_player("apex_player@gmail.com");
  std::cout << "\n";
  get_developer("developer@chess.com");
  std::cout << "\n";
  get_game_details(2);
  std::cout << "\n";
  get_player_game_rating("apex_player@gmail.com", 1);
  std::cout << "\n";
  get_joined_player_game_rating("apex_player@gmail.com", 1);
  std::cout << "\n";
  get_all_players();
  std::cout << "\n";
  get_all_developers();
  std::cout << "\n";
  get_all_games();
  std::cout << "\n";
  get_all_player_game_ratings_for_game(1);
  std::cout << "\n";
  get_all_games_for_developer("developer@chess.com");
  std::cout << "\n";

  Player P;
  P.player_email = "fake_player@gmail.com";
  P = add_player(P);
  if (P.is_valid)
    remove_player("fake_player@gmail.com");

  Developer D;
  D.developer_email = "fake_developer@dev.com";
  D.developer_password = "some_password";
  D = add_developer(D);
  if (D.is_valid)
    remove_developer("fake_developer@dev.com");

  Game_Details GD;
  GD.game_name = "fake game";
  GD.developer_email = "developer@chess.com";
  GD = add_game_details(GD);
  if (GD.is_valid)
    remove_game_details(GD.game_id);

  Player_Game_Ratings PGR;
  PGR.player_email = "apex_player@gmail.com";
  PGR.game_id = 2;
  PGR = add_player_rating(PGR);
  if (PGR.is_valid)
    remove_player_rating(PGR.player_email, PGR.game_id);

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
      D = get_developer(developer_email);
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
      D = add_developer(D);
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
      D = get_developer(developer_email);
      if (!D.is_valid) {
        return crow::response(400, "User not found");
      }

      if (D.developer_password != developer_password) {
        return crow::response(400, "Invalid credentials");
      }

      D = remove_developer(developer_email);
      if (!D.is_valid) {
        return crow::response(500, "Internal Server Error");
      }
      return crow::response(200, "Succesfully deleted account");
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
  });

  CROW_ROUTE(app, "/games").methods(crow::HTTPMethod::GET)
  ([](const crow::request& req){
    std::pair<int, std::string> rsp = getGames(req);
    return crow::response(rsp.first, rsp.second);
  });

  CROW_ROUTE(app, "/games").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req){
    std::pair<int, std::string> rsp = postGames(req);
    return crow::response(rsp.first, rsp.second);
  });

  // set the port, set the app to run on multiple threads, and run the app
  app.port(18080).multithreaded().run();
  return 0;
}
