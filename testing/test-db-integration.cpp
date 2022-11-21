// "Copyright [year] <Copyright Owner>"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "main/db-service.h"

class DBServiceTestFixture: public testing::Test {
 public:
    static DBService DB;

    virtual void SetUp() {
      DB.tearDownDatabase();
      DB.setUpDatabase();
    }

    virtual void TearDown() {
      DB.tearDownDatabase();
    }

    static void SetUpTestCase() {
      DB = DBService("tcp://127.0.0.1:3306", "dbuser", "123", "test_matchmaking_api_db");
    }

    static void TearDownTestCase() {
      DB.tearDownDatabase();
    }
};

DBService DBServiceTestFixture::DB;

TEST_F(DBServiceTestFixture, Player_Tests) {
  Player P;
  P.player_email = "fake_player@gmail.com";

  Player update_details;
  update_details.player_email = "updated_email@gmail.com";

  Player updated_player;

  // Invalid Update, player doesn't exist
  updated_player = DB.update_player(P.player_email, update_details);
  ASSERT_EQ(updated_player.is_valid, false);

  P = DB.add_player(P);  // Valid create
  ASSERT_EQ(P.is_valid, true);

  P = DB.add_player(P);  // invalid create (duplicate)
  ASSERT_EQ(P.is_valid, false);

  P = DB.get_player("fake_player@gmail.com");  // Valid read
  ASSERT_EQ(P.is_valid, true);
  ASSERT_EQ(P.player_email, "fake_player@gmail.com");

  // Valid Update
  updated_player = DB.update_player(P.player_email, update_details);
  ASSERT_EQ(updated_player.is_valid, true);

  P = DB.get_player("updated_email@gmail.com");  // Valid read
  ASSERT_EQ(P.is_valid, true);
  ASSERT_EQ(P.player_email, "updated_email@gmail.com");

  P = DB.remove_player("updated_email@gmail.com");  // Valid delete
  ASSERT_EQ(P.is_valid, true);

  P = DB.remove_player("updated_email@gmail.com");  // Invalid delete
  ASSERT_EQ(P.is_valid, false);

  P = DB.get_player("updated_email@gmail.com");  // Invalid read
  ASSERT_EQ(P.is_valid, false);

  // Get_all test
  std::vector<Player> players;
  players = DB.get_all_players();
  ASSERT_EQ(players.size(), 0);

  P.player_email = "fake_player@gmail.com";
  P = DB.add_player(P);  // Valid create
  ASSERT_EQ(P.is_valid, true);

  P.player_email = "fake_player_again@gmail.com";
  P = DB.add_player(P);  // Valid create
  ASSERT_EQ(P.is_valid, true);

  players = DB.get_all_players();
  ASSERT_EQ(players.size(), 2);
}

TEST_F(DBServiceTestFixture, Developer_Tests) {
  Developer D;
  D.developer_email = "fake_developer@dev.com";
  D.developer_password = "some_password";

  Developer update_details;
  update_details.developer_email = "updated_email@gmail.com";
  update_details.developer_password = "updated_password";

  Developer updated_developer;

  // Invalid Update, developer doesn't exist
  updated_developer = DB.update_developer(D.developer_email, update_details);
  ASSERT_EQ(updated_developer.is_valid, false);

  D = DB.add_developer(D);  // Valid create
  ASSERT_EQ(D.is_valid, true);

  D = DB.add_developer(D);  // Invalid create (duplicate)
  ASSERT_EQ(D.is_valid, false);

  D = DB.get_developer("fake_developer@dev.com");  // Valid read
  ASSERT_EQ(D.is_valid, true);
  ASSERT_EQ(D.developer_email, "fake_developer@dev.com");
  ASSERT_EQ(D.developer_password, "some_password");

  // Valid Update
  updated_developer = DB.update_developer(D.developer_email, update_details);
  ASSERT_EQ(updated_developer.is_valid, true);

  D = DB.get_developer("updated_email@gmail.com");  // Valid read
  ASSERT_EQ(D.is_valid, true);
  ASSERT_EQ(D.developer_email, "updated_email@gmail.com");
  ASSERT_EQ(D.developer_password, "updated_password");

  D = DB.remove_developer("updated_email@gmail.com");  // Valid delete
  ASSERT_EQ(D.is_valid, true);

  D = DB.remove_developer("updated_email@gmail.com");  // Invalid delete
  ASSERT_EQ(D.is_valid, false);

  D = DB.get_developer("updated_email@gmail.com");  // Invalid read
  ASSERT_EQ(D.is_valid, false);

  // Get_all test
  std::vector<Developer> developers;
  developers = DB.get_all_developers();
  ASSERT_EQ(developers.size(), 0);

  D.developer_email = "fake_developer@dev.com";
  D.developer_email = "some_password";
  D = DB.add_developer(D);  // Valid create
  ASSERT_EQ(D.is_valid, true);

  D.developer_email = "fake_developer_again@dev.com";
  D.developer_email = "other_password";
  D = DB.add_developer(D);  // Valid create
  ASSERT_EQ(D.is_valid, true);

  developers = DB.get_all_developers();
  ASSERT_EQ(developers.size(), 2);
}

TEST_F(DBServiceTestFixture, Game_Details_Tests) {
  Game_Details GD;
  GD.game_name = "fake game";
  GD.developer_email = "fake_developer@dev.com";
  GD.game_parameter1_name = "wins";
  GD.game_parameter1_weight = 1.0;
  GD.game_parameter2_name = "defeats";
  GD.game_parameter2_weight = -0.7;
  GD.game_parameter3_name = "kills";
  GD.game_parameter3_weight = 0.8;
  GD.game_parameter4_name = "assists";
  GD.game_parameter4_weight = 0.1;

  Game_Details update_details;
  update_details.game_name = "updated game name";
  update_details.developer_email = "some_other_email@gmail.com";

  Game_Details updated_game_details;

  GD = DB.add_game_details(GD);  // Invalid create (nonexistent developer)
  ASSERT_EQ(GD.is_valid, false);

  Developer D;
  D.developer_email = "fake_developer@dev.com";
  D.developer_password = "some_password";

  D = DB.add_developer(D);  // Valid create
  ASSERT_EQ(D.is_valid, true);

  // Invalid Update, game doesn't exist
  updated_game_details = DB.update_game_details(-1, update_details);
  ASSERT_EQ(updated_game_details.is_valid, false);

  GD.game_name = "fake game";
  GD.developer_email = "fake_developer@dev.com";
  GD = DB.add_game_details(GD);  // Valid Create
  ASSERT_EQ(GD.is_valid, true);
  // *NOTE* Should we make game names unique per developer?

  GD = DB.get_game_details(GD.game_id);  // Valid Read
  ASSERT_EQ(GD.is_valid, true);
  ASSERT_EQ(GD.developer_email, "fake_developer@dev.com");
  ASSERT_EQ(GD.game_name, "fake game");
  ASSERT_EQ(GD.game_parameter1_name, "wins");
  ASSERT_FLOAT_EQ(GD.game_parameter1_weight, 1.0);
  ASSERT_EQ(GD.game_parameter2_name, "defeats");
  ASSERT_FLOAT_EQ(GD.game_parameter2_weight, -0.7);
  ASSERT_EQ(GD.game_parameter3_name, "kills");
  ASSERT_FLOAT_EQ(GD.game_parameter3_weight, 0.8);
  ASSERT_EQ(GD.game_parameter4_name, "assists");
  ASSERT_FLOAT_EQ(GD.game_parameter4_weight, 0.1);

  // Invalid Update, developer doesn't exist
  update_details.game_id = GD.game_id;
  update_details.developer_email = "some_other_email@gmail.com";
  update_details.game_name = "updated game name";
  updated_game_details = DB.update_game_details(GD.game_id, update_details);
  ASSERT_EQ(updated_game_details.is_valid, false);

  D.developer_email = "some_other_email@gmail.com";
  D = DB.add_developer(D);  // Valid create
  ASSERT_EQ(D.is_valid, true);

  // Valid Update
  updated_game_details = DB.update_game_details(GD.game_id, update_details);
  ASSERT_EQ(updated_game_details.is_valid, true);
  ASSERT_EQ(updated_game_details.developer_email, "some_other_email@gmail.com");
  ASSERT_EQ(updated_game_details.game_name, "updated game name");

  GD = DB.remove_game_details(updated_game_details.game_id);  // Valid delete
  ASSERT_EQ(GD.is_valid, true);

  GD = DB.get_game_details(-1);  // Invalid Read
  ASSERT_EQ(GD.is_valid, false);

  GD = DB.remove_game_details(-1);  // Invalid delete
  ASSERT_EQ(GD.is_valid, false);

  // Get_all tests
  std::vector<Game_Details> games;
  games = DB.get_all_games();
  ASSERT_EQ(games.size(), 0);

  Game_Details Game1;
  Game1.game_name = "fake game";
  Game1.developer_email = "fake_developer@dev.com";

  Game_Details Game2;
  Game2.game_name = "fake game 2";
  Game2.developer_email = "some_other_email@gmail.com";

  Game_Details Game3;
  Game3.game_name = "fake game 3";
  Game3.developer_email = "fake_developer@dev.com";

  Game1 = DB.add_game_details(Game1);  // Valid Create
  ASSERT_EQ(Game1.is_valid, true);
  ASSERT_EQ(Game1.developer_email, "fake_developer@dev.com");
  ASSERT_EQ(Game1.game_name, "fake game");

  Game2 = DB.add_game_details(Game2);  // Valid Create
  ASSERT_EQ(Game2.is_valid, true);
  ASSERT_EQ(Game2.developer_email, "some_other_email@gmail.com");
  ASSERT_EQ(Game2.game_name, "fake game 2");

  Game3 = DB.add_game_details(Game3);  // Valid Create
  ASSERT_EQ(Game3.is_valid, true);
  ASSERT_EQ(Game3.developer_email, "fake_developer@dev.com");
  ASSERT_EQ(Game3.game_name, "fake game 3");

  games = DB.get_all_games_for_developer("NONEXISTENT@fake.com");
  ASSERT_EQ(games.size(), 0);

  games = DB.get_all_games_for_developer("some_other_email@gmail.com");
  ASSERT_EQ(games.size(), 1);

  games = DB.get_all_games_for_developer("fake_developer@dev.com");
  ASSERT_EQ(games.size(), 2);

  games = DB.get_all_games();
  ASSERT_EQ(games.size(), 3);
}

TEST_F(DBServiceTestFixture, Player_Game_Ratings_Tests) {
  Joined_Player_Game_Ratings JPGR;
  Player_Game_Ratings PGR;
  PGR.player_email = "fake_player@gmail.com";
  PGR.game_id = -1;
  PGR.game_parameter1_value = 10;
  PGR.game_parameter2_value = 5;
  PGR.game_parameter3_value = 100;
  PGR.game_parameter4_value = 20;

  // Invalid Create (nonexistent player and game)
  PGR = DB.add_player_rating(PGR);
  ASSERT_EQ(PGR.is_valid, false);

  // Valid create
  Player P;
  P.player_email = "fake_player@gmail.com";
  P = DB.add_player(P);
  ASSERT_EQ(P.is_valid, true);

  // Invalid Create (nonexistent game)
  PGR.player_email = "fake_player@gmail.com";
  PGR.game_id = -1;
  PGR.game_parameter1_value = 10;
  PGR.game_parameter2_value = 5;
  PGR.game_parameter3_value = 100;
  PGR.game_parameter4_value = 20;
  PGR = DB.add_player_rating(PGR);
  ASSERT_EQ(PGR.is_valid, false);

  // Valid create
  Developer D;
  D.developer_email = "fake_developer@dev.com";
  D.developer_password = "some_password";
  D = DB.add_developer(D);
  ASSERT_EQ(D.is_valid, true);

  // Valid Create
  Game_Details GD;
  GD.game_name = "fake game";
  GD.developer_email = "fake_developer@dev.com";
  GD.game_parameter1_name = "wins";
  GD.game_parameter1_weight = 1.0;
  GD.game_parameter2_name = "defeats";
  GD.game_parameter2_weight = -0.7;
  GD.game_parameter3_name = "kills";
  GD.game_parameter3_weight = 0.8;
  GD.game_parameter4_name = "assists";
  GD.game_parameter4_weight = 0.1;
  GD = DB.add_game_details(GD);
  ASSERT_EQ(GD.is_valid, true);

  // Invalid Create (nonexistent player)
  PGR.player_email = "nonexistent@gmail.com";
  PGR.game_id = GD.game_id;
  PGR.game_parameter1_value = 10;
  PGR.game_parameter2_value = 5;
  PGR.game_parameter3_value = 100;
  PGR.game_parameter4_value = 20;
  PGR = DB.add_player_rating(PGR);
  ASSERT_EQ(PGR.is_valid, false);

  // Invalid Read (rating does not exist)
  PGR = DB.get_player_game_rating("fake_player@gmail.com", GD.game_id);
  ASSERT_EQ(PGR.is_valid, false);

  // Invalid Read (rating does not exist)
  JPGR = DB.get_joined_player_game_rating("fake_player@gmail.com", GD.game_id);
  ASSERT_EQ(JPGR.is_valid, false);

  // Invalid Read (player does not exist)
  PGR = DB.get_player_game_rating("NONEXISTENT@gmail.com", GD.game_id);
  ASSERT_EQ(PGR.is_valid, false);

  // Invalid Read (player does not exist)
  JPGR = DB.get_joined_player_game_rating("NONEXISTENT@gmail.com", GD.game_id);
  ASSERT_EQ(JPGR.is_valid, false);

  // Invalid Read (game does not exist)
  PGR = DB.get_player_game_rating("fake_player@gmail.com", -1);
  ASSERT_EQ(PGR.is_valid, false);

  // Invalid Read (game does not exist)
  JPGR = DB.get_joined_player_game_rating("fake_player@gmail.com", -1);
  ASSERT_EQ(JPGR.is_valid, false);

  // Valid Create
  PGR.player_email = "fake_player@gmail.com";
  PGR.game_id = GD.game_id;
  PGR.game_parameter1_value = 10;
  PGR.game_parameter2_value = 5;
  PGR.game_parameter3_value = 100;
  PGR.game_parameter4_value = 20;
  PGR = DB.add_player_rating(PGR);
  ASSERT_EQ(PGR.is_valid, true);

  // Valid Read
  PGR = DB.get_player_game_rating("fake_player@gmail.com", GD.game_id);
  ASSERT_EQ(PGR.is_valid, true);
  ASSERT_EQ(PGR.player_email, "fake_player@gmail.com");
  ASSERT_EQ(PGR.game_id, GD.game_id);
  ASSERT_EQ(PGR.game_parameter1_value, 10);
  ASSERT_EQ(PGR.game_parameter2_value, 5);
  ASSERT_EQ(PGR.game_parameter3_value, 100);
  ASSERT_EQ(PGR.game_parameter4_value, 20);

  // Valid Read
  JPGR = DB.get_joined_player_game_rating("fake_player@gmail.com", GD.game_id);
  ASSERT_EQ(JPGR.is_valid, true);
  ASSERT_EQ(JPGR.player_email, "fake_player@gmail.com");
  ASSERT_EQ(JPGR.game_id, GD.game_id);
  ASSERT_EQ(JPGR.game_parameter1_name, "wins");
  ASSERT_FLOAT_EQ(JPGR.game_parameter1_weight, 1.0);
  ASSERT_EQ(JPGR.game_parameter2_name, "defeats");
  ASSERT_FLOAT_EQ(JPGR.game_parameter2_weight, -0.7);
  ASSERT_EQ(JPGR.game_parameter3_name, "kills");
  ASSERT_FLOAT_EQ(JPGR.game_parameter3_weight, 0.8);
  ASSERT_EQ(JPGR.game_parameter4_name, "assists");
  ASSERT_FLOAT_EQ(JPGR.game_parameter4_weight, 0.1);
  ASSERT_EQ(JPGR.game_parameter1_value, 10);
  ASSERT_EQ(JPGR.game_parameter2_value, 5);
  ASSERT_EQ(JPGR.game_parameter3_value, 100);
  ASSERT_EQ(JPGR.game_parameter4_value, 20);

  Player_Game_Ratings update_details;
  update_details.player_email = "fake_player@gmail.com";
  update_details.game_id = GD.game_id;
  update_details.game_parameter1_value = 9;

  Player_Game_Ratings updated_rating;

  // Invalid Update, player doesn't exist
  updated_rating = DB.update_player_rating("NONEXISTENT_PLAYER", GD.game_id, update_details);
  ASSERT_EQ(updated_rating.is_valid, false);

  // Invalid Update, game doesn't exist
  updated_rating = DB.update_player_rating("fake_player@gmail.com", -1, update_details);
  ASSERT_EQ(updated_rating.is_valid, false);

  // Valid Update
  updated_rating = DB.update_player_rating("fake_player@gmail.com", GD.game_id, update_details);
  ASSERT_EQ(updated_rating.is_valid, true);
  ASSERT_EQ(updated_rating.game_parameter1_value, 9);

  // Valid delete
  PGR = DB.remove_player_rating("fake_player@gmail.com", GD.game_id);
  ASSERT_EQ(PGR.is_valid, true);

  // Invalid delete
  PGR = DB.remove_player_rating("fake_player@gmail.com", GD.game_id);
  ASSERT_EQ(PGR.is_valid, false);

  // Get_all tests
  std::vector<Joined_Player_Game_Ratings> ratings;
  ratings = DB.get_all_player_game_ratings_for_game(-1);
  ASSERT_EQ(ratings.size(), 0);

  Player player1;
  player1.player_email = "player1@gmail.com";
  player1 = DB.add_player(player1);
  ASSERT_EQ(player1.is_valid, true);

  Player player2;
  player2.player_email = "player2@gmail.com";
  player2 = DB.add_player(player2);
  ASSERT_EQ(player2.is_valid, true);

  Developer developer1;
  developer1.developer_email = "developer1@dev.com";
  developer1.developer_password = "some_password";
  developer1 = DB.add_developer(developer1);
  ASSERT_EQ(developer1.is_valid, true);

  Developer developer2;
  developer2.developer_email = "developer2@dev.com";
  developer2.developer_password = "some_password";
  developer2 = DB.add_developer(developer2);
  ASSERT_EQ(developer2.is_valid, true);

  Game_Details Game1;
  Game1.game_name = "Game1";
  Game1.developer_email = "developer1@dev.com";
  Game1.game_parameter1_name = "wins";
  Game1.game_parameter1_weight = 1.0;
  Game1 = DB.add_game_details(Game1);
  ASSERT_EQ(Game1.is_valid, true);

  Game_Details Game2;
  Game2.game_name = "Game2";
  Game2.developer_email = "developer2@dev.com";
  Game2.game_parameter1_name = "kills";
  Game2.game_parameter1_weight = 0.9;
  Game2 = DB.add_game_details(Game2);
  ASSERT_EQ(Game2.is_valid, true);

  Player_Game_Ratings PGR1;
  PGR1.player_email = "player1@gmail.com";
  PGR1.game_id = Game1.game_id;
  PGR1.game_parameter1_value = 10;
  PGR1 = DB.add_player_rating(PGR1);
  ASSERT_EQ(PGR1.is_valid, true);

  Player_Game_Ratings PGR2;
  PGR2.player_email = "player1@gmail.com";
  PGR2.game_id = Game2.game_id;
  PGR2.game_parameter1_value = 500;
  PGR2 = DB.add_player_rating(PGR2);
  ASSERT_EQ(PGR2.is_valid, true);

  Player_Game_Ratings PGR3;
  PGR3.player_email = "player2@gmail.com";
  PGR3.game_id = Game2.game_id;
  PGR3.game_parameter1_value = 100;
  PGR3 = DB.add_player_rating(PGR3);
  ASSERT_EQ(PGR3.is_valid, true);

  ratings = DB.get_all_player_game_ratings_for_game(Game1.game_id);
  ASSERT_EQ(ratings.size(), 1);

  ratings = DB.get_all_player_game_ratings_for_game(Game2.game_id);
  ASSERT_EQ(ratings.size(), 2);
}
