// Copyright [2022] <Copyright Owner>"

#ifndef MAIN_DB_SERVICE_H_
#define MAIN_DB_SERVICE_H_

#include <string>
#include <vector>

struct Player {
  std::string player_email;
  bool is_valid;

  std::string toString() {
    return "Player: player_email = " + player_email;
  }
};

struct Developer {
  std::string developer_email;
  std::string developer_password;
  bool is_valid;

  std::string toString() {
    return "Developer: developer_email = " + developer_email +
    ", developer_password = " + developer_password;
  }
};

struct Game_Details {
  int game_id;
  std::string developer_email;
  std::string game_name;

  std::string game_parameter1_name;
  float game_parameter1_weight;

  std::string game_parameter2_name;
  float game_parameter2_weight;

  std::string game_parameter3_name;
  float game_parameter3_weight;

  std::string game_parameter4_name;
  float game_parameter4_weight;

  std::string category;
  int players_per_team;
  int teams_per_match;

  bool is_valid;

  std::string toString() {
    std::string output =
    "GameDetails:\ngame_id = " + std::to_string(game_id) +
    ", developer_email = " + developer_email +
    ", game_name = " + game_name;

    if (!game_parameter1_name.empty())
      output +=
      "\ngame_parameter1_name = " + game_parameter1_name +
      ", game_parameter1_weight = " + std::to_string(game_parameter1_weight);

    if (!game_parameter2_name.empty())
      output +=
      "\ngame_parameter2_name = " + game_parameter2_name +
      ", game_parameter2_weight = " + std::to_string(game_parameter2_weight);

    if (!game_parameter3_name.empty())
      output +=
      "\ngame_parameter3_name = " + game_parameter3_name +
      ", game_parameter3_weight = " + std::to_string(game_parameter3_weight);

    if (!game_parameter4_name.empty())
      output +=
      "\ngame_parameter4_name = " + game_parameter4_name +
      ", game_parameter4_weight = " + std::to_string(game_parameter4_weight);

    output += "\ncategory = " + category +
    ", players_per_team = " + std::to_string(players_per_team) +
    ", teams_per_match = " + std::to_string(teams_per_match);

    return output;
  }
};

struct Player_Game_Ratings {
  std::string player_email;
  int game_id;

  int game_parameter1_value;
  int game_parameter2_value;
  int game_parameter3_value;
  int game_parameter4_value;

  bool is_valid;

  std::string toString() {
    std::string output =
    "Player Game Ratings:\nplayer_email = " + player_email +
    "game_id = " +  std::to_string(game_id) +
    "\ngame_parameter1_value = " + std::to_string(game_parameter1_value) +
    "\ngame_parameter2_value = " + std::to_string(game_parameter2_value) +
    "\ngame_parameter3_value = " + std::to_string(game_parameter3_value) +
    "\ngame_parameter4_value = " + std::to_string(game_parameter4_value);

    return output;
  }
};

// This struct represents the join between
// Player, Game Details, and Player_Game_Ratings
struct Joined_Player_Game_Ratings {
  std::string player_email;
  int game_id;
  std::string developer_email;
  std::string game_name;

  std::string game_parameter1_name;
  float game_parameter1_weight;
  int game_parameter1_value;

  std::string game_parameter2_name;
  float game_parameter2_weight;
  int game_parameter2_value;

  std::string game_parameter3_name;
  float game_parameter3_weight;
  int game_parameter3_value;

  std::string game_parameter4_name;
  float game_parameter4_weight;
  int game_parameter4_value;

  std::string category;
  int players_per_team;
  int teams_per_match;

  bool is_valid;

  std::string toString() {
    std::string output =
    "Joined Player Game Ratings:\ngame_id = " + std::to_string(game_id) +
    ", developer_email = " + developer_email + ", game_name = " + game_name +
    "\nplayer_email = " + player_email;

    if (!game_parameter1_name.empty())
      output +=
      "\ngame_parameter1_name = " + game_parameter1_name +
      ", game_parameter1_weight = " + std::to_string(game_parameter1_weight) +
      ", game_parameter1_value = " + std::to_string(game_parameter1_value);

    if (!game_parameter2_name.empty())
      output +=
      "\ngame_parameter2_name = " + game_parameter2_name +
      ", game_parameter2_weight = " + std::to_string(game_parameter2_weight) +
      ", game_parameter2_value = " + std::to_string(game_parameter2_value);

    if (!game_parameter3_name.empty())
      output +=
      "\ngame_parameter3_name = " + game_parameter3_name +
      ", game_parameter3_weight = " + std::to_string(game_parameter3_weight) +
      ", game_parameter3_value = " + std::to_string(game_parameter3_value);

    if (!game_parameter4_name.empty())
      output +=
      "\ngame_parameter4_name = " + game_parameter4_name +
      ", game_parameter4_weight = " + std::to_string(game_parameter4_weight) +
      ", game_parameter4_value = " + std::to_string(game_parameter4_value);

    output +=
    "\ncategory = " + category +
    ", players_per_team = " + std::to_string(players_per_team) +
    ", teams_per_match = " + std::to_string(teams_per_match);

    return output;
  }
};

Player get_player(std::string player_email);
Developer get_developer(std::string developer_email);
Game_Details get_game_details(int game_id);

Player_Game_Ratings
get_player_game_rating(std::string player_email, int game_id);

// Gets rating and game details for a specific player
// for a specific game
Joined_Player_Game_Ratings
get_joined_player_game_rating(std::string player_email, int game_id);

std::vector<Player> get_all_players();
std::vector<Developer> get_all_developers();
std::vector<Game_Details> get_all_games();
std::vector<Joined_Player_Game_Ratings>
get_all_player_game_ratings_for_game(int game_id);

Player add_player(Player P);
Developer add_developer(Developer D);
Game_Details add_game_details(Game_Details GD);
Player_Game_Ratings add_player_rating(Player_Game_Ratings PGR);

bool update_player(Player P);
bool update_developer(Developer D);
bool update_game_details(Game_Details GD);
bool update_player_rating(Player_Game_Ratings PGR);

Player remove_player(std::string player_email);
Developer remove_developer(std::string developer_email);
Game_Details remove_game_details(int game_id);
Player_Game_Ratings remove_player_rating(std::string player_email, int game_id);

std::vector<Game_Details>
get_all_games_for_developer(std::string developer_email);

#endif  // MAIN_DB_SERVICE_H_
