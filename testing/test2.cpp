// "Copyright [year] <Copyright Owner>"

#include <gtest/gtest.h>
#include "main/db-service.h"

TEST(DBServiceTest, Player_Tests1) {
  DBService DB = DBService();
  Player P;
  P.player_email = "fake_player@gmail.com";

  P = DB.add_player(P);  // Valid create
  EXPECT_EQ(P.is_valid, true);

  P = DB.add_player(P);  // invalid create (duplicate)
  EXPECT_EQ(P.is_valid, false);

  P = DB.get_player("fake_player@gmail.com");  // Valid read
  EXPECT_EQ(P.is_valid, true);
  EXPECT_EQ(P.player_email, "fake_player@gmail.com");

  P = DB.remove_player("fake_player@gmail.com");  // Valid delete
  EXPECT_EQ(P.is_valid, true);

  P = DB.remove_player("fake_player@gmail.com");  // Invalid delete
  EXPECT_EQ(P.is_valid, false);

  P = DB.get_player("fake_player@gmail.com");  // Invalid read
  EXPECT_EQ(P.is_valid, false);
}

TEST(DBServiceTest, Developer_Tests1) {
  DBService DB = DBService();
  Developer D;
  D.developer_email = "fake_developer@dev.com";
  D.developer_password = "some_password";

  D = DB.add_developer(D);  // Valid create
  EXPECT_EQ(D.is_valid, true);

  D = DB.add_developer(D);  // Invalid create (duplicate)
  EXPECT_EQ(D.is_valid, false);

  D = DB.get_developer("fake_developer@dev.com");  // Valid read
  EXPECT_EQ(D.is_valid, true);
  EXPECT_EQ(D.developer_email, "fake_developer@dev.com");
  EXPECT_EQ(D.developer_password, "some_password");

  D = DB.remove_developer("fake_developer@dev.com");  // Valid delete
  EXPECT_EQ(D.is_valid, true);

  D = DB.remove_developer("fake_developer@dev.com");  // Invalid delete
  EXPECT_EQ(D.is_valid, false);

  D = DB.get_developer("fake_developer@dev.com");  // Invalid read
  EXPECT_EQ(D.is_valid, false);
}

TEST(DBServiceTest, Game_Details_Tests1) {
  DBService DB = DBService();
  Game_Details GD;
  GD.game_name = "fake game";
  GD.developer_email = "fake_developer@dev.com";

  GD = DB.add_game_details(GD);  // Invalid create (nonexistent developer)
  EXPECT_EQ(GD.is_valid, false);

  Developer D;
  D.developer_email = "fake_developer@dev.com";
  D.developer_password = "some_password";

  D = DB.add_developer(D);  // Valid create
  EXPECT_EQ(D.is_valid, true);

  GD.game_name = "fake game";
  GD.developer_email = "fake_developer@dev.com";
  GD = DB.add_game_details(GD);  // Valid Create
  EXPECT_EQ(GD.is_valid, true);
  // *NOTE* Should we make game names unique per developer?

  GD = DB.get_game_details(GD.game_id);  // Valid Read
  EXPECT_EQ(GD.developer_email, "fake_developer@dev.com");
  EXPECT_EQ(GD.game_name, "fake game");
  EXPECT_EQ(GD.is_valid, true);

  GD = DB.remove_game_details(GD.game_id);  // Valid delete
  EXPECT_EQ(GD.is_valid, true);

  GD = DB.get_game_details(-1);  // Invalid Read
  EXPECT_EQ(GD.is_valid, false);

  GD = DB.remove_game_details(-1);  // Invalid delete
  EXPECT_EQ(GD.is_valid, false);

  D = DB.remove_developer("fake_developer@dev.com");  // Valid delete
  EXPECT_EQ(D.is_valid, true);
}

TEST(DBServiceTest, Player_Game_Ratings_Tests1) {
  DBService DB = DBService();

  Player_Game_Ratings PGR;
  PGR.player_email = "fake_player@gmail.com";
  PGR.game_id = -1;
  PGR = DB.add_player_rating(PGR);  // Invalid Create
  EXPECT_EQ(PGR.is_valid, false);

  Player P;
  P.player_email = "fake_player@gmail.com";
  P = DB.add_player(P);  // Valid create
  EXPECT_EQ(P.is_valid, true);

  PGR.player_email = "fake_player@gmail.com";
  PGR.game_id = -1;
  PGR = DB.add_player_rating(PGR);  // Invalid Create (nonexistent game)
  EXPECT_EQ(PGR.is_valid, false);

  Developer D;
  D.developer_email = "fake_developer@dev.com";
  D.developer_password = "some_password";
  D = DB.add_developer(D);  // Valid create
  EXPECT_EQ(D.is_valid, true);

  Game_Details GD;
  GD.game_name = "fake game";
  GD.developer_email = "fake_developer@dev.com";
  GD = DB.add_game_details(GD);  // Valid Create
  EXPECT_EQ(GD.is_valid, true);

  PGR.player_email = "fake_player@gmail.com";
  PGR.game_id = GD.game_id;
  PGR = DB.add_player_rating(PGR);  // Valid Create
  EXPECT_EQ(PGR.is_valid, true);

  PGR = DB.get_player_game_rating("fake_player@gmail.com",
  GD.game_id);  // Valid Read
  EXPECT_EQ(PGR.is_valid, true);

  PGR = DB.remove_player_rating("fake_player@gmail.com",
  GD.game_id);  // Valid delete
  EXPECT_EQ(PGR.is_valid, true);

  PGR = DB.remove_player_rating("fake_player@gmail.com",
  GD.game_id);  // invalid delete
  EXPECT_EQ(PGR.is_valid, false);

  PGR = DB.get_player_game_rating("fake_player@gmail.com",
  GD.game_id);  // Invalid read
  EXPECT_EQ(PGR.is_valid, false);

  P = DB.remove_player("fake_player@gmail.com");  // Valid delete
  EXPECT_EQ(P.is_valid, true);

  GD = DB.remove_game_details(GD.game_id);  // Valid delete
  EXPECT_EQ(GD.is_valid, true);

  D = DB.remove_developer("fake_developer@dev.com");  // Valid delete
  EXPECT_EQ(D.is_valid, true);
}
