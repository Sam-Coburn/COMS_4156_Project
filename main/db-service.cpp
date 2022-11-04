/*Copyright [year] <Copyright Owner>*/

#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <stdlib.h>
#include <iostream>

#include "main/db-service.h"

DBService::DBService
(std::string host, std::string user, std::string pass, std::string db) {
  hostname = host;
  username = user;
  password = pass;
  database = db;
}

Player DBService::get_player(std::string player_email) {
  std::cout << "Connecting to MYSQL to get player with email: "
  + player_email << std::endl;
  Player P;
  P.is_valid = false;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement  *prep_stmt;
    sql::ResultSet *res;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Create statement and fill in relevant variables
    prep_stmt = con->prepareStatement("SELECT * FROM Players P "
    "WHERE P.player_email = ?;");
    prep_stmt->setString(1, player_email);

    // Execute query and get results
    res = prep_stmt->executeQuery();
    while (res->next()) {
      P.player_email = res->getString("player_email");
      P.is_valid = true;
      std::cout << P.toString() << std::endl;
    }

    delete prep_stmt;
    delete res;
    delete con;

    std::cout << "Successfully retrieved player with email: " +
    player_email << std::endl;
  } catch (sql::SQLException &e) {
    std::cout <<
    "Error getting player with email: " + player_email << std::endl;
    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }
  return P;
}

Developer DBService::get_developer(std::string developer_email) {
  std::cout << "Connecting to MYSQL to get developer with email: " +
  developer_email << std::endl;

  Developer D;
  D.is_valid = false;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement  *prep_stmt;
    sql::ResultSet *res;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Create statement and fill in relevant variables
    prep_stmt = con->prepareStatement("SELECT * FROM Developers D "
    "WHERE D.developer_email = ?;");
    prep_stmt->setString(1, developer_email);

    // Execute query and get results
    res = prep_stmt->executeQuery();
    while (res->next()) {
        D.developer_email = res->getString("developer_email");
        D.developer_password = res->getString("developer_password");
        D.is_valid = true;
        std::cout << D.toString() << std::endl;
    }

    delete prep_stmt;
    delete res;
    delete con;

    std::cout << "Successfully retrieved developer with email: " +
    developer_email << std::endl;
  } catch (sql::SQLException &e) {
    std::cout <<
    "Error getting developer with email: " + developer_email << std::endl;
    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }
  return D;
}

Game_Details DBService::get_game_details(int game_id) {
  std::cout << "Connecting to MYSQL to get game with game_id: " +
  std::to_string(game_id) << std::endl;

  Game_Details GD;
  GD.is_valid = false;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement  *prep_stmt;
    sql::ResultSet *res;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Create query and fill in relevant variables
    prep_stmt = con->prepareStatement("SELECT * FROM Game_Details GD "
    "WHERE GD.game_id = ?;");
    prep_stmt->setInt(1, game_id);

    // Execute query and get results
    res = prep_stmt->executeQuery();
    while (res->next()) {
      GD.game_id = std::stoi(res->getString("game_id"));
      GD.developer_email = res->getString("developer_email");
      GD.game_name = res->getString("game_name");
      GD.game_parameter1_name = res->getString("game_parameter1_name");
      GD.game_parameter1_weight =
      std::stof(res->getString("game_parameter1_weight"));

      GD.game_parameter2_name = res->getString("game_parameter2_name");
      GD.game_parameter2_weight =
      std::stof(res->getString("game_parameter2_weight"));

      GD.game_parameter3_name = res->getString("game_parameter3_name");
      GD.game_parameter3_weight =
      std::stof(res->getString("game_parameter3_weight"));

      GD.game_parameter4_name = res->getString("game_parameter4_name");
      GD.game_parameter4_weight =
      std::stof(res->getString("game_parameter4_weight"));

      GD.category = res->getString("category");
      GD.players_per_team = std::stoi(res->getString("players_per_team"));
      GD.teams_per_match = std::stoi(res->getString("teams_per_match"));
      GD.is_valid = true;
      std::cout << GD.toString() << std::endl;
    }

    delete prep_stmt;
    delete res;
    delete con;

    std::cout << "Successfully retrieved game with game_id: " +
    std::to_string(game_id) << std::endl;
  } catch (sql::SQLException &e) {
    std::cout <<
    "Error getting game with game_id: " + std::to_string(game_id) << std::endl;
    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }

  return GD;
}

Player_Game_Ratings
DBService::get_player_game_rating(std::string player_email, int game_id) {
  std::cout << "Connecting to MYSQL to get rating in game with game_id: " +
  std::to_string(game_id) + " for player with email: " +
  player_email<< std::endl;

  Player_Game_Ratings PGR;
  PGR.is_valid = false;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement  *prep_stmt;
    sql::ResultSet *res;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Create query and fill in relevant variables
    prep_stmt = con->prepareStatement("SELECT * FROM Player_Game_Ratings PGR "
    "WHERE PGR.player_email = ? AND PGR.game_id = ?;");
    prep_stmt->setString(1, player_email);
    prep_stmt->setInt(2, game_id);

    // Execute query and get results
    res = prep_stmt->executeQuery();
    while (res->next()) {
      PGR.player_email = res->getString("player_email");
      PGR.game_id = std::stoi(res->getString("game_id"));
      PGR.game_parameter1_value =
      std::stoi(res->getString("game_parameter1_value"));
      PGR.game_parameter2_value =
      std::stoi(res->getString("game_parameter2_value"));
      PGR.game_parameter3_value =
      std::stoi(res->getString("game_parameter3_value"));
      PGR.game_parameter4_value =
      std::stoi(res->getString("game_parameter4_value"));
      PGR.is_valid = true;
      std::cout << PGR.toString() << std::endl;
    }

    delete prep_stmt;
    delete res;
    delete con;

    std::cout << "Successfully retrieved rating in game with game_id: " +
    std::to_string(game_id) + " for player with email: " +
    player_email << std::endl;
  } catch (sql::SQLException &e) {
    std::cout << "Error getting rating in game with game_id: " +
    std::to_string(game_id) + " for player with email: " +
    player_email<< std::endl;

    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }

  return PGR;
}

Joined_Player_Game_Ratings
DBService::
get_joined_player_game_rating(std::string player_email, int game_id) {
  std::cout << "Connecting to MYSQL to get rating in game with game_id: " +
  std::to_string(game_id) + " for player with email: " + player_email
  << std::endl;

  Joined_Player_Game_Ratings JPGR;
  JPGR.is_valid = false;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement  *prep_stmt;
    sql::ResultSet *res;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Create query and fill in relevant variables
    prep_stmt = con->prepareStatement("SELECT PGR.*, GD.* FROM Players P "
    "JOIN Player_Game_Ratings PGR ON P.player_email = PGR.player_email "
    "JOIN Game_Details GD ON PGR.game_id = GD.game_id "
    "WHERE P.player_email = ? AND GD.game_id = ?;");
    prep_stmt->setString(1, player_email);
    prep_stmt->setInt(2, game_id);

    // Execute query and retrieve results
    res = prep_stmt->executeQuery();
    while (res->next()) {
      JPGR.player_email = res->getString("player_email");
      JPGR.game_id = std::stoi(res->getString("game_id"));
      JPGR.developer_email = res->getString("developer_email");
      JPGR.game_name = res->getString("game_name");
      JPGR.game_parameter1_name = res->getString("game_parameter1_name");
      JPGR.game_parameter1_weight =
      std::stof(res->getString("game_parameter1_weight"));
      JPGR.game_parameter1_value =
      std::stoi(res->getString("game_parameter1_value"));

      JPGR.game_parameter2_name = res->getString("game_parameter2_name");
      JPGR.game_parameter2_weight =
      std::stof(res->getString("game_parameter2_weight"));
      JPGR.game_parameter2_value =
      std::stoi(res->getString("game_parameter2_value"));

      JPGR.game_parameter3_name = res->getString("game_parameter3_name");
      JPGR.game_parameter3_weight =
      std::stof(res->getString("game_parameter3_weight"));
      JPGR.game_parameter3_value =
      std::stoi(res->getString("game_parameter3_value"));

      JPGR.game_parameter4_name = res->getString("game_parameter4_name");
      JPGR.game_parameter4_weight =
      std::stof(res->getString("game_parameter4_weight"));
      JPGR.game_parameter4_value =
      std::stoi(res->getString("game_parameter4_value"));

      JPGR.category = res->getString("category");
      JPGR.players_per_team = std::stoi(res->getString("players_per_team"));
      JPGR.teams_per_match = std::stoi(res->getString("teams_per_match"));
      JPGR.is_valid = true;
      std::cout << JPGR.toString() << std::endl;
    }

    delete prep_stmt;
    delete res;
    delete con;

    std::cout << "Successfully retrieved rating in game with game_id: " +
    std::to_string(game_id) + " for player with email: " +
    player_email<< std::endl;
  } catch (sql::SQLException &e) {
    std::cout << "Error getting rating in game with game_id: " +
    std::to_string(game_id) + " for player with email: " +
    player_email<< std::endl;

    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }

  return JPGR;
}

std::vector<Player> DBService::get_all_players() {
  std::cout << "Connecting to MYSQL to get all players" << std::endl;
  std::vector<Player> players;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;
    Player p;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Create query, execute it, and get results
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM Players");
    while (res->next()) {
        p.player_email = res->getString("player_email");
        p.is_valid = true;
        std::cout << p.toString() << std::endl;
        players.push_back(p);
    }

    delete stmt;
    delete res;
    delete con;

    std::cout << "Successfully retrieved all players" << std::endl;
  } catch (sql::SQLException &e) {
    std::cout << "Error getting all players" << std::endl;
    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }

  return players;
}

std::vector<Developer> DBService::get_all_developers() {
  std::cout << "Connecting to MYSQL to get all developers" << std::endl;
  std::vector<Developer> developers;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;
    Developer d;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Create query, execute it, and get results
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM Developers");
    while (res->next()) {
        d.developer_email = res->getString("developer_email");
        d.developer_password = res->getString("developer_password");
        d.is_valid = true;
        std::cout << d.toString() << std::endl;
        developers.push_back(d);
    }

    delete stmt;
    delete res;
    delete con;

    std::cout << "Successfully retrieved all developers" << std::endl;
  } catch (sql::SQLException &e) {
    std::cout << "Error getting all developers" << std::endl;
    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }
  return developers;
}

std::vector<Game_Details> DBService::get_all_games() {
  std::cout << "Connecting to MYSQL to get all game details" << std::endl;
  std::vector<Game_Details> game_details;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;
    Game_Details gd;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Create query, execute it and get results
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM Game_Details");
    while (res->next()) {
      gd.game_id = std::stoi(res->getString("game_id"));
      gd.developer_email = res->getString("developer_email");
      gd.game_name = res->getString("game_name");
      gd.game_parameter1_name = res->getString("game_parameter1_name");
      gd.game_parameter1_weight =
      std::stof(res->getString("game_parameter1_weight"));

      gd.game_parameter2_name = res->getString("game_parameter2_name");
      gd.game_parameter2_weight =
      std::stof(res->getString("game_parameter2_weight"));

      gd.game_parameter3_name = res->getString("game_parameter3_name");
      gd.game_parameter3_weight =
      std::stof(res->getString("game_parameter3_weight"));

      gd.game_parameter4_name = res->getString("game_parameter4_name");
      gd.game_parameter4_weight =
      std::stof(res->getString("game_parameter4_weight"));

      gd.category = res->getString("category");
      gd.players_per_team = std::stoi(res->getString("players_per_team"));
      gd.teams_per_match = std::stoi(res->getString("teams_per_match"));
      gd.is_valid = true;
      std::cout << gd.toString() << std::endl;
      game_details.push_back(gd);
    }

    delete stmt;
    delete res;
    delete con;

    std::cout << "Successfully retrieved all game details" << std::endl;
  } catch (sql::SQLException &e) {
    std::cout << "Error getting all game details" << std::endl;
    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }

  return game_details;
}


std::vector<Joined_Player_Game_Ratings>
DBService::get_all_player_game_ratings_for_game(int game_id) {
  std::cout <<
  "Connecting to MYSQL to get all player ratings for a game" << std::endl;
  std::vector<Joined_Player_Game_Ratings> JPGR;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement  *prep_stmt;
    sql::ResultSet *res;
    Joined_Player_Game_Ratings jpgr;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Create query and fill in relevant variables
    prep_stmt = con->prepareStatement("SELECT PGR.*, GD.* FROM Players P "
    "JOIN Player_Game_Ratings PGR ON P.player_email = PGR.player_email "
    "JOIN Game_Details GD ON PGR.game_id = GD.game_id WHERE GD.game_id = ?;");
    prep_stmt->setInt(1, game_id);

    // Execute query and get results
    res = prep_stmt->executeQuery();
    while (res->next()) {
      jpgr.player_email = res->getString("player_email");
      jpgr.game_id = std::stoi(res->getString("game_id"));
      jpgr.developer_email = res->getString("developer_email");
      jpgr.game_name = res->getString("game_name");
      jpgr.game_parameter1_name = res->getString("game_parameter1_name");
      jpgr.game_parameter1_weight =
      std::stof(res->getString("game_parameter1_weight"));
      jpgr.game_parameter1_value =
      std::stoi(res->getString("game_parameter1_value"));

      jpgr.game_parameter2_name = res->getString("game_parameter2_name");
      jpgr.game_parameter2_weight =
      std::stof(res->getString("game_parameter2_weight"));
      jpgr.game_parameter2_value =
      std::stoi(res->getString("game_parameter2_value"));

      jpgr.game_parameter3_name = res->getString("game_parameter3_name");
      jpgr.game_parameter3_weight =
      std::stof(res->getString("game_parameter3_weight"));
      jpgr.game_parameter3_value =
      std::stoi(res->getString("game_parameter3_value"));

      jpgr.game_parameter4_name = res->getString("game_parameter4_name");
      jpgr.game_parameter4_weight =
      std::stof(res->getString("game_parameter4_weight"));
      jpgr.game_parameter4_value =
      std::stoi(res->getString("game_parameter4_value"));

      jpgr.category = res->getString("category");
      jpgr.players_per_team = std::stoi(res->getString("players_per_team"));
      jpgr.teams_per_match = std::stoi(res->getString("teams_per_match"));
      jpgr.is_valid = true;
      std::cout << jpgr.toString() << std::endl;
      JPGR.push_back(jpgr);
    }

    delete prep_stmt;
    delete res;
    delete con;

    std::cout << "Successfully retrieved all player game ratings" << std::endl;
  } catch (sql::SQLException &e) {
    std::cout << "Error getting all player game ratings" << std::endl;
    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }

  return JPGR;
}

std::vector<Game_Details>
DBService::get_all_games_for_developer(std::string developer_email) {
  std::cout << "Connecting to MYSQL to get games for developer with email: " +
  developer_email << std::endl;

  std::vector<Game_Details> GD;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement  *prep_stmt;
    sql::ResultSet *res;
    Game_Details gd;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Create query and fill in relevant variables
    prep_stmt = con->prepareStatement("SELECT * FROM Game_Details GD "
    "WHERE GD.developer_email = ?;");
    prep_stmt->setString(1, developer_email);

    // Execute query and get results
    res = prep_stmt->executeQuery();
    while (res->next()) {
      gd.game_id = std::stoi(res->getString("game_id"));
      gd.developer_email = res->getString("developer_email");
      gd.game_name = res->getString("game_name");
      gd.game_parameter1_name = res->getString("game_parameter1_name");
      gd.game_parameter1_weight =
      std::stof(res->getString("game_parameter1_weight"));

      gd.game_parameter2_name = res->getString("game_parameter2_name");
      gd.game_parameter2_weight =
      std::stof(res->getString("game_parameter2_weight"));

      gd.game_parameter3_name = res->getString("game_parameter3_name");
      gd.game_parameter3_weight =
      std::stof(res->getString("game_parameter3_weight"));

      gd.game_parameter4_name = res->getString("game_parameter4_name");
      gd.game_parameter4_weight =
      std::stof(res->getString("game_parameter4_weight"));

      gd.category = res->getString("category");
      gd.players_per_team = std::stoi(res->getString("players_per_team"));
      gd.teams_per_match = std::stoi(res->getString("teams_per_match"));
      gd.is_valid = true;
      std::cout << gd.toString() << std::endl;
      GD.push_back(gd);
    }

    delete prep_stmt;
    delete res;
    delete con;

    std::cout << "Successfully retrieved games for developer with email: " +
    developer_email << std::endl;
  } catch (sql::SQLException &e) {
    std::cout << "Error getting games for developer with email: " +
    developer_email << std::endl;
    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }

  return GD;
}

Player DBService::add_player(Player P) {
  std::cout << "Connecting to MYSQL to insert player with email: " +
  P.player_email << std::endl;
  P.is_valid = false;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement  *prep_stmt;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Create statement and fill in relevant variables
    prep_stmt = con->prepareStatement("INSERT INTO Players VALUES(?);");
    prep_stmt->setString(1, P.player_email);

    // Execute statament and set flag
    prep_stmt->executeUpdate();
    P.is_valid = true;

    delete prep_stmt;
    delete con;

    std::cout <<
    "Successfully created player with email: " + P.player_email << std::endl;
  } catch (sql::SQLException &e) {
    std::cout <<
    "Error creating player with email: " + P.player_email << std::endl;
    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }

  return P;
}

Developer DBService::add_developer(Developer D) {
  std::cout << "Connecting to MYSQL to insert developer with email: " +
  D.developer_email << std::endl;
  D.is_valid = false;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement  *prep_stmt;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Create statement and fill in relevant variables
    prep_stmt = con->prepareStatement("INSERT INTO Developers VALUES(?,?);");
    prep_stmt->setString(1, D.developer_email);
    prep_stmt->setString(2, D.developer_password);

    // Execute statement and set flag
    prep_stmt->executeUpdate();
    D.is_valid = true;

    delete prep_stmt;
    delete con;

    std::cout << "Successfully created developer with email: " +
    D.developer_email << std::endl;
  } catch (sql::SQLException &e) {
    std::cout <<
    "Error creating developer with email: " + D.developer_email << std::endl;
    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }
  return D;
}

Game_Details DBService::add_game_details(Game_Details GD) {
  std::cout << "Connecting to MYSQL to insert new game" << std::endl;
  GD.is_valid = false;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::PreparedStatement  *prep_stmt;
    sql::ResultSet *res;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Create SQL statement
    prep_stmt = con->prepareStatement("INSERT INTO Game_Details "
    "(game_name, developer_email, "
    "game_parameter1_name, game_parameter1_weight, "
    "game_parameter2_name, game_parameter2_weight, "
    "game_parameter3_name, game_parameter3_weight, "
    "game_parameter4_name, game_parameter4_weight, "
    "category, players_per_team, teams_per_match) "
    "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");

    // Fill in relevant variables in statement and execute
    prep_stmt->setString(1, GD.game_name);
    prep_stmt->setString(2, GD.developer_email);
    prep_stmt->setString(3, GD.game_parameter1_name);
    prep_stmt->setDouble(4, GD.game_parameter1_weight);
    prep_stmt->setString(5, GD.game_parameter2_name);
    prep_stmt->setDouble(6, GD.game_parameter2_weight);
    prep_stmt->setString(7, GD.game_parameter3_name);
    prep_stmt->setDouble(8, GD.game_parameter3_weight);
    prep_stmt->setString(9, GD.game_parameter4_name);
    prep_stmt->setDouble(10, GD.game_parameter4_weight);
    prep_stmt->setString(11, GD.category);
    prep_stmt->setInt(12, GD.players_per_team);
    prep_stmt->setInt(13, GD.teams_per_match);
    prep_stmt->executeUpdate();

    // Create and execute a second statement to get id of object just created
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT LAST_INSERT_ID()");
    while (res->next()) {
      GD.game_id = res->getInt("LAST_INSERT_ID()");
    }

    GD.is_valid = true;

    delete prep_stmt;
    delete stmt;
    delete res;
    delete con;

    std::cout << "Successfully created game with game_id: " +
    std::to_string(GD.game_id) << std::endl;
  } catch (sql::SQLException &e) {
    std::cout << "Error creating game" << std::endl;
    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }

  return GD;
}

Player_Game_Ratings DBService::add_player_rating(Player_Game_Ratings PGR) {
  std::cout << "Connecting to MYSQL to create rating in game with game_id: " +
  std::to_string(PGR.game_id) + " for player with email: " +
  PGR.player_email << std::endl;

  PGR.is_valid = false;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement  *prep_stmt;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Create SQL statement
    prep_stmt = con->prepareStatement("INSERT INTO Player_Game_Ratings "
    "VALUES(?, ?, ?, ?, ?, ?);");

    // Fill in relevant variables in statement and execute
    prep_stmt->setString(1, PGR.player_email);
    prep_stmt->setInt(2, PGR.game_id);
    prep_stmt->setInt(3, PGR.game_parameter1_value);
    prep_stmt->setInt(4, PGR.game_parameter2_value);
    prep_stmt->setInt(5, PGR.game_parameter3_value);
    prep_stmt->setInt(6, PGR.game_parameter4_value);
    prep_stmt->executeUpdate();

    PGR.is_valid = true;

    delete prep_stmt;
    delete con;

    std::cout << "Successfully created rating in game with game_id: " +
    std::to_string(PGR.game_id) +" for player with email: " +
    PGR.player_email<< std::endl;
  } catch (sql::SQLException &e) {
    std::cout << "Error creating rating in game with game_id: " +
    std::to_string(PGR.game_id) + " for player with email: " +
    PGR.player_email<< std::endl;

    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }

  return PGR;
}

Player DBService::remove_player(std::string player_email) {
  std::cout << "Connecting to MYSQL to delete player with email: " +
  player_email << std::endl;

  Player p;
  p.is_valid = false;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement  *prep_stmt;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Retrieve player to delete and return p if player does not exist
    p = get_player(player_email);
    if (!p.is_valid)
      return p;

    prep_stmt = con->prepareStatement("DELETE FROM Players "
    "WHERE player_email = ?;");
    prep_stmt->setString(1, player_email);

    prep_stmt->executeUpdate();

    delete prep_stmt;
    delete con;

    std::cout <<
    "Successfully deleted player with email: " + player_email << std::endl;
  } catch (sql::SQLException &e) {
    std::cout <<
    "Error deleting player with email: " + player_email << std::endl;
    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }

  return p;
}

Developer DBService::remove_developer(std::string developer_email) {
  std::cout << "Connecting to MYSQL to delete developer with email: " +
  developer_email << std::endl;

  Developer d;
  d.is_valid = false;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement  *prep_stmt;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Check if developer exists, return d if not
    d = get_developer(developer_email);
    if (!d.is_valid)
      return d;

    prep_stmt = con->prepareStatement("DELETE FROM Developers "
    "WHERE developer_email = ?;");
    prep_stmt->setString(1, developer_email);
    prep_stmt->executeUpdate();

    delete prep_stmt;
    delete con;

    std::cout <<
    "Successfully deleted developer with email: " + developer_email
    << std::endl;
  } catch (sql::SQLException &e) {
    std::cout <<
    "Error deleting developer with email: " + developer_email << std::endl;
    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }

  return d;
}

Game_Details DBService::remove_game_details(int game_id) {
  std::cout << "Connecting to MYSQL to delete game with game_id: " +
  std::to_string(game_id) << std::endl;

  Game_Details GD;
  GD.is_valid = false;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement  *prep_stmt;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Check if object exists, return if it does not
    GD = get_game_details(game_id);
    if (!GD.is_valid)
      return GD;

    prep_stmt = con->prepareStatement("DELETE FROM Game_Details "
    "WHERE game_id = ?;");
    prep_stmt->setInt(1, game_id);
    prep_stmt->executeUpdate();

    delete prep_stmt;
    delete con;

    std::cout <<
    "Successfully deleted game with game_id: " + std::to_string(game_id)
    << std::endl;
  } catch (sql::SQLException &e) {
    std::cout <<
    "Error deleting game with game_id: " + std::to_string(game_id)
    << std::endl;

    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }

  return GD;
}

Player_Game_Ratings
DBService::remove_player_rating(std::string player_email, int game_id) {
  std::cout << "Connecting to MYSQL to delete rating for player with email: " +
  player_email + " for game with game_id: " +
  std::to_string(game_id) << std::endl;

  Player_Game_Ratings PGR;
  PGR.is_valid = false;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement  *prep_stmt;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Check if object exists, return otherwise
    PGR = get_player_game_rating(player_email, game_id);
    if (!PGR.is_valid)
      return PGR;

    prep_stmt = con->prepareStatement("DELETE FROM Player_Game_Ratings "
    "WHERE player_email = ? AND game_id = ?;");
    prep_stmt->setString(1, player_email);
    prep_stmt->setInt(2, game_id);
    prep_stmt->executeUpdate();

    delete prep_stmt;
    delete con;

    std::cout << "Successfully deleted rating for player with email: " +
    player_email + " for game with game_id: "+
    std::to_string(game_id) << std::endl;
  } catch (sql::SQLException &e) {
    std::cout << "Error deleting rating for player with email: " +
    player_email + " for game with game_id: " +
    std::to_string(game_id) << std::endl;

    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }

  return PGR;
}

Player DBService::update_player(std::string player_email, Player P) {
  std::cout << "Connecting to MYSQL to update player with email: " +
  player_email << std::endl;

  // Object to store updated player
  Player p;
  p.is_valid = false;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement  *prep_stmt;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Retrieve player to update and return p if player does not exist
    p = get_player(player_email);
    if (!p.is_valid)
      return p;

    // Create statement and fill in relevant variables
    prep_stmt = con->prepareStatement("UPDATE Players SET player_email=? WHERE player_email=?;");
    prep_stmt->setString(1, P.player_email);
    prep_stmt->setString(2, player_email);

    // Execute statament and set flag
    prep_stmt->executeUpdate();
    p.is_valid = true;

    delete prep_stmt;
    delete con;

    std::cout <<
    "Successfully updated player with email: " + player_email << std::endl;
  } catch (sql::SQLException &e) {
    std::cout <<
    "Error updating player with email: " + player_email << std::endl;
    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }

  return p;
}

Developer DBService::update_developer(std::string developer_email, Developer D) {
  std::cout << "Connecting to MYSQL to update developer with email: " +
  developer_email << std::endl;

  // Object to store updated developer
  Developer d;
  d.is_valid = false;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement  *prep_stmt;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Check if developer exists, return d if not
    d = get_developer(developer_email);
    if (!d.is_valid)
      return d;

    // Create statement and fill in relevant variables
    prep_stmt = con->prepareStatement("UPDATE Developers SET developer_email=?, developer_password=? WHERE developer_email=?;");
    prep_stmt->setString(1, D.developer_email);
    prep_stmt->setString(2, D.developer_password);
    prep_stmt->setString(3, developer_email);

    // Execute statement and set flag
    prep_stmt->executeUpdate();
    d.is_valid = true;

    delete prep_stmt;
    delete con;

    std::cout << "Successfully updated developer with email: " +
    developer_email << std::endl;
  } catch (sql::SQLException &e) {
    std::cout <<
    "Error updating developer with email: " + developer_email << std::endl;
    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }
  return d;
}

Game_Details DBService::update_game_details(int game_id, Game_Details GD) {
  std::cout << "Connecting to MYSQL to update game" << std::endl;

  // Object to store updated Game_Details object
  Game_Details gd;
  gd.is_valid = false;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement  *prep_stmt;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Check if object exists, return if it does not
    gd = get_game_details(game_id);
    if (!gd.is_valid)
      return gd;

    // Create SQL statement
    prep_stmt = con->prepareStatement("UPDATE Game_Details SET "
    "game_id=?, game_name=?, developer_email=?, "
    "game_parameter1_name=?, game_parameter1_weight=?, "
    "game_parameter2_name=?, game_parameter2_weight=?, "
    "game_parameter3_name=?, game_parameter3_weight=?, "
    "game_parameter4_name=?, game_parameter4_weight=?, "
    "category=?, players_per_team=?, teams_per_match=? "
    "WHERE game_id=?;");

    // Fill in relevant variables in statement and execute
    prep_stmt->setInt(1, GD.game_id);
    prep_stmt->setString(2, GD.game_name);
    prep_stmt->setString(3, GD.developer_email);
    prep_stmt->setString(4, GD.game_parameter1_name);
    prep_stmt->setDouble(5, GD.game_parameter1_weight);
    prep_stmt->setString(6, GD.game_parameter2_name);
    prep_stmt->setDouble(7, GD.game_parameter2_weight);
    prep_stmt->setString(8, GD.game_parameter3_name);
    prep_stmt->setDouble(9, GD.game_parameter3_weight);
    prep_stmt->setString(10, GD.game_parameter4_name);
    prep_stmt->setDouble(11, GD.game_parameter4_weight);
    prep_stmt->setString(12, GD.category);
    prep_stmt->setInt(13, GD.players_per_team);
    prep_stmt->setInt(14, GD.teams_per_match);
    prep_stmt->setInt(15, game_id);
    prep_stmt->executeUpdate();

    gd.is_valid = true;

    delete prep_stmt;
    delete con;

    std::cout << "Successfully updated game with game_id: " +
    std::to_string(game_id) << std::endl;
  } catch (sql::SQLException &e) {
    std::cout << "Error updating game" << std::endl;
    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }

  return gd;
}

Player_Game_Ratings DBService::update_player_rating(std::string player_email, int game_id, Player_Game_Ratings PGR) {
  std::cout << "Connecting to MYSQL to update rating in game with game_id: " +
  std::to_string(game_id) + " for player with email: " +
  player_email << std::endl;

  // Object to store updated object
  Player_Game_Ratings pgr;
  pgr.is_valid = false;

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement  *prep_stmt;

    // Connect to database
    driver = get_driver_instance();
    con = driver->connect(hostname, username, password);
    con->setSchema(database);

    // Check if object exists, return otherwise
    pgr = get_player_game_rating(player_email, game_id);
    if (!pgr.is_valid)
      return pgr;

    // Create SQL statement
    prep_stmt = con->prepareStatement("UPDATE Player_Game_Ratings SET "
    "player_email=?, game_id=?, game_parameter1_value=?, "
    "game_parameter2_value=?, game_parameter3_value=?, "
    "game_parameter4_value=? WHERE player_email=? AND "
    "game_id=?;");

    // Fill in relevant variables in statement and execute
    prep_stmt->setString(1, PGR.player_email);
    prep_stmt->setInt(2, PGR.game_id);
    prep_stmt->setInt(3, PGR.game_parameter1_value);
    prep_stmt->setInt(4, PGR.game_parameter2_value);
    prep_stmt->setInt(5, PGR.game_parameter3_value);
    prep_stmt->setInt(6, PGR.game_parameter4_value);
    prep_stmt->setString(7, player_email);
    prep_stmt->setInt(8, game_id);
    prep_stmt->executeUpdate();
    pgr.is_valid = true;

    delete prep_stmt;
    delete con;

    std::cout << "Successfully updated rating in game with game_id: " +
    std::to_string(game_id) +" for player with email: " +
    player_email<< std::endl;
  } catch (sql::SQLException &e) {
    std::cout << "Error updating rating in game with game_id: " +
    std::to_string(game_id) + " for player with email: " +
    player_email<< std::endl;

    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
  }

  return pgr;
}
