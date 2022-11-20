/*Copyright [year] <Copyright Owner>*/

#include "authentication/auth-service.h"
#include "bcrypt/bcrypt.h"
#include <jwt-cpp/jwt.h>


std::string AuthService::encryptPassword(std::string password) {
    return bcrypt::generateHash(password);
}

bool AuthService::validatePassword(std::string password, std::string hash) {
    return bcrypt::validatePassword(password, hash);
}

std::string AuthService::createJWT(std::string email, int seconds) {
    std::string token = jwt::create()
    .set_issuer("matchmaking-api")
    .set_type("JWT")
    .set_issued_at(std::chrono::system_clock::now())
    .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{seconds})
    .set_payload_claim("email", jwt::claim(std::string(email))) // developer_email
    .sign(jwt::algorithm::hs256{"secret"}); // ideally should be environmental variable

   return token;
}

std::pair<bool, std::string> AuthService::decodeAndVerifyJWT(std::string token) {
    auto verifier = jwt::verify()
    .allow_algorithm(jwt::algorithm::hs256{ "secret" })
    .with_issuer("matchmaking-api");

    try {
        auto decoded = jwt::decode(token);
		verifier.verify(decoded);
        for(auto& e : decoded.get_payload_json())
            if (e.first == "email")
                return std::make_pair(true, std::string(e.second.to_str()));
                
	} catch (const std::exception& ex) { 
        return std::make_pair(false, ex.what());
    }

    return std::make_pair(false, "Invalid token");
}

