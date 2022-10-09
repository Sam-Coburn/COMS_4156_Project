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

std::vector<Game_Details> get_all_games();

Developer add_developer(Developer D);
Player add_player(Player P);
Game_Details add_game_details(Game_Details GD);
Player_Game_Ratings add_player_rating(Player_Game_Ratings PGR);

std::vector<Joined_Developer_Games> join_developer_games(std::string developer_email);
std::vector<Joined_Player_Game_Ratings> join_player_game_ratings_for_game(int game_id);