// Copyright [2022] RaisingCanesFanClub

#ifndef TESTING_MOCKDBSERVICE_H_
#define TESTING_MOCKDBSERVICE_H_

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <string>
#include "main/db-service.h"

class MockAuthService : public AuthService {
 public:
  MOCK_METHOD(std::string, encryptPassword, (std::string password), (override));
  MOCK_METHOD(bool, validatePassword, (std::string password, std::string hash), (override));
  MOCK_METHOD(std::string, createJWT, (std::string password, int seconds), (override));
  MOCK_METHOD((std::pair<bool, std::string>), decodeAndVerifyJWT, (std::string token), (override));
};

class MockDBService : public DBService {
 public:
  MOCK_METHOD(Player, get_player, (std::string player_email), (override));
  MOCK_METHOD(Developer, get_developer, (std::string developer_email), (override));
  MOCK_METHOD(Game_Details, get_game_details, (int game_id), (override));
  MOCK_METHOD(Player_Game_Ratings, get_player_game_rating, (std::string player_email, int game_id), (override));
  MOCK_METHOD(Joined_Player_Game_Ratings, get_joined_player_game_rating,
  (std::string player_email, int game_id), (override));

  MOCK_METHOD(std::vector<Player>, get_all_players, (), (override));
  MOCK_METHOD(std::vector<Developer>, get_all_developers, (), (override));
  MOCK_METHOD(std::vector<Game_Details>, get_all_games, (), (override));
  MOCK_METHOD(std::vector<Joined_Player_Game_Ratings>, get_all_player_game_ratings_for_game, (int game_id), (override));
  MOCK_METHOD(std::vector<Game_Details>, get_all_games_for_developer, (std::string developer_email), (override));

  MOCK_METHOD(Player, add_player, (Player P), (override));
  MOCK_METHOD(Developer, add_developer, (Developer D), (override));
  MOCK_METHOD(Game_Details, add_game_details, (Game_Details GD), (override));
  MOCK_METHOD(Player_Game_Ratings, add_player_rating, (Player_Game_Ratings PGR), (override));

  MOCK_METHOD(Player, remove_player, (std::string player_email), (override));
  MOCK_METHOD(Developer, remove_developer, (std::string developer_email), (override));
  MOCK_METHOD(Game_Details, remove_game_details, (int game_id), (override));
  MOCK_METHOD(Player_Game_Ratings, remove_player_rating, (std::string player_email, int game_id), (override));

  MOCK_METHOD(Player, update_player, (std::string player_email, Player P), (override));
  MOCK_METHOD(Developer, update_developer, (std::string developer_email, Developer D), (override));
  MOCK_METHOD(Game_Details, update_game_details,
  (int game_id, Game_Details GD), (override));
  MOCK_METHOD(Player_Game_Ratings, update_player_rating,
  (std::string player_email, int game_id, Player_Game_Ratings PGR), (override));
};

#endif  // TESTING_MOCKDBSERVICE_H_
