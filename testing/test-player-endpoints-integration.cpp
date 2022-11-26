// "Copyright 2022 RaisingCanesFanClub"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "api-endpoints/api-endpoints-lib.h"
#include "main/db-service.h"
#include "authentication/auth-service.h"

using ::testing::_;
using ::testing::Return;

class D {
 public:
    Developer developer;
    std::string token;
};

class GD {
 public:
    Game_Details game;
    int game_id;
    D *developer;
};

class PGR {
 public:
    Player_Game_Ratings pgr;
    GD *game;
};

class PlayerTestIntegrationFixture : public testing::Test {
 public:
    static DBService DB;
    static AuthService auth;

    static APIEndPoints api;
    static D dev1, dev2;
    static GD game1, game2, game3;
    static Player p1, p2;
    static PGR pgr1, pgr2, pgr3, pgr4, pgr5, pgr_invalid1, pgr_invalid2;

    crow::request req;
    crow::response res;
    crow::json::wvalue body, return_body;

    virtual void SetUp() {
        DB.tearDownDatabase();
        DB.setUpDatabase();
    }

    virtual void TearDown() {
        DB.tearDownDatabase();
    }

    static void SetUpTestCase() {
        DB = DBService("tcp://127.0.0.1:3306", "dbuser", "123", "test_matchmaking_api_db");
        auth = AuthService();
        api = APIEndPoints(&DB, &auth);

        // Adding developers
        dev1.developer.developer_email = "dev1@gmail.com";
        dev1.developer.developer_password = "some_password1";

        dev2.developer.developer_email = "dev2@gmail.com";
        dev2.developer.developer_password = "some_password2";

        // Adding games
        game1.developer = &dev1;
        game1.game.developer_email = game1.developer->developer.developer_email;
        game1.game.game_name = "Skip-Bo";
        game1.game.category = "Cards";
        game1.game.game_parameter1_name = "stacks";
        game1.game.game_parameter2_name = "cardsLeft";
        game1.game.game_parameter3_name = "stacksCleared";
        game1.game.game_parameter4_name = "cardsHeld";
        game1.game.game_parameter1_weight = 2.5;
        game1.game.game_parameter2_weight = 3.7;
        game1.game.game_parameter3_weight = 1.8;
        game1.game.game_parameter4_weight = 0.7;
        game1.game.teams_per_match = 2;
        game1.game.players_per_team = 1;

        game2.developer = &dev2;
        game2.game.developer_email = game2.developer->developer.developer_email;
        game2.game.game_name = "Speed";
        game2.game.category = "Cards";
        game2.game.game_parameter1_name = "timeFinished";
        game2.game.game_parameter2_name = "cardsLeft";
        game2.game.game_parameter3_name = "cardsStacked";
        game2.game.game_parameter4_name = "cardsUsed";
        game2.game.game_parameter1_weight = 1.25;
        game2.game.game_parameter2_weight = 3.75;
        game2.game.game_parameter3_weight = 1.4;
        game2.game.game_parameter4_weight = 0.63;
        game2.game.teams_per_match = 2;
        game2.game.players_per_team = 1;

        game3.developer = &dev1;
        game3.game.developer_email = game3.developer->developer.developer_email;
        game3.game.game_name = "War";
        game3.game.category = "Cards";
        game3.game.game_parameter1_name = "cardsGainedFromOpponent";
        game3.game.game_parameter2_name = "cardsLostToOpponent";
        game3.game.game_parameter3_name = "cardsHeld";
        game3.game.game_parameter4_name = "warsWon";
        game3.game.game_parameter1_weight = 2.20;
        game3.game.game_parameter2_weight = 3.27;
        game3.game.game_parameter3_weight = 3.22;
        game3.game.game_parameter4_weight = 1.02;
        game3.game.teams_per_match = 2;
        game3.game.players_per_team = 1;

        // Adding players
        p1.player_email = "player1@gmail.com";
        p2.player_email = "player2@gmail.com";

        // Adding players' stats
        pgr1.pgr.player_email = p1.player_email;
        pgr1.pgr.game_parameter1_value = 10.1;
        pgr1.pgr.game_parameter2_value = 8.7;
        pgr1.pgr.game_parameter3_value = 4.62;
        pgr1.pgr.game_parameter4_value = 3.33;
        pgr1.game = &game1;

        pgr2.pgr.player_email = p2.player_email;
        pgr2.pgr.game_parameter1_value = 1.1;
        pgr2.pgr.game_parameter2_value = 2.7;
        pgr2.pgr.game_parameter3_value = 24.62;
        pgr2.pgr.game_parameter4_value = 13.33;
        pgr2.game = &game1;

        pgr3.pgr.player_email = "player3@gmail.com";
        pgr3.pgr.game_parameter1_value = 10.12;
        pgr3.pgr.game_parameter2_value = 38.7;
        pgr3.pgr.game_parameter3_value = 44.62;
        pgr3.pgr.game_parameter4_value = 31.33;
        pgr3.game = &game1;

        pgr4.pgr.player_email = "player5@gmail.com";
        pgr4.pgr.game_parameter1_value = 12.1;
        pgr4.pgr.game_parameter2_value = 18.7;
        pgr4.pgr.game_parameter3_value = 4.92;
        pgr4.pgr.game_parameter4_value = 23.33;
        pgr4.game = &game3;

        pgr5.pgr.player_email = "player6@gmail.com";
        pgr5.pgr.game_parameter1_value = 10.1;
        pgr5.pgr.game_parameter2_value = 28.7;
        pgr5.pgr.game_parameter3_value = 4.99;
        pgr5.pgr.game_parameter4_value = 13.33;
        pgr5.game = &game3;

        pgr_invalid1.pgr.player_email = "player.invalid1@gmail.com";
        pgr_invalid1.game = &game2;

        pgr_invalid2.pgr.player_email = "player.invalid2@gmail.com";
    }

    static void TearDownTestCase() {
        DB.tearDownDatabase();
    }

    /* Helper functions for setting up correct database entries (all valid) */
    void signUp(Developer d) {
        body = {{"developer_email", d.developer_email}, {"developer_password", d.developer_password}};
        req.body = body.dump();
        res = api.postSignUp(req);
        ASSERT_EQ(res.code, 200);
    }

    void logIn(Developer d, std::string *token) {
        body = {{"developer_email", d.developer_email}, {"developer_password", d.developer_password}};
        req.body = body.dump();
        res = api.postLogin(req);
        ASSERT_EQ(res.code, 200);
        *token = res.body.substr(res.body.find(":") + 2);
    }

    void createGame(Game_Details g, std::string token, int *game_id) {
        std::vector<std::string> parms;
        parms.push_back(g.game_parameter1_name);
        parms.push_back(g.game_parameter2_name);
        parms.push_back(g.game_parameter3_name);
        parms.push_back(g.game_parameter4_name);

        std::vector<float> weights;
        weights.push_back(g.game_parameter1_weight);
        weights.push_back(g.game_parameter2_weight);
        weights.push_back(g.game_parameter3_weight);
        weights.push_back(g.game_parameter4_weight);

        req.add_header("Authorization", token);
        body = {
            {"name", g.game_name},
            {"category", g.category},
            {"teams_per_match", g.teams_per_match},
            {"players_per_team", g.players_per_team}
        };
        body["parameters"] = parms;
        body["weights"] = weights;
        req.body = body.dump();
        std::pair <int, std::string> res_pair = api.postGames(req);
        ASSERT_EQ(res.code, res_pair.first);
        *game_id = std::stoi(res_pair.second);
    }

    void addPlayersStats(std::vector<Player_Game_Ratings> pgrs, std::string token, int game_id) {
        req.add_header("Authorization", token);
        body = {};
        for (Player_Game_Ratings pgr : pgrs) {
            body[pgr.player_email]["game_parameter1_value"] = pgr.game_parameter1_value;
            body[pgr.player_email]["game_parameter2_value"] = pgr.game_parameter2_value;
            body[pgr.player_email]["game_parameter3_value"] = pgr.game_parameter3_value;
            body[pgr.player_email]["game_parameter4_value"] = pgr.game_parameter4_value;
        }
        req.body = body.dump();
        res = api.addPlayersStats(req, game_id);
        ASSERT_EQ(res.code, 200);
    }

    void addPlayersStatsForGame(int game_id) {
        std::vector<Player_Game_Ratings> pgrs;
        if (game_id == pgr1.game->game_id) {
            pgrs.push_back(pgr1.pgr);
            pgrs.push_back(pgr2.pgr);
            pgrs.push_back(pgr3.pgr);
            addPlayersStats(pgrs, pgr1.game->developer->token, pgr1.game->game_id);
        } else if (game_id == pgr4.game->game_id) {
            pgrs.push_back(pgr4.pgr);
            pgrs.push_back(pgr5.pgr);
            addPlayersStats(pgrs, pgr4.game->developer->token, pgr4.game->game_id);
        }
    }
};

DBService PlayerTestIntegrationFixture::DB;
AuthService PlayerTestIntegrationFixture::auth;
APIEndPoints PlayerTestIntegrationFixture::api;

D PlayerTestIntegrationFixture::dev1, PlayerTestIntegrationFixture::dev2;
GD PlayerTestIntegrationFixture::game1, PlayerTestIntegrationFixture::game2, PlayerTestIntegrationFixture::game3;
Player PlayerTestIntegrationFixture::p1, PlayerTestIntegrationFixture::p2;
PGR PlayerTestIntegrationFixture::pgr1, PlayerTestIntegrationFixture::pgr2, PlayerTestIntegrationFixture::pgr3,
    PlayerTestIntegrationFixture::pgr4, PlayerTestIntegrationFixture::pgr5, PlayerTestIntegrationFixture::pgr_invalid1,
    PlayerTestIntegrationFixture::pgr_invalid2;

/*
    --------------------------------------------------------------------------------
    Endpoint: Get Game Players
    (Get all players for a requested game)
    --------------------------------------------------------------------------------
*/

/* Testing invalid developer authentication */
TEST_F(PlayerTestIntegrationFixture, GetGamePlayersAuthTest) {
    // Invalid header
    body = {};
    req.body = body.dump();
    res = api.getGamePlayers(req, game1.game_id);
    EXPECT_EQ(res.code, 401);
    EXPECT_EQ(res.body, "Invalid Header");

    signUp(dev2.developer);
    logIn(dev2.developer, &(dev2.token));
    // Developer does not own game
    req.add_header("Authorization", game2.developer->token);
    res = api.getGamePlayers(req, game1.game_id);
    std::string error = "Do not have access to game with game_id " + std::to_string(game1.game_id);
    EXPECT_EQ(res.code, 403);
    EXPECT_EQ(res.body, error);
}

/* No players for this game */
TEST_F(PlayerTestIntegrationFixture, GetGamePlayersNoPlayersTest) {
    signUp(game1.developer->developer);
    logIn(game1.developer->developer, &(game1.developer->token));
    createGame(game1.game, game1.developer->token, &game1.game_id);

    req.add_header("Authorization", game1.developer->token);
    res = api.getGamePlayers(req, game1.game_id);
    EXPECT_EQ(res.code, 204);
    std::string response = "No players found for game_id " + std::to_string(game1.game_id);
    EXPECT_EQ(res.body, response);
}

/* Developer owns multiple games, each with players */
TEST_F(PlayerTestIntegrationFixture, GetGamePlayersDevOwnsMultipleGamesTest) {
    signUp(game1.developer->developer);
    logIn(game1.developer->developer, &(game1.developer->token));
    createGame(game1.game, game1.developer->token, &game1.game_id);
    createGame(game3.game, game1.developer->token, &game3.game_id);
    addPlayersStatsForGame(game1.game_id);
    addPlayersStatsForGame(game3.game_id);

    req.add_header("Authorization", game1.developer->token);
    return_body = {
        {pgr1.pgr.player_email, {
            {"game_id", game1.game_id},
            {game1.game.game_parameter1_name, pgr1.pgr.game_parameter1_value},
            {game1.game.game_parameter2_name, pgr1.pgr.game_parameter2_value},
            {game1.game.game_parameter3_name, pgr1.pgr.game_parameter3_value},
            {game1.game.game_parameter4_name, pgr1.pgr.game_parameter4_value},
        }},
        {pgr2.pgr.player_email, {
            {"game_id", game1.game_id},
            {game1.game.game_parameter1_name, pgr2.pgr.game_parameter1_value},
            {game1.game.game_parameter2_name, pgr2.pgr.game_parameter2_value},
            {game1.game.game_parameter3_name, pgr2.pgr.game_parameter3_value},
            {game1.game.game_parameter4_name, pgr2.pgr.game_parameter4_value},
        }},
        {pgr3.pgr.player_email, {
            {"game_id", game1.game_id},
            {game1.game.game_parameter1_name, pgr3.pgr.game_parameter1_value},
            {game1.game.game_parameter2_name, pgr3.pgr.game_parameter2_value},
            {game1.game.game_parameter3_name, pgr3.pgr.game_parameter3_value},
            {game1.game.game_parameter4_name, pgr3.pgr.game_parameter4_value},
        }}
    };
    res = api.getGamePlayers(req, pgr1.game->game_id);
    EXPECT_EQ(res.code, 200);
    EXPECT_EQ(res.body, return_body.dump());
}

/*
    --------------------------------------------------------------------------------
    Endpoint: Add Player Stats
    (Add player stats for a game)
    --------------------------------------------------------------------------------
*/

/* Testing invalid developer authentication */
TEST_F(PlayerTestIntegrationFixture, AddPlayerStatsAuthTest) {
    // Invalid header
    body = {};
    req.body = body.dump();
    res = api.addPlayersStats(req, game1.game_id);
    EXPECT_EQ(res.code, 401);
    EXPECT_EQ(res.body, "Invalid Header");

    signUp(dev2.developer);
    logIn(dev2.developer, &(dev2.token));
    // Developer does not own game
    req.add_header("Authorization", game2.developer->token);
    res = api.addPlayersStats(req, game1.game_id);
    std::string error = "Do not have access to game with game_id " + std::to_string(game1.game_id);
    EXPECT_EQ(res.code, 403);
    EXPECT_EQ(res.body, error);
}

/* No players provided -- Therefore cannot add any stats */
TEST_F(PlayerTestIntegrationFixture, AddPlayerStatsNoPlayersTest) {
    signUp(game1.developer->developer);
    logIn(game1.developer->developer, &(game1.developer->token));
    createGame(game1.game, game1.developer->token, &game1.game_id);
    body = {};
    req.body = body.dump();
    res = api.addPlayersStats(req, game1.game_id);
    EXPECT_EQ(res.code, 200);
    EXPECT_EQ(res.body, "No player stats added due to empty request");
}

/* Adding player stats for already existing players */
TEST_F(PlayerTestIntegrationFixture, AddPlayerStatsPlayersExistTest) {
    signUp(pgr1.game->developer->developer);
    logIn(pgr1.game->developer->developer, &(pgr1.game->developer->token));
    createGame(pgr1.game->game, pgr1.game->developer->token, &pgr1.game->game_id);
    DB.add_player(p1);
    DB.add_player(p2);

    body = {
        {pgr1.pgr.player_email, {
            {"game_parameter1_value", pgr1.pgr.game_parameter1_value},
            {"game_parameter2_value", pgr1.pgr.game_parameter2_value},
            {"game_parameter3_value", pgr1.pgr.game_parameter3_value},
            {"game_parameter4_value", pgr1.pgr.game_parameter4_value},
        }},
        {pgr2.pgr.player_email, {
            {"game_parameter1_value", pgr2.pgr.game_parameter1_value},
            {"game_parameter2_value", pgr2.pgr.game_parameter2_value},
            {"game_parameter3_value", pgr2.pgr.game_parameter3_value},
            {"game_parameter4_value", pgr2.pgr.game_parameter4_value},
        }}
    };
    req.body = body.dump();
    res = api.addPlayersStats(req, pgr1.game->game_id);
    EXPECT_EQ(res.code, 200);
    EXPECT_EQ(res.body, "Player stats were added");
}

/* Adding player stats for player which doesn't exist yet */
TEST_F(PlayerTestIntegrationFixture, AddPlayerStatsPlayerDoesntExistTest) {
    signUp(pgr1.game->developer->developer);
    logIn(pgr1.game->developer->developer, &(pgr1.game->developer->token));
    createGame(pgr1.game->game, pgr1.game->developer->token, &pgr1.game->game_id);

    body = {
        {pgr1.pgr.player_email, {
            {"game_parameter1_value", pgr1.pgr.game_parameter1_value},
            {"game_parameter2_value", pgr1.pgr.game_parameter2_value},
            {"game_parameter3_value", pgr1.pgr.game_parameter3_value},
            {"game_parameter4_value", pgr1.pgr.game_parameter4_value},
        }}
    };
    req.body = body.dump();
    res = api.addPlayersStats(req, pgr1.game->game_id);
    EXPECT_EQ(res.code, 200);
    EXPECT_EQ(res.body, "Player stats were added");
}

/*
    --------------------------------------------------------------------------------
    Endpoint: Get Players Stats
    (Get specific players' stats for a game)
    --------------------------------------------------------------------------------
*/

/* Testing invalid developer authentication */
TEST_F(PlayerTestIntegrationFixture, GetPlayersStatsAuthTest) {
    // Invalid header
    body = {};
    req.body = body.dump();
    res = api.getPlayerStats(req, game1.game_id, "");
    EXPECT_EQ(res.code, 401);
    EXPECT_EQ(res.body, "Invalid Header");

    signUp(dev2.developer);
    logIn(dev2.developer, &(dev2.token));
    // Developer does not own game
    req.add_header("Authorization", game2.developer->token);
    res = api.getPlayerStats(req, game1.game_id, "");
    std::string error = "Do not have access to game with game_id " + std::to_string(game1.game_id);
    EXPECT_EQ(res.code, 403);
    EXPECT_EQ(res.body, error);
}

/* Player game rating is missing for multiple players */
TEST_F(PlayerTestIntegrationFixture, GetPlayersStatsMissingStatsTest) {
    signUp(pgr1.game->developer->developer);
    logIn(pgr1.game->developer->developer, &(pgr1.game->developer->token));
    createGame(pgr1.game->game, pgr1.game->developer->token, &pgr1.game->game_id);
    addPlayersStatsForGame(pgr1.game->game_id);

    res = api.getPlayerStats(req, pgr1.game->game_id, pgr_invalid1.pgr.player_email);
    EXPECT_EQ(res.code, 404);
    // Players can be listed in any order as long as both players are stated as invalid
    std::string missing_players1 = "The given player does not have ratings for this game";
    EXPECT_TRUE(res.body == missing_players1);
}

/* Player game ratings are valid */
TEST_F(PlayerTestIntegrationFixture, GetPlayersStatsValidTest) {
    signUp(pgr1.game->developer->developer);
    logIn(pgr1.game->developer->developer, &(pgr1.game->developer->token));
    createGame(pgr1.game->game, pgr1.game->developer->token, &pgr1.game->game_id);
    addPlayersStatsForGame(pgr1.game->game_id);

    crow::json::wvalue expected_return = {
        {"game_parameter1_value", pgr1.pgr.game_parameter1_value},
        {"game_parameter2_value", pgr1.pgr.game_parameter2_value},
        {"game_parameter3_value", pgr1.pgr.game_parameter3_value},
        {"game_parameter4_value", pgr1.pgr.game_parameter4_value},
    };

    res = api.getPlayerStats(req, pgr1.game->game_id, pgr1.pgr.player_email);
    EXPECT_EQ(res.code, 200);
}

/*
    --------------------------------------------------------------------------------
    Endpoint: Delete Players Stats
    (Delete requested players' stats for a given game)
    --------------------------------------------------------------------------------
*/

/* Testing invalid developer authentication */
TEST_F(PlayerTestIntegrationFixture, DeletePlayersStatsAuthTest) {
    // Invalid header
    body = {};
    req.body = body.dump();
    res = api.deletePlayerStats(req, game1.game_id, "");
    EXPECT_EQ(res.code, 401);
    EXPECT_EQ(res.body, "Invalid Header");

    signUp(dev2.developer);
    logIn(dev2.developer, &(dev2.token));
    // Developer does not own game
    req.add_header("Authorization", game2.developer->token);
    res = api.deletePlayerStats(req, game1.game_id, "");
    std::string error = "Do not have access to game with game_id " + std::to_string(game1.game_id);
    EXPECT_EQ(res.code, 403);
    EXPECT_EQ(res.body, error);
}

/* Attempting to delete multiple players that don't exist */
TEST_F(PlayerTestIntegrationFixture, DeletePlayersStatsMissingPlayersTest) {
    signUp(pgr1.game->developer->developer);
    logIn(pgr1.game->developer->developer, &(pgr1.game->developer->token));
    createGame(pgr1.game->game, pgr1.game->developer->token, &pgr1.game->game_id);
    addPlayersStatsForGame(pgr1.game->game_id);

    res = api.deletePlayerStats(req, pgr1.game->game_id, pgr_invalid1.pgr.player_email);
    EXPECT_EQ(res.code, 404);
    // Players can be listed in any order as long as both players are stated as invalid
    std::string missing_players1 = "The given player does not have ratings for this game";
    EXPECT_TRUE(res.body == missing_players1);
}

/* Player game ratings are valid */
TEST_F(PlayerTestIntegrationFixture, DeletePlayersStatsMissingStatsTest) {
    signUp(pgr1.game->developer->developer);
    logIn(pgr1.game->developer->developer, &(pgr1.game->developer->token));
    createGame(pgr1.game->game, pgr1.game->developer->token, &pgr1.game->game_id);
    addPlayersStatsForGame(pgr1.game->game_id);

    res = api.deletePlayerStats(req, pgr1.game->game_id, pgr1.pgr.player_email);
    EXPECT_EQ(res.code, 200);
    EXPECT_EQ(res.body, "Player stats were removed");
}

/*
    --------------------------------------------------------------------------------
    Endpoint: Update Players Stats
    (Update requested players' stats for a given game)
    --------------------------------------------------------------------------------
*/

/* Testing invalid developer authentication */
TEST_F(PlayerTestIntegrationFixture, UpdatePlayersStatsAuthTest) {
    // Invalid header
    body = {};
    req.body = body.dump();
    res = api.updatePlayerStats(req, game1.game_id, "");
    EXPECT_EQ(res.code, 401);
    EXPECT_EQ(res.body, "Invalid Header");

    signUp(dev2.developer);
    logIn(dev2.developer, &(dev2.token));
    // Developer does not own game
    req.add_header("Authorization", game2.developer->token);
    res = api.updatePlayerStats(req, game1.game_id, "");
    std::string error = "Do not have access to game with game_id " + std::to_string(game1.game_id);
    EXPECT_EQ(res.code, 403);
    EXPECT_EQ(res.body, error);
}

/* Player rating does not exist */
TEST_F(PlayerTestIntegrationFixture, UpdatePlayersStatsMissingStatsTest) {
    signUp(pgr1.game->developer->developer);
    logIn(pgr1.game->developer->developer, &(pgr1.game->developer->token));
    createGame(pgr1.game->game, pgr1.game->developer->token, &pgr1.game->game_id);

    // Performing test
    body = {
        {"game_parameter1_value", pgr1.pgr.game_parameter1_value},
        {"game_parameter2_value", pgr1.pgr.game_parameter2_value},
        {"game_parameter3_value", pgr1.pgr.game_parameter3_value},
        {"game_parameter4_value", pgr1.pgr.game_parameter4_value}
    };
    req.body = body.dump();
    res = api.updatePlayerStats(req, game1.game_id, pgr_invalid1.pgr.player_email);
    EXPECT_EQ(res.code, 404);
    std::string missing_players1 = "The given player does not have ratings for this game";
    EXPECT_TRUE(res.body == missing_players1);
}

/* Not giving any stats to update */
TEST_F(PlayerTestIntegrationFixture, UpdatePlayersStatsEmptyBodyTest) {
    signUp(pgr1.game->developer->developer);
    logIn(pgr1.game->developer->developer, &(pgr1.game->developer->token));
    createGame(pgr1.game->game, pgr1.game->developer->token, &pgr1.game->game_id);
    addPlayersStatsForGame(pgr1.game->game_id);

    // Performing test
    body = {};
    req.body = body.dump();
    res = api.updatePlayerStats(req, game1.game_id, pgr1.pgr.player_email);
    EXPECT_EQ(res.code, 400);
    EXPECT_EQ(res.body, "Invalid request body");
}


/* Attempting to update players stats with invalid stats */
TEST_F(PlayerTestIntegrationFixture, UpdatePlayersStatsInvalidStatsTest) {
    signUp(pgr1.game->developer->developer);
    logIn(pgr1.game->developer->developer, &(pgr1.game->developer->token));
    createGame(pgr1.game->game, pgr1.game->developer->token, &pgr1.game->game_id);
    addPlayersStatsForGame(pgr1.game->game_id);

    // Performing test
    body = {
        {"game_parameter1_value", "abc"},
        {"game_parameter2_value", pgr1.pgr.game_parameter2_value},
        {"game_parameter3_value", pgr1.pgr.game_parameter3_value},
        {"game_parameter4_value", pgr1.pgr.game_parameter4_value}
    };
    req.body = body.dump();
    res = api.updatePlayerStats(req, game1.game_id, pgr1.pgr.player_email);
    EXPECT_EQ(res.code, 400);
    EXPECT_TRUE(res.body ==  "Invalid request body");
}

/* Valid update */
TEST_F(PlayerTestIntegrationFixture, UpdatePlayersStatsValidTest) {
    signUp(pgr1.game->developer->developer);
    logIn(pgr1.game->developer->developer, &(pgr1.game->developer->token));
    createGame(pgr1.game->game, pgr1.game->developer->token, &pgr1.game->game_id);
    addPlayersStatsForGame(pgr1.game->game_id);

    // Performing test
    body = {
        {"game_parameter1_value", pgr1.pgr.game_parameter1_value},
        {"game_parameter2_value", pgr1.pgr.game_parameter2_value},
        {"game_parameter3_value", pgr1.pgr.game_parameter3_value},
        {"game_parameter4_value", pgr1.pgr.game_parameter4_value}
    };
    req.body = body.dump();
    res = api.updatePlayerStats(req, game1.game_id, pgr1.pgr.player_email);
    EXPECT_EQ(res.code, 200);
    EXPECT_EQ(res.body, "Player stats were updated");
}