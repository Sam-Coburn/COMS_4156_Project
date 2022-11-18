// Copyright [2022] RaisingCanesFanClub

#ifndef API_ENDPOINTS_API_MATCHMAKING_LIB_H_
#define API_ENDPOINTS_API MATCHMAKING_LIB_H_

#include <jsoncpp/json/json.h>  // JsonCpp header file
#include <cmath>
#include <cstdlib>              // EXIT_FAILURE
#include <string>
#include <iostream>
#include <sstream>              // std::stringstream
#include <tuple>
#include <vector>
#include <fstream>
#include <map>
#include <utility>              // std::pair, std::make_pair
#include "crow/crow_all.h"
#include "main/db-service.h"

class Matchmaking {
    public:
        std::tuple<
        std::vector<std::vector<std::vector<std::string> > >,
        std::vector<std::string> > matchmakingBackendBasic(int game_id, std::vector<std::string> player_emails, DBService* DB);

        std::tuple<
        std::vector<std::vector<std::vector<std::string> > >,
        std::vector<std::string> > matchmakingBackendAdvanced(int game_id, std::vector<std::string> player_emails, DBService* DB);
};

#endif