#ifndef LIB_API_ENDPOINTS_H_
#define LIB_API_ENDPOINTS_H_

#endif

#include <jsoncpp/json/json.h>
#include <cstdlib>      // EXIT_FAILURE
#include <string>
#include <iostream>
#include <fstream>      // JsonCpp header file
#include <utility>      // std::pair, std::make_pair
#include "crow/crow_all.h"
#include "main/db-service.h"

std::pair<bool, std::string> authenticateToken(const crow::request& req);

// Description: get all of my games
// takes in a crow request from route GET /games
// On success, returns a pair of (200, reponseBody),
// On failure, returns a pair of (400 or 401, emptyResponseBody)
//    where responseBody = string in json format
//          emptyResponseBody = empty string ""
// returns success/failure status and response body(in json format) as a pair
std::pair <int, std::string> getGames(const crow::request& req);

// Description: add a new game
// takes in a crow request from route POST /games/
// On success, returns a pair of (200, emptyResponseBody),
// On failure, returns a pair of (400 or 401, emptyResponseBody)
//    where emptyResponseBody = empty string ""
std::pair <int, std::string> postGames(const crow::request& req);


/*
GET /games
Description: get all of my games
Response:
200: The request was successful. The response will contain a JSON body.
400: The request was not successful. The response will be empty.
401: You did not supply a valid authorization header. The response will be empty.
Response Body:
Games [Array<Game>]
Where a Game object looks as such
game.id [String]
game.name [String]
game.category [String]
game.parameters [Array<String>]
game.weights [Array<Float>]
game.teamsPerMatch [Integer]
game.minPlayersPerTeam [Integer]
game.maxPlayersPerTeam [Integer]
std::string myauth = req.get_header_value("Authorization");
*/




/*
POST /games/

POST -H “Content-Type: application/json” /games -d “{ ‘name’: ‘value’,  ‘parameters’: ‘value2’, ‘parameter-weights: ‘value3’, /‘teams-per-match’: ‘value4’, {min-players-per-team: ‘value5’, ‘max-players-per-team’: ‘value6’, ‘category’: value7’}”

Description: add a new game
Request Parameters:
Name [String] REQUIRED
Parameters [Array<String>] REQUIRED
Parameter-weights [Array<Integer>] REQUIRED
Min-players-per-team [Integer] REQUIRED
Max-players-per-team [Integer] REQUIRED
Category [String] OPTIONAL
Response:
200: The request was successful. The response will contain a JSON body.
400: The request was not successful. The response will be empty.
401: You did not supply a valid authorization header. The response will be empty.
Response Body:
Game-id [String]
*/