// "Copyright [2022] <Copyright Owner>"

#include "crow/crow_all.h"
#include "main/db-service.h"

#include <cmath>
#include <stdlib.h>
#include <string.h>
#include <tuple>

std::vector<std::vector<std::vector<std::string> > > matchmaking(int game_id, std::vector<std::string> player_emails)
{
  // Step 0: Retrieve Game Details by Game ID
  Game_Details details = get_game_details(game_id);

  // Step 1: Compute Holistic Ranking for Each Player using their Metrics
  std::vector<std::tuple<float, std::string> > ranked_players; // vector to which players and their ranks will be added

  // Iterate through each passed player and computer their rank
  for (int i = 0; i < player_emails.size(); i++) {
    Player_Game_Ratings player_metrics = get_player_game_rating(player_emails.at(i), game_id);
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

  for (int i = 0; i < ranked_players.size(); i++) {
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
  if (player_emails.size() < num_players_per_game) {
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
    for (int i = player_iter; i < ranked_players.size(); i++)
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

    for (int i = 0; i < overflow.size(); i++) {
      std::cout << overflow.at(i) << std::endl;
    }
  }

  return games;
}

int main(void) {
  get_player("apex_player@gmail.com");
  std::cout << "\n";
  get_developer("developer@chess.com");
  std::cout << "\n";
  get_game_details(2);
  std::cout << "\n";
  get_player_game_rating("apex_player@gmail.com", 1);
  std::cout << "\n";
  get_joined_player_game_rating("apex_player@gmail.com", 1);
  std::cout << "\n";
  get_all_players();
  std::cout << "\n";
  get_all_developers();
  std::cout << "\n";
  get_all_games();
  std::cout << "\n";
  get_all_player_game_ratings_for_game(1);
  std::cout << "\n";
  get_all_games_for_developer("developer@chess.com");
  std::cout << "\n";

  Player P;
  P.player_email = "fake_player@gmail.com";
  P = add_player(P);
  if (P.is_valid)
    remove_player("fake_player@gmail.com");

  Developer D;
  D.developer_email = "fake_developer@dev.com";
  D.developer_password = "some_password";
  D = add_developer(D);
  if (D.is_valid)
    remove_developer("fake_developer@dev.com");

  Game_Details GD;
  GD.game_name = "fake game";
  GD.developer_email = "developer@chess.com";
  GD = add_game_details(GD);
  if (GD.is_valid)
    remove_game_details(GD.game_id);

  Player_Game_Ratings PGR;
  PGR.player_email = "apex_player@gmail.com";
  PGR.game_id = 2;
  PGR = add_player_rating(PGR);
  if (PGR.is_valid)
    remove_player_rating(PGR.player_email, PGR.game_id);

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
      D = get_developer(developer_email);
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
      D = add_developer(D);
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
      D = get_developer(developer_email);
      if (!D.is_valid) {
        return crow::response(400, "User not found");
      }

      if (D.developer_password != developer_password) {
        return crow::response(400, "Invalid credentials");
      }

      D = remove_developer(developer_email);
      if (!D.is_valid) {
        return crow::response(500, "Internal Server Error");
      }
      return crow::response(200, "Succesfully deleted account");
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
  });

  // set the port, set the app to run on multiple threads, and run the app
  app.port(18080).multithreaded().run();

  // Chess Matchmaking Testing
  std::vector<std::string> players_to_match;
  players_to_match.push_back("chess_player@gmail.com");
  players_to_match.push_back("apex_and_chess_player@gmail.com");
  players_to_match.push_back("magnus_carlsen@gmail.com");
  players_to_match.push_back("chess_newb@gmail.com");
  // players_to_match.push_back("chess_player_extraordinaire@gmail.com");

  matchmaking(2, players_to_match);

  // Overwatch Matchmaking Testing
  std::vector<std::string> players_to_match2;
  players_to_match2.push_back("overwatch_player_1@gmail.com");
  players_to_match2.push_back("overwatch_player_2@gmail.com");
  players_to_match2.push_back("overwatch_player_3@gmail.com");
  players_to_match2.push_back("overwatch_player_4@gmail.com");
  players_to_match2.push_back("overwatch_player_5@gmail.com");
  players_to_match2.push_back("overwatch_player_6@gmail.com");
  players_to_match2.push_back("overwatch_player_7@gmail.com");
  players_to_match2.push_back("overwatch_player_8@gmail.com");
  players_to_match2.push_back("overwatch_player_9@gmail.com");
  players_to_match2.push_back("overwatch_player_10@gmail.com");
  players_to_match2.push_back("overwatch_player_11@gmail.com");

  matchmaking(3, players_to_match2);

  return 0;
}
