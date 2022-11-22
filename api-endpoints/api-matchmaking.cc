// Copyright [2022] RaisingCanesFanClub
#include "api-endpoints/api-matchmaking-lib.h"

std::tuple<
std::vector<std::vector<std::vector<std::string> > >,
std::vector<std::string> >
Matchmaking::matchmakingBackendBasic(int game_id, std::vector<std::string> player_emails, DBService* DB) {
  // Step 0: Retrieve Game Details by Game ID
  Game_Details details = DB->get_game_details(game_id);

  // Step 1: Compute Holistic Ranking for Each Player using their Metrics
  std::vector<std::tuple<float, std::string> > ranked_players;  // vector to which players and their ranks will be added

  // Iterate through each passed player and computer their rank
  for (uint64_t i = 0; i < player_emails.size(); i++) {
    Player_Game_Ratings player_metrics = DB->get_player_game_rating(player_emails.at(i), game_id);
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

  // Step 2: Populate Games w/ Players
  int num_players_per_game = details.players_per_team * details.teams_per_match;
  int num_games = floor(player_emails.size() / num_players_per_game);
  int num_overflow = player_emails.size() % num_players_per_game;

  // If the number of players given is less than the number of players expected in a game,
  // add them all to overflow and return
  if (player_emails.size() < (uint64_t) num_players_per_game) {
    std::vector<std::vector<std::vector<std::string> > > games;

    return make_tuple(games, player_emails);
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

  // Populate Overflow with Remaining Players
  std::vector<std::string> overflow;
  if (num_overflow > 0) {
    for (uint64_t i = player_iter; i < ranked_players.size(); i++)
      overflow.push_back(std::get<1>(ranked_players.at(i)));
  }

  return make_tuple(games, overflow);
}

std::tuple<
std::vector<std::vector<std::vector<std::string> > >,
std::vector<std::string> >
Matchmaking::matchmakingBackendAdvanced(int game_id, std::vector<std::string> player_emails, DBService* DB) {
  // Step 0: Retrieve Game Details by Game ID
  Game_Details details = DB->get_game_details(game_id);
  std::string category = details.category;

  std::vector<Game_Details> all_games = DB->get_all_games();
  std::vector<std::vector<std::tuple<int, std::string> > > category_rankings;
  for (uint64_t i = 0; i < all_games.size(); i++) {
    Game_Details current_game = all_games.at(i);
    if (current_game.category == category) {
      int current_game_id = current_game.game_id;
      std::vector<std::tuple<float, std::string> > current_ranked_players;

      for (uint64_t j = 0; j < player_emails.size(); j++) {
        Player_Game_Ratings player_metrics = DB->get_player_game_rating(player_emails.at(j), current_game_id);
        float rating = 0.0;

        if (player_metrics.is_valid == true) {
          rating = (current_game.game_parameter1_weight) * (player_metrics.game_parameter1_value) +
                   (current_game.game_parameter2_weight) * (player_metrics.game_parameter2_value) +
                   (current_game.game_parameter3_weight) * (player_metrics.game_parameter3_value) +
                   (current_game.game_parameter4_weight) * (player_metrics.game_parameter4_value);
        }

        current_ranked_players.push_back(make_tuple(rating, player_emails.at(j)));
      }

      sort(current_ranked_players.begin(), current_ranked_players.end());
      reverse(current_ranked_players.begin(), current_ranked_players.end());

      // Replace Game-Specific Rankings with Players Rank in List
      std::vector<std::tuple<int, std::string> > current_ranked_players_int;
      for (uint64_t j = 0; j < current_ranked_players.size(); j++) {
        if (std::get<0>(current_ranked_players.at(j)) == 0)
          current_ranked_players_int.push_back(make_tuple(current_ranked_players.size(),
          std::get<1>(current_ranked_players.at(j))));
        else
          current_ranked_players_int.push_back(make_tuple(j + 1, std::get<1>(current_ranked_players.at(j))));
      }

      category_rankings.push_back(current_ranked_players_int);
    }
  }

  // Organize Relative Player Ranks by Player Email
  std::map<std::string, std::vector<int> > players_and_rankings;
  for (uint64_t i = 0; i < category_rankings.size(); i++) {
    std::vector<std::tuple<int, std::string> > current_game_rankings = category_rankings.at(i);
    for (uint64_t j = 0; j < current_game_rankings.size(); j++) {
      std::string current_player = std::get<1>(current_game_rankings.at(j));
      players_and_rankings[current_player].push_back(std::get<0>(current_game_rankings.at(j)));
    }
  }

  // Populate Final Vector of Average Player Rankings
  std::vector<std::tuple<float, std::string> > ranked_players;
  for (uint64_t i = 0; i < player_emails.size(); i++) {
    std::string current_player = player_emails.at(i);
    std::vector<int> current_player_ranks = players_and_rankings[current_player];

    float average_rank = 0.0;
    for (uint64_t j = 0; j < current_player_ranks.size(); j++) {
      average_rank += current_player_ranks.at(j);
    }
    average_rank = average_rank / current_player_ranks.size();

    ranked_players.push_back(make_tuple(average_rank, current_player));
  }

  sort(ranked_players.begin(), ranked_players.end());

  // Step 2: Populate Games w/ Players
  int num_players_per_game = details.players_per_team * details.teams_per_match;
  int num_games = floor(player_emails.size() / num_players_per_game);
  int num_overflow = player_emails.size() % num_players_per_game;

  // If the number of players given is less than the number of players expected in a game,
  // add them all to overflow and return
  if (player_emails.size() < (uint64_t) num_players_per_game) {
    std::vector<std::vector<std::vector<std::string> > > games;

    return make_tuple(games, player_emails);
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

  // Populate Overflow with Remaining Players
  std::vector<std::string> overflow;
  if (num_overflow > 0) {
    for (uint64_t i = player_iter; i < ranked_players.size(); i++)
      overflow.push_back(std::get<1>(ranked_players.at(i)));
  }

  return make_tuple(games, overflow);
}
