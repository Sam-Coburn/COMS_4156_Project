#ifndef LIB_EMILY_H_
#define LIB_EMILY_H_

#endif

#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h> // including JsonCpp header file

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