// Copyright [2022] RaisingCanesFanClub

#ifndef API_ENDPOINTS_API_ENDPOINTS_LIB_H_
#define API_ENDPOINTS_API_ENDPOINTS_LIB_H_

#include <jsoncpp/json/json.h>  // JsonCpp header file
#include <cstdlib>              // EXIT_FAILURE
#include <string>
#include <iostream>
#include <sstream>              // std::stringstream
#include <fstream>
#include <utility>              // std::pair, std::make_pair
#include "crow/crow_all.h"
#include "main/db-service.h"

// cpplint --linelength=100 ./main/* ./testing/*

class APIEndPoints {
 private:
    DBService DB;  // a DB service to use for api calls

    // Checks whether supplied username and password are valid
    // helper for all API calls that require authentication before proceeding
    std::pair<int, std::string> authenticateBadly(const crow::request& req);

    // Checks whether supplied token is valid for client
    // helper for all API calls (other than login)
    // that require authentication of a token before proceeding
    std::pair<int, std::string> authenticateToken(const crow::request& req);

    /*
    Validate user using email and password
    ** Only for first iteration **
    */
    bool valid_user(std::string email, std::string password);

    /*
    Validate user is authorized to access requested game
    */
    bool valid_user_gameid(std::string email, std::string password, int game_id);

 public:
    APIEndPoints() : DB(DBService()) {};  // default constructor

    /*
        --------------------------------------------------------------------------------
        API Endpoint Functions
        --------------------------------------------------------------------------------
    */

    // All API Endpoints
    // Take in a crow request and return a pair of (int, string)
    // Crow routes initialized in our main function wrap calls to these methods
    // Input: takes in a crow request
    // On success, returns a pair of (200, reponseBody),
    //    where responseBody = JSON string
    // On failure, returns a pair of (400 or 401, failResponseBody)
    //    where failResponseBody = empty string "" or error message

    // Gets all client's games
    // Response Body Details:
    // Games [Array<Game>]
    // Where a Game object looks as such
    // game.id [String]
    // game.name [String]
    // game.category [String]
    // game.parameters [Array<String>]
    // game.weights [Array<Float>]
    // game.teamsPerMatch [Integer]
    // game.minPlayersPerTeam [Integer]
    // game.maxPlayersPerTeam [Integer]
    std::pair <int, std::string> getGames(const crow::request& req);

    // Adds a list of games to client's account
    // Request Parameters:
    // Games [Array<Game>]
    // Where a Game object looks as such
    // Name [String] REQUIRED
    // Parameters [Array<String>] REQUIRED
    // Parameter-weights [Array<Integer>] REQUIRED
    // Min-players-per-team [Integer] REQUIRED
    // Max-players-per-team [Integer] REQUIRED
    // Category [String] OPTIONAL
    std::pair <int, std::string> postGame(const crow::request& req);

    /*
        Get all players for a requested game
        Request Parameters:
            game_id [Integer] REQUIRED
        Request Body:
            developer_email [String] REQUIRED
            developer_password [String] REQUIRED
        Return Body:
            players [JSON with PlayerGameRating Objects]
            Where a PlayerGameRating Object in the JSON looks as such
                player_email [String] (Key)
                    game_id [Integer]
                    game_parameter1_name [String]
                        game_parameter1_value [Float]
                    game_parameter2_name [String]
                        game_parameter2_value [Float]
                    game_parameter3_name [String]
                        game_parameter3_value [Float]
                    game_parameter4_name [String]
                        game_parameter4_value [Float]
    */
    crow::response getGamePlayers(const crow::request& req, int game_id);

    /*
        Add player stats for a game
        Request Parameters:
            game_id [Integer] REQUIRED
        Request Body:
            developer_email [String] REQUIRED
            developer_password [String] REQUIRED
            player_game_ratings [Json with PlayerGameRatings Objects] REQUIRED
            Where a PlayerGameRatings Object in the JSON looks as such
                player_email [String] (Key)
                    game_id [Integer]
                    game_parameter1_value [Float]
                    game_parameter2_value [Float]
                    game_parameter3_value [Float]
                    game_parameter4_value [Float]
        Return Body:
            return_message [String]    
    */
    crow::response addPlayersStats(const crow::request& req, int game_id);

    /*
        Get specific players' stats for a game
        Request Parameters:
            game_id [Integer] REQUIRED
        Request Body:
            developer_email [String] REQUIRED
            developer_password [String] REQUIRED
            player_emails [List[String]] REQUIRED
        Return Body:
            players [JSON with PlayerGameRating Objects]
            Where a PlayerGameRating Object in the JSON looks as such
                player_email [String] (Key)
                    game_id [Integer]
                    game_parameter1_name [String]
                        game_parameter1_value [Float]
                    game_parameter2_name [String]
                        game_parameter2_value [Float]
                    game_parameter3_name [String]
                        game_parameter3_value [Float]
                    game_parameter4_name [String]
                        game_parameter4_value [Float]
    */
    crow::response getPlayersStats(const crow::request& req, int game_id);

    /*
        Delete requested players' stats for a given game
        Request Parameters:
            game_id [Integer] REQUIRED
        Request Body:
            developer_email [String] REQUIRED
            developer_password [String] REQUIRED
            player_emails [List[String]] REQUIRED
        Return Body:
            return_message [String]
    */
    crow::response deletePlayersStats(const crow::request& req, int game_id);

    /*
        Update requested players' stats for a given game
        Request Parameters:
            game_id [Integer] REQUIRED
        Request Body:
            developer_email [String] REQUIRED
            developer_password [String] REQUIRED
            players [Json with PlayerGameRating Objects] REQUIRED (Key: player_email)
            Where a PlayerGameRatings Object in the JSON looks as such
                player_email [String] (Key)
                    game_id [Integer]
                    game_parameter1_value [Float]
                    game_parameter2_value [Float]
                    game_parameter3_value [Float]
                    game_parameter4_value [Float]
        Return Body:
            return_message [String]
    */
    crow::response updatePlayersStats(const crow::request& req, int game_id);
};

#endif  // API_ENDPOINTS_API_ENDPOINTS_LIB_H_
