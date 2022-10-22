#include "api-endpoints/api-endpoints-lib.h"

std::pair<bool, std::string> authenticateToken(const crow::request& req) {
    std::string myauth = req.get_header_value("Authorization");
    // in future, should find user email from their stored token
    if (myauth.compare("test-token") == 0) {
        // for now, just return one valid user email
        return std::make_pair(true, std::string("apex_player@gmail.com"));
    } else {
        // return failure; token not found, or maybe expired!
        return std::make_pair(false, std::string(""));
    }
}

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
std::pair <int, std::string> getGames(const crow::request& req) {
    std::vector<Game_Details> games;
    std::pair<bool, std::string> tokenCheck = authenticateToken(req);
    if (!tokenCheck.first) {
        return std::make_pair(401, std::string("Bad token"));
    }
    //games = get_all_games_for_developer(tokenCheck.second);
    std::string forNow = "someday I'll return a jason list of games info.";
    return std::make_pair(200, forNow);
}

// Description: add a new game
// takes in a crow request from route POST /games/
// On success, returns a pair of (200, emptyResponseBody),
// On failure, returns a pair of (400 or 401, emptyResponseBody)
//    where emptyResponseBody = empty string ""
std::pair <int, std::string> postGames(const crow::request& req) {
    std::pair<bool, std::string> tokenCheck = authenticateToken(req);
    if (!tokenCheck.first) {
        return std::make_pair(401, std::string("Bad token"));
    }
    std::string forNow = "someday I'll write game info to the DB";
    return std::make_pair(200, forNow);
}
