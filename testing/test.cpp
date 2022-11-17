// "Copyright [year] <Copyright Owner>"

#include <gtest/gtest.h>
#include "main/db-service.h"

class DBServiceTestFixture: public testing::Test
{
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
}

TEST_F(DBServiceTestFixture, Developer_Tests) {
  Developer D;
  D.developer_email = "fake_developer@dev.com";
  D.developer_password = "some_password";

  D = DB.add_developer(D);  // Valid create
  ASSERT_EQ(D.is_valid, true);

  D = DB.add_developer(D);  // Invalid create (duplicate)
  ASSERT_EQ(D.is_valid, false);

  D = DB.get_developer("fake_developer@dev.com");  // Valid read
  ASSERT_EQ(D.is_valid, true);
  ASSERT_EQ(D.developer_email, "fake_developer@dev.com");
  ASSERT_EQ(D.developer_password, "some_password");

  D = DB.remove_developer("fake_developer@dev.com");  // Valid delete
  ASSERT_EQ(D.is_valid, true);

  D = DB.remove_developer("fake_developer@dev.com");  // Invalid delete
  ASSERT_EQ(D.is_valid, false);

  D = DB.get_developer("fake_developer@dev.com");  // Invalid read
  ASSERT_EQ(D.is_valid, false);
}

TEST_F(DBServiceTestFixture, Game_Details_Tests) {
  Game_Details GD;
  GD.game_name = "fake game";
  GD.developer_email = "fake_developer@dev.com";

  GD = DB.add_game_details(GD);  // Invalid create (nonexistent developer)
  ASSERT_EQ(GD.is_valid, false);

  Developer D;
  D.developer_email = "fake_developer@dev.com";
  D.developer_password = "some_password";

  D = DB.add_developer(D);  // Valid create
  ASSERT_EQ(D.is_valid, true);

  GD.game_name = "fake game";
  GD.developer_email = "fake_developer@dev.com";
  GD = DB.add_game_details(GD);  // Valid Create
  ASSERT_EQ(GD.is_valid, true);
  // *NOTE* Should we make game names unique per developer?

  GD = DB.get_game_details(GD.game_id);  // Valid Read
  ASSERT_EQ(GD.developer_email, "fake_developer@dev.com");
  ASSERT_EQ(GD.game_name, "fake game");
  ASSERT_EQ(GD.is_valid, true);

  GD = DB.remove_game_details(GD.game_id);  // Valid delete
  ASSERT_EQ(GD.is_valid, true);

  GD = DB.get_game_details(-1);  // Invalid Read
  ASSERT_EQ(GD.is_valid, false);

  GD = DB.remove_game_details(-1);  // Invalid delete
  ASSERT_EQ(GD.is_valid, false);

  D = DB.remove_developer("fake_developer@dev.com");  // Valid delete
  ASSERT_EQ(D.is_valid, true);
}

TEST_F(DBServiceTestFixture, Player_Game_Ratings_Tests) {

  Player_Game_Ratings PGR;
  PGR.player_email = "fake_player@gmail.com";
  PGR.game_id = -1;
  PGR = DB.add_player_rating(PGR);  // Invalid Create
  ASSERT_EQ(PGR.is_valid, false);

  Player P;
  P.player_email = "fake_player@gmail.com";
  P = DB.add_player(P);  // Valid create
  ASSERT_EQ(P.is_valid, true);

  PGR.player_email = "fake_player@gmail.com";
  PGR.game_id = -1;
  PGR = DB.add_player_rating(PGR);  // Invalid Create (nonexistent game)
  ASSERT_EQ(PGR.is_valid, false);

  Developer D;
  D.developer_email = "fake_developer@dev.com";
  D.developer_password = "some_password";
  D = DB.add_developer(D);  // Valid create
  ASSERT_EQ(D.is_valid, true);

  Game_Details GD;
  GD.game_name = "fake game";
  GD.developer_email = "fake_developer@dev.com";
  GD = DB.add_game_details(GD);  // Valid Create
  ASSERT_EQ(GD.is_valid, true);

  PGR.player_email = "fake_player@gmail.com";
  PGR.game_id = GD.game_id;
  PGR = DB.add_player_rating(PGR);  // Valid Create
  ASSERT_EQ(PGR.is_valid, true);

  PGR = DB.get_player_game_rating("fake_player@gmail.com",
  GD.game_id);  // Valid Read
  ASSERT_EQ(PGR.is_valid, true);

  PGR = DB.remove_player_rating("fake_player@gmail.com",
  GD.game_id);  // Valid delete
  ASSERT_EQ(PGR.is_valid, true);

  PGR = DB.remove_player_rating("fake_player@gmail.com",
  GD.game_id);  // invalid delete
  ASSERT_EQ(PGR.is_valid, false);

  PGR = DB.get_player_game_rating("fake_player@gmail.com",
  GD.game_id);  // Invalid read
  ASSERT_EQ(PGR.is_valid, false);

  P = DB.remove_player("fake_player@gmail.com");  // Valid delete
  ASSERT_EQ(P.is_valid, true);

  GD = DB.remove_game_details(GD.game_id);  // Valid delete
  ASSERT_EQ(GD.is_valid, true);

  D = DB.remove_developer("fake_developer@dev.com");  // Valid delete
  ASSERT_EQ(D.is_valid, true);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    google::InitGoogleLogging(argv[0]);
    return RUN_ALL_TESTS();
}
