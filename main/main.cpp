#include "crow/crow_all.h"
#include "db-service.h"


using namespace std;

int main(void)
{
	get_all_players();
	get_all_developers();
	crow::SimpleApp app; //define your crow application

	//define your endpoint at the root directory
	CROW_ROUTE(app, "/")([](){
		return "Hello world";
	});

	//set the port, set the app to run on multiple threads, and run the app
	app.port(18080).multithreaded().run();
	return 0;
}
