// Copyright [2022] RaisingCanesFanClub

#ifndef API_ENDPOINTS_API_ENDPOINTS_LIB_H_
#define API_ENDPOINTS_API_ENDPOINTS_LIB_H_

#include <glog/logging.h>
#include <gtest/gtest_prod.h>   // For FRIEND_TEST
#include <jsoncpp/json/json.h>  // JsonCpp header file
#include <cmath>
#include <cstdlib>              // EXIT_FAILURE
#include <string>
#include <iostream>
#include <sstream>              // std::stringstream
#include <tuple>
#include <vector>
#include <fstream>
#include <utility>              // std::pair, std::make_pair
#include "crow/crow_all.h"
#include "main/db-service.h"
#include "authentication/auth-service.h"
#include "api-endpoints/api-matchmaking-lib.h"

// cpplint --linelength=100 ./main/* ./testing/*

class APIEndPoints {
 protected:
DBService* DB;  // a DB service to use for api calls
AuthService* auth;  // an auth service to use for api calls
bool onHeap;    // whether the DB service object is allocated on heap
// Checks whether supplied username and password are valid
// helper for all API calls that require authentication before proceeding
std::pair<int, std::string> authenticateBadly(const crow::request& req);

// Authenticates JWT token in header of request. The token must be passed in
// the Authorization header and must be of the form: "Bearer <token>" where
// <token> is the JWT token. The return value is a pair where the first
// element is a boolean indicating if the authentication was successful.
// If the first element of the pair is true then the authentication was
// successful and the second element of the pair will be the email of
// the developer just authenticated. If the first element of the pair
// is false the authentication failed, and the second element of the
// pair will be an error message.
[[deprecated("Replaced by AuthenticateTokenGetErrorCode, which returns helpful error codes on failure")]]
std::pair<bool, std::string> authenticateToken(const crow::request& req);
FRIEND_TEST(AuthRouteTest, Authenticate_Token_Test);
FRIEND_TEST(AuthRouteTestFixture, Authenticate_Token_Test);

// works exactly the same as authenticateToken except
// - on success 200 is returned as first elt of tuple
// - failure, specific failure error code is returned as first elt of tuple
std::pair<int, std::string> authenticateTokenGetErrorCode(const crow::request& req);

 public:
APIEndPoints() : DB(new DBService()), auth(new AuthService()), onHeap(true) { }  // default constructor
APIEndPoints(DBService* db, AuthService *auth, bool dbOnHeap = false) : DB(db), auth(auth), onHeap(dbOnHeap) {}
~APIEndPoints() {
    if (onHeap) { delete DB;
                  delete auth;}
}

// All API Endpoints
std::pair <int, std::string> getGames(const crow::request& req);  // Gets all client's games
std::pair <int, std::string> postGame(const crow::request& req);  // Adds a list of games to client's account
crow::response postSignUp(const crow::request& req);
crow::response postLogin(const crow::request& req);
crow::response deleteLogin(const crow::request& req);
virtual crow::response matchmake(const crow::request& req, DBService *DB, Matchmaking *M);
// Kaylee and Maryam, go ahead and change these method names/signatures
//  if they don't match what you want for your endpoint methods
virtual crow::response getGamePlayers(const crow::request& req, int game_id);
virtual crow::response getGame(const crow::request& req, int game_id);
virtual crow::response postGamePlayers(const crow::request& req, int game_id);
virtual crow::response putGame(const crow::request& req, int game_id);
virtual crow::response getGamePlayer(const crow::request& req, int game_id, std::string player_email);
virtual crow::response deleteGame(const crow::request& req, int game_id);
virtual crow::response deleteGamePlayer(const crow::request& req, int game_id);
};
#endif  // API_ENDPOINTS_API_ENDPOINTS_LIB_H_
