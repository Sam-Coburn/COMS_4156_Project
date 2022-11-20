// "Copyright [2022] <Copyright Owner>"

// Includes
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "crow/crow_all.h"
#include "main/db-service.h"
#include "api-endpoints/api-endpoints-lib.h"
#include "api-endpoints/api-endpoints-bandaid-lib.h"

#define CREATE_API_ENDPOINTS_OBJECT APIEndPoints api = APIEndPoints()
// #define CREATE_API_ENDPOINTS_OBJECT APIEndPointsBandaid api = APIEndPointsBandaid()

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

int main(int argc, char* argv[]) {
  crow::SimpleApp app;  // define your crow application
  // define your endpoint at the root directory
  CROW_ROUTE(app, "/")([](){
    return "Hello world";
  });

  CROW_ROUTE(app, "/signup").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req){
    CREATE_API_ENDPOINTS_OBJECT;
    return api.postSignUp(req);
  });

  // Login doesn't do much since we are always expecting to get the
  // email and password with every request
  CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req){
    CREATE_API_ENDPOINTS_OBJECT;
    return api.postLogin(req);
  });

  CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::DELETE)
  ([](const crow::request& req){
    CREATE_API_ENDPOINTS_OBJECT;
    return api.deleteLogin(req);
  });

  CROW_ROUTE(app, "/games").methods(crow::HTTPMethod::GET)
  ([](const crow::request& req){
    CREATE_API_ENDPOINTS_OBJECT;
    std::pair<int, std::string> rsp = api.getGames(req);
    return crow::response(rsp.first, rsp.second);
       return crow::response(400, "Emily needs to use crow jsons.");
  });

  CROW_ROUTE(app, "/games").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req){
    CREATE_API_ENDPOINTS_OBJECT;
    std::pair<int, std::string> rsp = api.postGames(req);
    return crow::response(rsp.first, rsp.second);
  });

  // Get all players in a certain game
  CROW_ROUTE(app, "/game/<int>/players").methods(crow::HTTPMethod::GET)
  ([](const crow::request& req, int game_id){
    CREATE_API_ENDPOINTS_OBJECT;
    return api.getGamePlayers(req, game_id);
  });

// return everything about the game details to the user except game_id and developer_email
  CROW_ROUTE(app, "/game/<int>").methods(crow::HTTPMethod::GET)
  ([](const crow::request& req, int game_id){
    CREATE_API_ENDPOINTS_OBJECT;
    return api.getGame(req, game_id);
  });

  // Add given player stats for a certain game
  CROW_ROUTE(app, "/game/<int>/players").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req, int game_id){
    CREATE_API_ENDPOINTS_OBJECT;
    return api.postGamePlayers(req, game_id);
  });

  // create set for PUT request to not reset all elements and to catch errors
  CROW_ROUTE(app, "/game/<int>").methods(crow::HTTPMethod::PUT)
  ([](const crow::request& req, int game_id){
    CREATE_API_ENDPOINTS_OBJECT;
    return api.putGame(req, game_id);
  });

  // Get a specific player's stats for a specific game
  CROW_ROUTE(app, "/game/<int>/players/<string>").methods(crow::HTTPMethod::GET)
  ([](const crow::request& req, int game_id, std::string player_email) {
     CREATE_API_ENDPOINTS_OBJECT;
     return api.getGamePlayer(req, game_id, player_email);
  });

  CROW_ROUTE(app, "/game/<int>").methods(crow::HTTPMethod::DELETE)
  ([](const crow::request& req, int game_id){
    CREATE_API_ENDPOINTS_OBJECT;
    return api.deleteGame(req, game_id);
  });

  // Remove given player stats for a certain game
  CROW_ROUTE(app, "/game/<int>/players").methods(crow::HTTPMethod::DELETE)
  ([](const crow::request& req, int game_id){
    CREATE_API_ENDPOINTS_OBJECT;
    return api.deleteGamePlayer(req, game_id);
  });

  CROW_ROUTE(app, "/matchmake").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req) {
    CREATE_API_ENDPOINTS_OBJECT;
    DBService DB = DBService();
    Matchmaking M = Matchmaking();
    return api.matchmake(req, &DB, &M);
  });

  // set the port, set the app to run on multiple threads, and run the app
  app.port(18080).multithreaded().run();

  return 0;
}
