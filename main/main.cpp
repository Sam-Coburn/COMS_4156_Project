// "Copyright [2022] <Copyright Owner>"

#include "crow/crow_all.h"
#include "main/db-service.h"
#include "api-endpoints/api-endpoints-lib.h"

#include <cmath>
#include <stdlib.h>
#include <string.h>
#include <tuple>

std::vector<std::vector<std::vector<std::string> > > matchmaking(int game_id, std::vector<std::string> player_emails)
{
  DBService DB = DBService();
  // Step 0: Retrieve Game Details by Game ID
  Game_Details details = DB.get_game_details(game_id);

  // Step 1: Compute Holistic Ranking for Each Player using their Metrics
  std::vector<std::tuple<float, std::string> > ranked_players; // vector to which players and their ranks will be added

  // Iterate through each passed player and computer their rank
  for (long unsigned int i = 0; i < player_emails.size(); i++) {
    Player_Game_Ratings player_metrics = DB.get_player_game_rating(player_emails.at(i), game_id);
    float rating = (details.game_parameter1_weight) * (player_metrics.game_parameter1_value) +
                   (details.game_parameter2_weight) * (player_metrics.game_parameter2_value) +
                   (details.game_parameter3_weight) * (player_metrics.game_parameter3_value) +
                   (details.game_parameter4_weight) * (player_metrics.game_parameter4_value);

    std::cout << rating << std::endl;

    // Add each player, rank tuple to the vector of ranked players
    ranked_players.push_back(make_tuple(rating, player_emails[i]));
  }

  // Sort the vector of ranked players by rank, starting with the highest value
  sort(ranked_players.begin(), ranked_players.end());
  reverse(ranked_players.begin(), ranked_players.end());

  for (long unsigned int i = 0; i < ranked_players.size(); i++) {
    std::cout << "Player " << i + 1 << ": " << std::get<1>(ranked_players.at(i)) << std::endl;
    std::cout << "Rank: " << std::get<0>(ranked_players.at(i)) << std::endl;
  }
  std::cout << std::endl;

  // Step 2: Populate Games w/ Players
  int num_players_per_game = details.players_per_team * details.teams_per_match;
  int num_games = floor(player_emails.size() / num_players_per_game);
  int num_overflow = player_emails.size() % num_players_per_game;

  std::cout << "# of Players Inputted: " << player_emails.size() << std::endl;

  std::cout << "# Players per Game: " << num_players_per_game << std::endl;
  std::cout << "# Games: " << num_games << std::endl;
  std::cout << "# Players in Overflow: " << num_overflow << std::endl;
  std::cout << std::endl;

  // If the number of players given is less than the number of players expected in a game,
  // add them all to overflow and return
  if (player_emails.size() < (long unsigned int) num_players_per_game) {
    std::vector<std::vector<std::vector<std::string> > > games;
    std::vector<std::vector<std::string> > teams;

    teams.push_back(player_emails);
    games.push_back(teams);

    return games;
  }

  // Populate the teams
  std::vector<std::vector<std::vector<std::string> > > games;
  int player_iter = 0;

  for (int i = 0; i < num_games; i++) {
    // Create empty vectors, representing each team in the game
    std::vector<std::vector<std::string> > teams;
    for (int j = 0; j < details.teams_per_match; j++) {
      std::vector<std::string> t;
      teams.push_back(t);
    }

    // Populate Each Team in the Game
    // - Iterate through the players, adding one to each team
    int players_ingame_count = 0;
    int team_number = 0;
    while (players_ingame_count < num_players_per_game) {
      teams.at(team_number).push_back(std::get<1>(ranked_players.at(player_iter)));

      player_iter++;
      players_ingame_count++;
      team_number++;

      if (team_number == details.teams_per_match)
        team_number = 0;
    }

    games.push_back(teams);
  }

  std::cout << player_iter << std::endl;

  // Populate Overflow with Remaining Players
  std::vector<std::string> overflow;
  if (num_overflow > 0) {
    for (long unsigned int i = player_iter; i < ranked_players.size(); i++)
      overflow.push_back(std::get<1>(ranked_players.at(i)));

    std::vector<std::vector<std::string> > overflow_team;
    overflow_team.push_back(overflow);

    games.push_back(overflow_team);
  }

  // Printout of Matchmaking Results
  std::cout << "MATCHMAKING OVERVIEW" << std::endl;
  std::cout << "Num Games: " << games.size() << std::endl;
  std::cout << "Num Teams: " << games.at(0).size() << std::endl;
  std::cout << "Teams Size: " << games.at(0).at(0).size() << std::endl;
  std::cout << std::endl;

  std::cout << "GAMES" << std::endl;
  for (int i = 0; i < num_games; i++) {
    std::cout << "Game " << i + 1 << std::endl;
    for (int j = 0; j < details.teams_per_match; j++) {
      std::cout << "Team " << j + 1 << std::endl;
      for (int k = 0; k < details.players_per_team; k++) {
        std::cout << "Player: " << games.at(i).at(j).at(k) << std::endl;
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }

  if (overflow.size() > 0) {
    std::cout << "OVERFLOW" << std::endl;

    for (long unsigned int i = 0; i < overflow.size(); i++) {
      std::cout << overflow.at(i) << std::endl;
    }
  }

  return games;
}

int main(void) {
  DBService DB = DBService();
  DB.get_player("apex_player@gmail.com");
  std::cout << "\n";
  DB.get_developer("developer@chess.com");
  std::cout << "\n";
  DB.get_game_details(2);
  std::cout << "\n";
  DB.get_player_game_rating("apex_player@gmail.com", 1);
  std::cout << "\n";
  DB.get_joined_player_game_rating("apex_player@gmail.com", 1);
  std::cout << "\n";
  DB.get_all_players();
  std::cout << "\n";
  DB.get_all_developers();
  std::cout << "\n";
  DB.get_all_games();
  std::cout << "\n";
  DB.get_all_player_game_ratings_for_game(1);
  std::cout << "\n";
  DB.get_all_games_for_developer("developer@chess.com");
  std::cout << "\n";

  Player P;
  P.player_email = "fake_player@gmail.com";
  P = DB.add_player(P);
  if (P.is_valid)
    DB.remove_player("fake_player@gmail.com");

  Developer D;
  D.developer_email = "fake_developer@dev.com";
  D.developer_password = "some_password";
  D = DB.add_developer(D);
  if (D.is_valid)
    DB.remove_developer("fake_developer@dev.com");

  Game_Details GD;
  GD.game_name = "fake game";
  GD.developer_email = "developer@chess.com";
  GD = DB.add_game_details(GD);
  if (GD.is_valid)
    DB.remove_game_details(GD.game_id);

  Player_Game_Ratings PGR;
  PGR.player_email = "apex_player@gmail.com";
  PGR.game_id = 2;
  PGR = DB.add_player_rating(PGR);
  if (PGR.is_valid)
    DB.remove_player_rating(PGR.player_email, PGR.game_id);

  D.developer_email = "fake_developer@dev.com";
  D.developer_password = "some_password";
  D = DB.add_developer(D);
  if (D.is_valid)
    DB.remove_developer("fake_developer@dev.com");

  crow::SimpleApp app;  // define your crow application
  // define your endpoint at the root directory
  CROW_ROUTE(app, "/")([](){
    return "Hello world";
  });

  // Login doesn't do much since we are always expecting to get the
  // email and password with every request
  CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req){
    crow::json::rvalue x = crow::json::load(req.body);
    std::string developer_email;
    std::string developer_password;
    Developer D;

    try {
      developer_email = x["developer_email"].s();
      developer_password = x["developer_password"].s();
      DBService DB = DBService();
      D = DB.get_developer(developer_email);
      if (!D.is_valid || D.developer_password != developer_password) {
        return crow::response(400, "Invalid credentials");
      }
      return crow::response(200, "Succesfully logged in");
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
  });

  CROW_ROUTE(app, "/signup").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req){
    crow::json::rvalue x = crow::json::load(req.body);
    Developer D;
    try {
      D.developer_email = x["developer_email"].s();
      D.developer_password = x["developer_password"].s();
      DBService DB = DBService();
      D = DB.add_developer(D);
      if (!D.is_valid) {
        return crow::response(400, "Developer already exists");
      }
      return crow::response(200, "Succesfully signed up");
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
  });

  CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::DELETE)
  ([](const crow::request& req){
    crow::json::rvalue x = crow::json::load(req.body);
    std::string developer_email;
    std::string developer_password;
    Developer D;

    try {
      developer_email = x["developer_email"].s();
      developer_password = x["developer_password"].s();
      DBService DB = DBService();
      D = DB.get_developer(developer_email);
      if (!D.is_valid) {
        return crow::response(400, "User not found");
      }

      if (D.developer_password != developer_password) {
        return crow::response(400, "Invalid credentials");
      }

      D = DB.remove_developer(developer_email);
      if (!D.is_valid) {
        return crow::response(500, "Internal Server Error");
      }
      return crow::response(200, "Succesfully deleted account");
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
  });

  CROW_ROUTE(app, "/games").methods(crow::HTTPMethod::GET)
  ([](const crow::request& req){
    // APIEndPoints api = APIEndPoints();
    // std::pair<int, std::string> rsp = api.getGames(req);
    // return crow::response(rsp.first, rsp.second);
       return crow::response(400, "Emily needs to use crow jsons.");
  });

  CROW_ROUTE(app, "/games").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req){
    // APIEndPoints api = APIEndPoints();
    // std::pair<int, std::string> rsp = api.postGames(req);
    // return crow::response(rsp.first, rsp.second);
    return crow::response(400, "Emily needs to use crow jsons.");
  });

  // Get all players in a certain game
  CROW_ROUTE(app, "/game/<int>/players").methods(crow::HTTPMethod::GET)
  ([](int game_id){
    std::vector<Joined_Player_Game_Ratings> joined_players;

    try {
      DBService DB = DBService();
      joined_players = DB.get_all_player_game_ratings_for_game(game_id);

      if (joined_players.empty()) {
        return crow::response(204, "No players found for game_id " + std::to_string(game_id));
      }
      
      // Converting vector of players to json object
      crow::json::wvalue players;
      for (Joined_Player_Game_Ratings p : joined_players) {
        if (p.is_valid) {
          players[p.player_email]["game_id"] = p.game_id;
          players[p.player_email][p.game_parameter1_name] = p.game_parameter1_value;
          players[p.player_email][p.game_parameter2_name] = p.game_parameter2_value;
          players[p.player_email][p.game_parameter3_name] = p.game_parameter3_value;
          players[p.player_email][p.game_parameter4_name] = p.game_parameter4_value;
        } else {
          return crow::response(500, "Internal Server Error due to player " + p.player_email);
        }
      }

      return crow::response(200, players);
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
  });

//return everything about the game details to the user except game_id and developer_email
  CROW_ROUTE(app, "/game/<int>").methods(crow::HTTPMethod::GET)
  ([](int game_id){
    Game_Details game_details;
    try {
      DBService DB = DBService();
      game_details = DB.get_game_details(game_id);
      crow::json::wvalue game_json;
      if (game_details.is_valid) {
        game_json["game_id"] = game_details.game_id;
        game_json["game_name"] = game_details.game_name;
        game_json["game_parameter1_name"] = game_details.game_parameter1_name;
        game_json["game_parameter1_weight"] = game_details.game_parameter1_weight;
        game_json["game_parameter2_name"] = game_details.game_parameter2_name;
        game_json["game_parameter2_weight"] = game_details.game_parameter2_weight;
        game_json["game_parameter3_name"] = game_details.game_parameter3_name;
        game_json["game_parameter3_weight"] = game_details.game_parameter3_weight;
        game_json["game_parameter4_name"] = game_details.game_parameter4_name;
        game_json["game_parameter4_weight"] = game_details.game_parameter4_weight;
        game_json["category"] = game_details.category;
        game_json["players_per_team"] = game_details.players_per_team;
        game_json["teams_per_match"] = game_details.teams_per_match;

      } else {
        return crow::response(400, "Invalid request body");
      }

      return crow::response(200, game_json);
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
  });

  // Add given player stats for a certain game
  CROW_ROUTE(app, "/game/<int>/players").methods(crow::HTTPMethod::POST) 
  ([](const crow::request& req, int game_id){
    try {
      DBService DB = DBService();
      crow::json::rvalue players_adding = crow::json::load(req.body);

      for (std::string p : players_adding.keys()) {
        Player_Game_Ratings pgr;
        pgr.player_email = p;
        pgr.game_id = game_id;
        pgr.game_parameter1_value = players_adding[p]["game_parameter1_value"].i();
        pgr.game_parameter2_value = players_adding[p]["game_parameter2_value"].i();
        pgr.game_parameter3_value = players_adding[p]["game_parameter3_value"].i();
        pgr.game_parameter4_value = players_adding[p]["game_parameter4_value"].i();
        
        pgr = DB.add_player_rating(pgr);
        if (!pgr.is_valid) {
          return crow::response(500, "Internal Server Error due to player " + p);
        }
      }

      return crow::response(200, "Player stats were added");
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
  });
    
  //create set for PUT request to not reset all elements and to catch errors
  CROW_ROUTE(app, "/game/<int>").methods(crow::HTTPMethod::PUT)
  ([](const crow::request& req, int game_id){
    try {
      crow::json::rvalue game_body = crow::json::load(req.body);
      std::set<std::string> all_keys = {"game_name", "developer_email"
      "game_parameter1_name", "game_parameter1_weight", 
      "game_parameter2_name", "game_parameter2_weight",
      "game_parameter3_name", "game_parameter3_weight",
      "game_parameter4_name", "game_parameter4_weight",
      "category", "players_per_team", "teams_per_match"};

      Game_Details game_details;
      for (std::string p : game_body.keys()) {
        if (all_keys.count(p)) {
            game_details.game_id = game_id;
            if (p == "game_name") {
              game_details.game_name = game_body["game_name"].s();
            }
            else if (p == "developer_email") {
              game_details.developer_email = game_body["developer_email"].s();
            }
            else if (p == "game_parameter1_name") {
              game_details.game_parameter1_name = game_body["game_parameter1_name"].s();
            }
            else if (p == "game_parameter1_weight") {
              game_details.game_parameter1_weight = game_body["game_parameter1_weight"].d();
            }
            else if (p == "game_parameter2_name") {
              game_details.game_parameter2_name = game_body["game_parameter2_name"].s();
            }
            else if (p == "game_parameter2_weight") {
              game_details.game_parameter2_weight = game_body["game_parameter2_weight"].d();
            }
            else if (p == "game_parameter3_name") {
              game_details.game_parameter3_name = game_body["game_parameter3_name"].s();
            }
            else if (p == "game_parameter3_weight") {
              game_details.game_parameter3_weight = game_body["game_parameter3_weight"].d();
            }
            else if (p == "game_parameter4_name") {
              game_details.game_parameter4_name = game_body["game_parameter4_name"].s();
            }
            else if (p == "game_parameter4_weight") {
              game_details.game_parameter4_weight = game_body["game_parameter4_weight"].d();
            }
            else if (p == "category") {
              game_details.category = game_body["category"].s();
            }
            else if (p == "players_per_team") {
              game_details.players_per_team = game_body["players_per_team"].i();
            }
            else if (p == "teams_per_match") {
              game_details.teams_per_match = game_body["teams_per_match"].i();
            }
            else {
              return crow::response(400, "Invalid request body");
            }

        } else {
          return crow::response(400, "Invalid request body");
        }
      }

      return crow::response(200, "Added requested game details");
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
  });

  // Get a specific player's stats for a specific game
  CROW_ROUTE(app, "/game/<int>/players/<string>").methods(crow::HTTPMethod::GET)
  ([](int game_id, std::string player_email) {
    Player_Game_Ratings pgr;

    try {
      DBService DB = DBService();
      pgr = DB.get_player_game_rating(player_email, game_id);

      if (!pgr.is_valid) {
        return crow::response(204, "Player " + player_email + " not found for game_id " + std::to_string(game_id));
      }
      
      // Converting players' stats to json object
      crow::json::wvalue stats;
      if (pgr.is_valid) {
        stats["player_email"] = pgr.player_email;
        stats["game_parameter1_value"] = pgr.game_parameter1_value;
        stats["game_parameter2_value"] = pgr.game_parameter2_value;
        stats["game_parameter3_value"] = pgr.game_parameter3_value;
        stats["game_parameter4_value"] = pgr.game_parameter4_value;
      } else {
        return crow::response(500, "Internal Server Error");
      }

      return crow::response(200, stats);
    }
  catch(...) {
      return crow::response(400, "Invalid request body");
    }
  });

  CROW_ROUTE(app, "/game/<int>").methods(crow::HTTPMethod::DELETE)
  ([](const crow::request& req, int game_id){
    crow::json::rvalue x = crow::json::load(req.body);
    Game_Details game_details;
    try {
      DBService DB = DBService();
      game_details = DB.get_game_details(game_id);
      if (game_details.is_valid) {
        DB.remove_game_details(game_details.game_id);
      } else {
        return crow::response(400, "Invalid request body");
      }
      return crow::response(200, "Succesfully deleted game details");
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
  });

  // Remove given player stats for a certain game
  CROW_ROUTE(app, "/game/<int>/players").methods(crow::HTTPMethod::DELETE) 
  ([](const crow::request& req, int game_id){
    try {
      DBService DB = DBService();
      crow::json::rvalue players_adding = crow::json::load(req.body);

      for (std::string p : players_adding.keys()) {
        Player_Game_Ratings pgr;
        pgr = DB.remove_player_rating(p, game_id);
        if (!pgr.is_valid) {
          return crow::response(500, "Internal Server Error due to player " + p);
        }
      }

      return crow::response(200, "Player stats were removed");
    } catch (...) {
      return crow::response(400, "Invalid request body");
    }
  });

  
  // curl POST Test Example
  // curl -X POST -H "Content-Type: application/json" http://0.0.0.0:18080/matchmake -d '{"game_id": "2", "player_emails": ["chess_newb@gmail.com", "magnus_carlsen@gmail.com"]}'
  CROW_ROUTE(app, "/matchmake").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req) {
    crow::json::rvalue request_body = crow::json::load(req.body);

    std::string developer_email;
    std::string developer_password;
    int game_id;
    crow::json::rvalue input_player_emails_rvalue;
    std::vector<crow::json::rvalue> input_player_emails;
    std::vector<std::string> player_emails;

    try {
      DBService DB = DBService();
      crow::json::wvalue json_result;

      // Authentication
      developer_email = request_body["developer_email"];
      developer_password = request_body["developer_password"];

      Developer d = DB.get_developer(developer_email);

      // Check given password against password in DB
      if (d.developer_password != developer_password)
        return crow::response(400, json_result);

      game_id = request_body["game_id"].i();
      std::vector<Game_Details> developer_games = DB.get_all_games_for_developer(developer_email);
      bool game_found = false;

      // Check to ensure that the developer "owns" the given game
      for (int i = 0; i < developer_games.size(); i++) {
        int g_id = developer_games.at(i).game_id;
        if (game_id == g_id)
          game_found = true;
      }

      if (game_found == false)
        return crow::response(400, json_result);

      input_player_emails_rvalue = request_body["player_emails"];
      input_player_emails = input_player_emails_rvalue.lo();

      for (crow::json::rvalue email : input_player_emails) {
        player_emails.push_back(email.s());
        std::cout << email.s() << std::endl;
      }

      std::vector<std::vector<std::vector<std::string> > > result = matchmaking(game_id, player_emails);
      // json_result["json"] = result;

      for (int i = 0; i < matchmaking.size(); i++) {
        std::vector<std::vector<std::string> > game;
        json_result["Game" + std::to_string(i + 1)] = game;
        for (int j = 0; j < matchmaking.at(i).size(); j++) {
          std::vector<std::string> team;
          json_result["Game" + std::to_string(i + 1)].push_back(team);
          for (int k = 0; k < matchmaking.at(i).at(j).size(); k++) {
            json_result["Game" + std::to_string(i + 1)]["Team" + std::to_string(j + 1)].push_back(matchmaking.at(i).at(j).at(k))
          }
        }
      }

      return crow::response(200, json_result);
    }
    catch(...) {
      // Return a response to indicate an invalid request body
      // Empty JSON == invalid request
      crow::json::wvalue json_result;
      return crow::response(400, json_result);
    }
  });

  // set the port, set the app to run on multiple threads, and run the app
  app.port(18080).multithreaded().run();

  return 0;
}
