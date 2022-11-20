// Copyright [2022] <Copyright Owner>"

#ifndef MAIN_DB_SERVICE_H_
#define MAIN_DB_SERVICE_H_

#include <glog/logging.h>
#include <string>
#include <vector>

struct Player {
  std::string player_email = "";
  bool is_valid = false;

  std::string toString() {
    return "Player: player_email = " + player_email;
  }
};

struct Developer {
  std::string developer_email = "";
  std::string developer_password = "";
  bool is_valid = false;

  std::string toString() {
    return "Developer: developer_email = " + developer_email +
    ", developer_password = " + developer_password;
  }
};

struct Game_Details {
  int game_id = -1;
  std::string developer_email = "";
  std::string game_name = "";

  std::string game_parameter1_name = "";
  float game_parameter1_weight = 0;

  std::string game_parameter2_name = "";
  float game_parameter2_weight = 0;

  std::string game_parameter3_name = "";
  float game_parameter3_weight = 0;

  std::string game_parameter4_name = "";
  float game_parameter4_weight = 0;

  std::string category = "";
  int players_per_team = 0;
  int teams_per_match = 0;

  bool is_valid = false;

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
  std::string player_email = "";
  int game_id = -1;

  int game_parameter1_value = 0;
  int game_parameter2_value = 0;
  int game_parameter3_value = 0;
  int game_parameter4_value = 0;

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
  std::string player_email = "";
  int game_id = -1;
  std::string developer_email = "";
  std::string game_name = "";

  std::string game_parameter1_name = "";
  float game_parameter1_weight = 0;
  int game_parameter1_value = 0;

  std::string game_parameter2_name = "";
  float game_parameter2_weight = 0;
  int game_parameter2_value = 0;

  std::string game_parameter3_name = "";
  float game_parameter3_weight = 0;
  int game_parameter3_value = 0;

  std::string game_parameter4_name = "";
  float game_parameter4_weight = 0;
  int game_parameter4_value = 0;

  std::string category = "";
  int players_per_team = 0;
  int teams_per_match = 0;

  bool is_valid = false;

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

class DBService {
  std::string hostname;
  std::string username;
  std::string password;
  std::string database;

 public:
    // Constructor that defines connection parameters,
    // should be instantiated without any arguments
    // unless explicity such as for testing.
    DBService(std::string host = "tcp://127.0.0.1:3306",
    std::string user = "dbuser",
    std::string pass = "123",
    std::string db = "matchmaking_api_db");

    // Destructor marked virtual
    virtual ~DBService() { }


    // These functions are mainly for testing
    bool setUpDatabase();     // Setup database if not setup
    bool tearDownDatabase();  // Drop all tables in database

    // All the get functions take in the primary key(s) for the relevant
    // object and returns the relevant instance of said object.
    // The get can only be considered succesful if the
    // is_valid flag of the returned object is true.
    virtual Player get_player(std::string player_email);
    virtual Developer get_developer(std::string developer_email);
    virtual Game_Details get_game_details(int game_id);
    virtual Player_Game_Ratings get_player_game_rating(std::string player_email, int game_id);
    virtual Joined_Player_Game_Ratings get_joined_player_game_rating(std::string player_email, int game_id);


    // The get_all functions return a vector of all instances of the
    // relevant object in the database
    virtual std::vector<Player> get_all_players();
    virtual std::vector<Developer> get_all_developers();
    virtual std::vector<Game_Details> get_all_games();
    virtual std::vector<Joined_Player_Game_Ratings> get_all_player_game_ratings_for_game(int game_id);
    virtual std::vector<Game_Details> get_all_games_for_developer(std::string developer_email);

    // The add functions take an instance of the object to
    // add to the database and return an instance of said
    // object. The creation of the object in the database
    // can only be considered successful if the is_valid
    // flag of the returned object is true.
    virtual Player add_player(Player P);
    virtual Developer add_developer(Developer D);
    virtual Game_Details add_game_details(Game_Details GD);
    virtual Player_Game_Ratings add_player_rating(Player_Game_Ratings PGR);

    // The remove functions take the primary key(s) of the
    // relevant object to be removed and returns an instance
    // of the object that was just removed. The deletion of
    // the object in the database can only be considered
    // successful if the is_valid flag of the returned object
    // is true.
    virtual Player remove_player(std::string player_email);
    virtual Developer remove_developer(std::string developer_email);
    virtual Game_Details remove_game_details(int game_id);
    virtual Player_Game_Ratings remove_player_rating(std::string player_email, int game_id);


    // The delete functions take the primary key(s) of the
    // relevant object to be updated and returns an instance
    // of the updated object i.e after being updated. The update of
    // the object in the database can only be considered
    // successful if the is_valid flag of the returned object
    // is true.
    virtual Player update_player(std::string player_email, Player P);
    virtual Developer update_developer(std::string developer_email, Developer D);
    virtual Game_Details update_game_details(int game_id, Game_Details GD);
    virtual Player_Game_Ratings update_player_rating(std::string player_email, int game_id, Player_Game_Ratings PGR);
};

#endif  // MAIN_DB_SERVICE_H_
