#include "db-service.h"

#include <stdlib.h>
#include <iostream>

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

std::string hostname = "tcp://127.0.0.1:3306";
std::string username = "dbuser";
std::string password = "123";
std::string database = "matchmaking_api_db";

std::vector<Player> get_all_players(){
    std::cout << "Connecting to MYSQL to get all players" << std::endl;
    std::vector<Player> players;

    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;
        sql::ResultSet *res;
        Player p;

        driver = get_driver_instance();
        con = driver->connect(hostname, username, password);

        con->setSchema(database);

        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT * FROM Players");
        while (res->next()) {
            p.player_email = res->getString("player_email");
            std::cout << p.toString() << std::endl;
            players.push_back(p);
        }

        delete stmt;
        delete res;
        delete con;

        std::cout << "Successfully retrieved all players" << std::endl;
        return players;

    } catch (sql::SQLException &e) {
        std::cout << "Error getting all players" << std::endl;
        std::cout << "# ERR: SQLException in " << __FILE__;
        std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
        std::cout << "# ERR: " << e.what();
        std::cout << " (MySQL error code: " << e.getErrorCode();
        std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
        return players;
    }

}

std::vector<Developer> get_all_developers() {
    std::cout << "Connecting to MYSQL to get all developers" << std::endl;
    std::vector<Developer> developers;

    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;
        sql::ResultSet *res;
        Developer d;

        driver = get_driver_instance();
        con = driver->connect(hostname, username, password);

        con->setSchema(database);

        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT * FROM Developers");
        while (res->next()) {
            d.developer_email = res->getString("developer_email");
            d.developer_password = res->getString("developer_password");
            std::cout << d.toString() << std::endl;
            developers.push_back(d);
        }

        delete stmt;
        delete res;
        delete con;

        std::cout << "Successfully retrieved all developers" << std::endl;
        return developers;

    } catch (sql::SQLException &e) {
        std::cout << "Error getting all developers" << std::endl;
        std::cout << "# ERR: SQLException in " << __FILE__;
        std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
        std::cout << "# ERR: " << e.what();
        std::cout << " (MySQL error code: " << e.getErrorCode();
        std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
        return developers;
    }
}

std::vector<Game_Details> get_all_games() {
    std::cout << "Connecting to MYSQL to get all game details" << std::endl;
    std::vector<Game_Details> game_details;

    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;
        sql::ResultSet *res;
        Game_Details gd;

        driver = get_driver_instance();
        con = driver->connect(hostname, username, password);

        con->setSchema(database);

        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT * FROM Game_Details");
        while (res->next()) {
            gd.game_id = std::stoi(res->getString("game_id"));
            gd.developer_email = res->getString("developer_email");
            gd.game_name = res->getString("game_name");
            gd.game_parameter1_name = res->getString("game_parameter1_name");
            gd.game_parameter1_weight = std::stof(res->getString("game_parameter1_weight"));

            gd.game_parameter2_name = res->getString("game_parameter2_name");
            gd.game_parameter2_weight = std::stof(res->getString("game_parameter2_weight"));

            gd.game_parameter3_name = res->getString("game_parameter3_name");
            gd.game_parameter3_weight = std::stof(res->getString("game_parameter3_weight"));

            gd.game_parameter4_name = res->getString("game_parameter4_name");
            gd.game_parameter4_weight = std::stof(res->getString("game_parameter4_weight"));

            gd.category = res->getString("category");
            gd.players_per_team = std::stoi(res->getString("players_per_team"));
            gd.teams_per_match = std::stoi(res->getString("teams_per_match"));
            std::cout << gd.toString() << std::endl;
            game_details.push_back(gd);
        }

        delete stmt;
        delete res;
        delete con;

        std::cout << "Successfully retrieved all game details" << std::endl;
        return game_details;

    } catch (sql::SQLException &e) {
        std::cout << "Error getting all game details" << std::endl;
        std::cout << "# ERR: SQLException in " << __FILE__;
        std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
        std::cout << "# ERR: " << e.what();
        std::cout << " (MySQL error code: " << e.getErrorCode();
        std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
        return game_details;
    }
}

Developer add_developer(Developer D);
Player add_player(Player P);
Game_Details add_game_details(Game_Details GD);
Player_Game_Ratings add_player_rating(Player_Game_Ratings PGR);

Developer remove_developer(std::string developer_email);
Player remove_player(std::string player_email);
Game_Details remove_game_details(int game_id);
Player_Game_Ratings remove_player_rating(std::string player_email, int game_id);

Developer update_developer(Developer D);
Player update_player(Player P);
Game_Details update_game_details(Game_Details GD);
Player_Game_Ratings update_player_rating(Player_Game_Ratings PGR);

std::vector<Game_Details> get_all_games_for_developer(std::string developer_email);

std::vector<Joined_Player_Game_Ratings> join_player_game_ratings_for_game(int game_id) {
    std::cout << "Connecting to MYSQL to get all player ratings for a game" << std::endl;
    std::vector<Joined_Player_Game_Ratings> PGR;

    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::PreparedStatement  *prep_stmt;
        sql::ResultSet *res;
        Joined_Player_Game_Ratings pgr;

        driver = get_driver_instance();
        con = driver->connect(hostname, username, password);

        con->setSchema(database);

        prep_stmt = con->prepareStatement("SELECT PGR.*, GD.* FROM Players P JOIN Player_Game_Ratings PGR ON P.player_email = PGR.player_email JOIN Game_Details GD ON PGR.game_id = GD.game_id WHERE GD.game_id = ?;");

        prep_stmt->setInt(1, game_id);

        res = prep_stmt->executeQuery();
        while (res->next()) {
            pgr.player_email = res->getString("player_email");
            pgr.game_id = std::stoi(res->getString("game_id"));
            pgr.developer_email = res->getString("developer_email");
            pgr.game_name = res->getString("game_name");
            pgr.game_parameter1_name = res->getString("game_parameter1_name");
            pgr.game_parameter1_weight = std::stof(res->getString("game_parameter1_weight"));
            pgr.game_parameter1_value = std::stoi(res->getString("game_parameter1_value"));

            pgr.game_parameter2_name = res->getString("game_parameter2_name");
            pgr.game_parameter2_weight = std::stof(res->getString("game_parameter2_weight"));
            pgr.game_parameter2_value = std::stoi(res->getString("game_parameter2_value"));

            pgr.game_parameter3_name = res->getString("game_parameter3_name");
            pgr.game_parameter3_weight = std::stof(res->getString("game_parameter3_weight"));
            pgr.game_parameter3_value = std::stoi(res->getString("game_parameter3_value"));

            pgr.game_parameter4_name = res->getString("game_parameter4_name");
            pgr.game_parameter4_weight = std::stof(res->getString("game_parameter4_weight"));
            pgr.game_parameter4_value = std::stoi(res->getString("game_parameter4_value"));

            pgr.category = res->getString("category");
            pgr.players_per_team = std::stoi(res->getString("players_per_team"));
            pgr.teams_per_match = std::stoi(res->getString("teams_per_match"));
            std::cout << pgr.toString() << std::endl;
            PGR.push_back(pgr);
        }

        delete prep_stmt;
        delete res;
        delete con;

        std::cout << "Successfully retrieved all player game ratings" << std::endl;
        return PGR;

    } catch (sql::SQLException &e) {
        std::cout << "Error getting all player game ratings" << std::endl;
        std::cout << "# ERR: SQLException in " << __FILE__;
        std::cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << std::endl;
        std::cout << "# ERR: " << e.what();
        std::cout << " (MySQL error code: " << e.getErrorCode();
        std::cout << ", SQLState: " << e.getSQLState() <<" )" << std::endl;
        return PGR;
    }
}