// "Copyright [2022] <Copyright Owner>"

#include "crow/crow_all.h"
#include "main/db-service.h"

int main(void) {
  // Crow application definition
  crow::SimpleApp app;

  // GET /
  CROW_ROUTE(app, "/")([](){
    return "Welcome to Matchmaking API";
  });

  /*
    POST /login
    Description:
      Due to the nature of our authentication this endpoint
      essentially just verifies credentials are correct

    Request Body:
      developer_email:string,
      developer_password:string
    
    Response:
      - Status Code 200 for successful login
      - Status Code 400 for invalid request body
      - Status Code 401 for invalid credentials
  */
  CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req){
    crow::json::rvalue body = crow::json::load(req.body);
    std::string developer_email;
    std::string developer_password;
    Developer D;

    try {
      developer_email = body["developer_email"].s();
      developer_password = body["developer_password"].s();
      DBService DB = DBService();
      D = DB.get_developer(developer_email);
      if (!D.is_valid || D.developer_password != developer_password) {
        return crow::response(401, "Invalid credentials");
      }
      return crow::response(200, "Succesfully logged in");
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
  });

  /*
    POST /signup
    Description:
      Adds a developer to the database with the
      specified credentials
      
    Request Body:
      developer_email:string,
      developer_password:string
    
    Response:
      - Status Code 200 for successful signup
      - Status Code 400 for invalid request body
      - Status Code 409 for developer already exists
  */
  CROW_ROUTE(app, "/signup").methods(crow::HTTPMethod::POST)
  ([](const crow::request& req){
    crow::json::rvalue body = crow::json::load(req.body);
    Developer D;
    try {
      D.developer_email = body["developer_email"].s();
      D.developer_password = body["developer_password"].s();
      DBService DB = DBService();
      D = DB.add_developer(D);
      if (!D.is_valid) {
        return crow::response(409, "Developer already exists");
      }
      return crow::response(200, "Succesfully signed up");
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
  });

  /*
    DELETE /login
    Description:
      Deletes a devloper from database provided the
      credentials match developer trying to be deleted
      
    Request Body:
      developer_email:string,
      developer_password:string
    
    Response:
      - Status Code 200 for successful deletion
      - Status Code 400 for invalid request body
      - Status Code 401 for invalid credentials
      - Status Code 404 for developer not found
      - Status Code 500 for internal server error
  */
  CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::DELETE)
  ([](const crow::request& req){
    crow::json::rvalue body = crow::json::load(req.body);
    std::string developer_email;
    std::string developer_password;
    Developer D;

    try {
      developer_email = body["developer_email"].s();
      developer_password = body["developer_password"].s();
      DBService DB = DBService();
      D = DB.get_developer(developer_email);
      if (!D.is_valid) {
        return crow::response(404, "Developer not found");
      }

      if (D.developer_password != developer_password) {
        return crow::response(401, "Invalid credentials");
      }

      D = DB.remove_developer(developer_email);
      if (!D.is_valid) {
        return crow::response(500, "Internal Server Error");
      }
      return crow::response(200, "Succesfully deleted account");
    } catch(...) {
      return crow::response(400, "Invalid request body");
    }
  });

  // set the port, set the app to run on multiple threads, and run the app
  app.port(18080).multithreaded().run();
  return 0;
}
