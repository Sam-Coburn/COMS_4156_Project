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
	crow::SimpleApp app; //define your crow application

	//define your endpoint at the root directory
	CROW_ROUTE(app, "/")([](){
		return "Hello world";
	});

	//set the port, set the app to run on multiple threads, and run the app
	app.port(18080).multithreaded().run();
	return 0;
}
