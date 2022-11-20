// Copyright [2022] RaisingCanesFanClub

#ifndef AUTHENTICATION_AUTH_SERVICE_H_
#define AUTHENTICATION_AUTH_SERVICE_H_

#include <glog/logging.h>
#include <utility>
#include <string>
#include <iostream>


/* 
The functions in this class are mainly just wrappers of existing
libraries. The source code for these libraries are located in 
this folder along with their licenses.
*/

class AuthService {
 public:
    // Destructor marked virtual
    virtual ~AuthService() {}

    // Hashes password using bcrypt
    // Note this encryption is one way and not one to
    // one so you cannot use this function to verify
    // a hashed password i.e. don't call this twice on
    // the same input and expect the same output.
    virtual std::string encryptPassword(std::string password);

    // Validates a given password against a hashed password
    virtual bool validatePassword(std::string password, std::string hash);

    // Generates a JWT token that will expire in the specified
    // number of seconds given an email.
    virtual std::string createJWT(std::string email, int seconds = 3600);

    // Decodes and verifies a given JWT token in the form of a pair
    // If the first element of the pair is false the token is invalid
    // and the error message is in the second element of the return
    // value. If the first element of the pair is true, the token is
    // valid and the second element of the pair is the developer
    // email.
    virtual std::pair<bool, std::string> decodeAndVerifyJWT(std::string token);
};
#endif  // AUTHENTICATION_AUTH_SERVICE_H_
