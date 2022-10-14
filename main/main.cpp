#include "crow/crow_all.h"
#include "db-service.h"


using namespace std;

int main(void)
{
	get_player("apex_player@gmail.com");
	cout << "\n";
	get_developer("developer@chess.com");
	cout << "\n";
	get_game_details(2);
	cout << "\n";
	get_player_game_rating("apex_player@gmail.com", 1);
	cout << "\n";
	get_joined_player_game_rating("apex_player@gmail.com", 1);
	cout << "\n";
	get_all_players();
	cout << "\n";
	get_all_developers();
	cout << "\n";
	get_all_games();
	cout << "\n";
	get_all_player_game_ratings_for_game(1);
	cout << "\n";
	get_all_games_for_developer("developer@chess.com");
	cout << "\n";
	
	crow::SimpleApp app; //define your crow application

	//define your endpoint at the root directory
	CROW_ROUTE(app, "/")([](){
		return "Hello world";
	});

	// Login doesn't do much since we are always expecting to get the email and password with every request
	CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::POST)([](const crow::request& req){
		crow::json::rvalue x = crow::json::load(req.body);
		std::string developer_email;
		std::string developer_password;
		Developer D;

		try {
			developer_email = x["developer_email"].s();
			developer_password = x["developer_password"].s();
			D = get_developer(developer_email);
			if(!D.is_valid || D.developer_password != developer_password) {
				return crow::response(400, "Invalid credentials");
			}
			return crow::response(200, "Succesfully logged in");
		} catch(...) {
			return crow::response(400, "Invalid request body");
		}
	});

	CROW_ROUTE(app, "/signup").methods(crow::HTTPMethod::POST)([](const crow::request& req){
		crow::json::rvalue x = crow::json::load(req.body);
		Developer D;
		bool success;
		try {
			D.developer_email = x["developer_email"].s();
			D.developer_password = x["developer_password"].s();
			success = add_developer(D);
			if(!success) {
				return crow::response(400, "Developer already exists");
			}
			return crow::response(200, "Succesfully signed up");
		} catch(...) {
			return crow::response(400, "Invalid request body");
		}
	});

	CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::DELETE)([](const crow::request& req){
		crow::json::rvalue x = crow::json::load(req.body);
		std::string developer_email;
		std::string developer_password;
		Developer D;

		try {
			developer_email = x["developer_email"].s();
			developer_password = x["developer_password"].s();
			D = get_developer(developer_email);
			if(!D.is_valid) {
				return crow::response(400, "User not found");
			}

			if(D.developer_password != developer_password) {
				return crow::response(400, "Invalid credentials");
			}

			D = remove_developer(developer_email);
			if(!D.is_valid) {
				return crow::response(500, "Internal Server Error");
			}
			return crow::response(200, "Succesfully deleted account");
		} catch(...) {
			return crow::response(400, "Invalid request body");
		}
	});

	//set the port, set the app to run on multiple threads, and run the app
	app.port(18080).multithreaded().run();
	return 0;
}
