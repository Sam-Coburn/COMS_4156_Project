#include <gtest/gtest.h>
#include "main/db-service.h"

TEST(DBService, Player_Tests) {

  DBService DB = DBService();
  Player P;
  P.player_email = "fake_player@gmail.com";

  P = DB.add_player(P);
  EXPECT_EQ(P.is_valid, true);

  P = DB.get_player("fake_player@gmail.com");
  EXPECT_EQ(P.is_valid, true);
  EXPECT_EQ(P.player_email, "fake_player@gmail.com");
  
  
  P = DB.remove_player("fake_player@gmail.com");
  EXPECT_EQ(P.is_valid, true);

  P = DB.get_player("fake_player@gmail.com");
  EXPECT_EQ(P.is_valid, false);
}