// Copyright [2022] RaisingCanesFanClub

#ifndef API_ENDPOINTS_API_ENDPOINTS_BANDAID_LIB_H_
#define API_ENDPOINTS_API_ENDPOINTS_BANDAID_LIB_H_
#include "api-endpoints/api-endpoints-lib.h"
#include <string>

class APIEndPointsBandaid : public APIEndPoints {
 public:
    APIEndPointsBandaid() {}
    ~APIEndPointsBandaid() {}
    bool valid_user(std::string email, std::string password);
    bool valid_user_gameid(std::string email, int game_id);

    crow::response matchmake(const crow::request& req, Matchmaking *M) override;
    crow::response getGamePlayers(const crow::request& req, int game_id) override;
    crow::response getGame(const crow::request& req, int game_id) override;
    crow::response postGamePlayers(const crow::request& req, int game_id) override;
    crow::response putGame(const crow::request& req, int game_id) override;
    crow::response getGamePlayer(const crow::request& req, int game_id, std::string player_email) override;
    crow::response deleteGame(const crow::request& req, int game_id) override;
    crow::response deleteGamePlayer(const crow::request& req, int game_id) override;
};

#endif  // API_ENDPOINTS_API_ENDPOINTS_BANDAID_LIB_H_
