# Matchmaking API

## Client Link
Client (Web UI): https://github.com/uarman29/COMS4156_Matchmaking_Client \
Alternative Client (Server): https://github.com/uarman29/COMS4156_Matchmaking_Client2

## Endpoints
- `POST /signup`
  - Description:
    Adds a developer to the database with the
    specified credentials
  - Request Body:
    - `developer_email:string`
    - `developer_password:string`
  - Response Codes:
    - ```200: Success```
    - ```400: Invalid Input```
    - ```409: Developer already exists```


- `POST /login`
  - Description:
    Verifies a developer's credentials and returns
    a JWT token to serve as their API Key
  - Request Body:
    - `developer_email:string`
    - `developer_password:string`
  - Response Codes:
    - ```200: Success```
    - ```400: Invalid Input```
    - ```401: Invalid Credentials```
    - ```404: Developer does not exist```
      
- `DELETE /login`
  - Description
    Deletes a devloper from database provided the
    credentials match developer trying to be deleted
  - Authorization Header: `Bearer <API-Key>`
  - Response Codes:
    - ```200: Success```
    - ```401: Invalid Token```
    - ```404: Developer does not exist```
    - ```500: Internal Server Error```
    
- `POST /games`
  - Description:
    Add a game to developer's account
  - Authorization Header: `Bearer <API-Key>`
  - Request Body:
    - `name:string`
    - `category:string`
    - `parameters:Array<String>`
    - `weights:Array<Float>`
    - `teams_per_match:Integer`
    - `players_per_team:Integer`
  - Response Codes:
    - ```200: Success```
    - ```400: Invalid Input```
    - ```401: Invalid Token```
    

- `GET /games`
  - Description:
    Retrieve a list of all games assocaited with developer's account
  - Authorization Header: `Bearer <API-Key>`
  - Response Codes:
    - ```200: Success```
    - ```204: No Content```
    - ```401: Invalid Token```

- `POST /matchmake`
  - Description:
    Use the given list of player emails to sort players into game lobbies
    using the given game's parameters.
  - Authorization Header: `Bearer <API-Key>`
  - Request Body:
    - `game_id:int`
    - `matchmaking_type: 'basic' or 'advanced'`
    - `player_emails:vector<string>`
  - Response Codes:
    - ```200: Success```
    - ```400: Invalid Input```
    - ```401: Invalid Credentials```
      
- `GET /games/<int>`
  - Description:
    Returns game details in JSON format. Details include, but are not limited to, game name, players per team, etc.
  - Request Parameters:
    - `game_id:int`
  - Authorization Header: `Bearer <API-Key>`
  - Response Codes:
    - ```200: Success```
    - ```401: Invalid Token / No token```
    - ```403: Not authorized```
    - ```404: Game Id not found```

- `PUT /games/<int>`
  - Description:
    Updates a game's details.
  - Request Parameters:
    - `game_id:int`
  - Authorization Header: `Bearer <API-Key>`
  - Request Body:
    - `name:string`
    - `category:string`
    - `game_parameter1_name:string`
    - `game_parameter2_name:string`
    - `game_parameter3_name:string`
    - `game_parameter4_name:string`
    - `game_parameter1_weight:float`
    - `game_parameter2_weight:float`
    - `game_parameter3_weight:float`
    - `game_parameter4_weight:float`
    - `teams_per_match:Integer`
    - `players_per_team:Integer`
  - Response Codes:
    - ```200: Success```
    - ```400: Invalid Input```
    - ```401: Invalid Token```
    - ```403: Not authorized```
    - ```404: Game Id not found```
 
- `DELETE /games/<int>`
  - Description:
    Deletes a game (and its details).
  - Request Parameters:
    - `game_id:int`
  - Authorization Header: `Bearer <API-Key>`
  - Response Code:
    - ```200: Success```
    - ```401: Invalid Token```
    - ```403: Not authorized```
    - ```404: Game id not found```
 
- `GET /games/{game-id}/players`
  - Description:
    Gets a list of players and player details for a game.
  - Request Parameters:
    - `game_id:int`
  - Authorization Header: `Bearer <API-Key>`
  - Response Codes:
    - ```200: Success```
    - ```204: No players found```
    - ```401: Invalid Token```
    - ```403: Not authorized```
    - ```404: Game id not found```
    - ```500: Internal Server Error```

- `POST /games/<int>/players`
  - Description:
    Adds players' stats for a game.
  - Request Parameters:
    - `game_id:int`
  - Authorization Header: `Bearer <API-Key>`
  - Request Body:
    - `(player_email:<JsonObject>)+`
      where `<JsonObject>` is of the form
      - `game_parameter1_value:float`
      - `game_parameter2_value:float`
      - `game_parameter3_value:float`
      - `game_parameter4_value:float`
  - Response Codes:
    - ```200: Success```
    - ```400: Invalid Input```
    - ```401: Invalid Token```
    - ```403: Not authorized```
    - ```404: Game id not found```
    - ```409: Player rating already exists```
    - ```417: Invalid input fields```
    - ```500: Internal Server Error```

- `GET /games/{game-id}/players/{player_email}`
  - Description:
    Gets player stats for a game.
  - Request Parameters:
    - `game_id:int`
    - `player_email:string`
  - Authorization Header: `Bearer <API-Key>`
  - Response Codes:
    - ```200: Success```
    - ```400: Invalid Input```
    - ```401: Invalid Token```
    - ```403: Not authorized```
    - ```404: Rating not found```

- `PUT /games/{game_id}/players/{player_email}`
  - Description:
    Updates player's stats for a game.
  - Request Parameters:
    - `game_id:int`
    - `player_email:string`
  - Authorization Header: `Bearer <API-Key>`
  - Request Body:
    - `game_parameter1_value:float`
    - `game_parameter2_value:float`
    - `game_parameter3_value:float`
    - `game_parameter4_value:float`
  - Response Codes:
    - ```200: Success```
    - ```400: Invalid Input```
    - ```401: Invalid Token```
    - ```403: Not authorized```
    - ```404: Game id not found```
    - ```417: Invalid Input Fields```
    - ```500: Internal Server Error```
    
- `DELETE /games/{game_id}/players/{player_email}`
  - Description:
    Removes player's stats for a game.
  - Request Parameters:
    - `game_id:int`
    - `player_email:string`
  - Authorization Header: `Bearer <API-Key>`
  - Response Codes:
    - ```200: Success```
    - ```400: Invalid Input```
    - ```401: Invalid Token```
    - ```403: Not authorized```
    - ```404: Game id not found```


      
## Build + Run
Inside top-level project directory, do
```
bazel build //main:main
bazel-bin/main/main
```
## Build + Run + Test
### Building
Build command is of the form

**`bazel build //package-name:build-target-name`**

For example,

    bazel build //main:main
### Running
Executable files are located in bazel-bin like so

**`bazel-bin/package-name/executable-name`**

For example, to run hello world, do

    bazel-bin/main/main
### Testing
Test command is of the form

**`bazel test --test_output=all //package-name:test-target-name`**

For example,

    bazel test --test_output=all //testing:test

#### Testing with Logging Levels
We use [glog](https://github.com/google/glog#severity-levels) for our logging output, which has log levels `INFO`, `WARNING`, `ERROR`, and `FATAL`, corresponding to 0, 1, 2, and 3.
- To run a test and only see error messages or worse, run
`bazel test //testing:test --test_output=all --test_env=GLOG_minloglevel=1`
- To run a test and see all logging output, run 
`bazel test //testing:test --test_output=all --test_env=GLOG_minloglevel=0`

### Style Checker
Once cpplint is installed run:
`cpplint --linelength=120 api-endpoints/* authentication/* main/* testing/*`

### Static Analysis
We use two command line tools for static analysis, flawfinder and cppchecker.
Once flawfinder is installed run:
`flawfinder api-endpoints/* authentication/auth-service.h authentication/auth-service.cpp main/* testing/*`
Once cppchecker is installed run:
```
cppcheck --language=c++ api-endpoints/*.cc api-endpoints/*.cpp api-endpoints/*.h	      
cppcheck --language=c++ authentication/auth-service.h authentication/auth-service.cpp	
cppcheck --language=c++ main/*.cc main/*.cpp main/*.h	     
cppcheck --language=c++ testing/*.cc testing/*.cpp testing/*.h
```
## Continuous Integration Reports
We run `bash ./reports/generateCIReports.sh` to generate reports and store them in the [reports](./reports/) directory. Links to individual reports below.
- [cpplint report](./reports/style-check.txt)
- [flawfinder report](./reports/flawfinder.txt)
- [cppcheck report](./reports/cppcheck.txt)
- [regression tests report](./reports/test-report.txt)
- [coverage report](./reports/coverage.txt)
- [coverage report as interactive webpage](./genhtml/index.html)

## Install Bazel
**Platform: Debian 11**
Reference instructions: https://bazel.build/install/ubuntu
```
	sudo apt install apt-transport-https curl gnupg
	curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor >bazel-archive-keyring.gpg
	sudo mv bazel-archive-keyring.gpg /usr/share/keyrings
	echo "deb [arch=amd64 signed-by=/usr/share/keyrings/bazel-archive-keyring.gpg] https://storage.googleapis.com/bazel-apt stable jdk1.8" | sudo tee /etc/apt/sources.list.d/bazel.list
	
	sudo apt update && sudo apt install bazel
	sudo apt update && sudo apt full-upgrade
```

**Platform: Ubuntu 22.04**
```
    sudo apt install npm
    sudo npm install -g @bazel/bazelisk
```
General Install Instructions:
https://bazel.build/install

## Install MySql + Set Up Database
**Platform: Debian 11**
1. Install MySQL: https://computingforgeeks.com/how-to-install-mysql-8-0-on-debian/
    ```
	sudo apt update && sudo apt -y  install wget
	wget https://repo.mysql.com//mysql-apt-config_0.8.22-1_all.deb
	sudo dpkg -i mysql-apt-config_0.8.22-1_all.deb
    ```
	You will be prompted, just navigate to OK and press Enter
	```
	sudo apt update
	sudo apt install mysql-server
	```
	You will be prompted to enter a password for root.\
	You will also be prompted for encryption type, just press enter

	Check status with: ```systemctl status mysql```

3. Setup Database\
    Enter the SQL Console
    ```
	sudo mysql -u root -p
	```
	Enter Root Password when prompted

	Run the provided SQL File (replace path with path to sql file):\
	```source /home/deb/Desktop/COMS_4156_Project/matchmaking_api_db_sql.sql```

	Exit Console: ```quit```

4. Install C++ Connector for MySQL

	```sudo apt install libmysqlcppconn-dev```
5. Restart the VM

**Platform: Ubuntu 22.04**
Instructions based on this tutorial: https://www.digitalocean.com/community/tutorials/how-to-install-mysql-on-ubuntu-22-04
^^Do NOT run the security script `mysql_secure_installation`, as this prevents our dbuser's password from being "123".

1. Install mysql: 
```
sudo apt update
sudo apt install mysql-server
sudo systemctl start mysql.service
```
Check status with: ```systemctl status mysql```

2. Set up users and permissions:
```
sudo mysql
ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'password';
ALTER USER 'dbuser'@'localhost' IDENTIFIED WITH mysql_native_password BY '123';
GRANT ALL ON *.* TO 'dbuser'@'localhost';
SELECT user,host, authentication_string FROM mysql.user;
exit
```
3. Install C++ Connector for MySQL

	```sudo apt install libmysqlcppconn-dev```

4. Install Boost for Crow\
	```sudo apt-get install libboost-all-dev```

5. Set up the DB:
  When prompted for a password, enter "password"
  ```
  mysql -u root -p
  source /home/spinel/COMS_4156_Project/matchmaking_api_db_sql.sql;
  SHOW DATABASES;
  exit
  ```
## Install Remaining Dependencies + Dev Tools
**Platform: Debian 11**
1. Install Git
  ```sudo apt-get install git```

2. Install Boost for Crow
	```sudo apt-get install libboost-all-dev```

3. Install JSONCPP
  ```sudo apt-get install libjsoncpp-dev```

4. Install Auth Libraries
  ```sudo apt-get install libssl-dev```

5. Install glog for logging
```sudo apt install libgoogle-glog-dev```

6. Install cpplint StyleChecker
```
sudo apt install -y python3-pip
pip3 install cpplint
```
7. Install flawchecker security checker
```pip install flawfinder```

8. Install cppcheck static analysis checker
```sudo apt-get -y install cppcheck```

9. (Optional) Install lcov & jdk for coverage reports
  ```
  sudo apt-get install default-jdk
  sudo apt-get install lcov
  ```

10. Restart the VM

**Platform: Ubuntu 22.04**
1. Install Git
  ```sudo apt-get install git```

2. Install Boost for Crow
	```sudo apt-get install libboost-all-dev```

3. Install JSONCPP
  ```sudo apt-get install libjsoncpp-dev```

4. Install Auth Libraries
  ```sudo apt-get install libssl-dev```

5. Install glog for logging
```sudo apt install libgoogle-glog-dev```

6. Install cpplint StyleChecker
```
sudo apt install -y python3-pip
pip3 install cpplint
```

7. Install flawchecker security checker
```pip install flawfinder```

8. Install cppcheck static analysis checker
```sudo apt-get -y install cppcheck```

## Example Run of Project
Navigate to Project Directory
Run:
```
    bazel build //main:main
    bazel-bin/main/main
```

When you start the project you should see something like the following:
```
(2022-10-10 15:01:13) [INFO    ] Crow/1.0 server is running at http://0.0.0.0:18080 using 2 threads
(2022-10-10 15:01:13) [INFO    ] Call `app.loglevel(crow::LogLevel::Warning)` to hide Info level logs.

```

## How to build your own client
To make a client using our service the third party would simply have to make HTTP requests to our endpoints. Our documentation specifies the parameters, headers, response codes, and bodies required for each endpoint so as long as the third party makes http requests using these specifications and catches every response code it would be simple to create their own client. 

For examples you can navigate to /src/app/services/matchmaking-api.service.ts in our sample Matchmaking Client which essentially just wraps every http call to every one of our endpoints in its own function.


## Extra Notes
Tutorials I read to make this set up bazel + test:
https://bazel.build/start/cpp
http://google.github.io/googletest/quickstart-bazel.html

Extra Resources I skimmed or thought might be helpful later:
https://bazel.build/build/style-guide
https://bazel.build/tutorials/cpp-use-cases
http://google.github.io/googletest/primer.html
http://google.github.io/googletest/gmock_for_dummies.html
https://qiangbo-workspace.oss-cn-shanghai.aliyuncs.com/2018-12-05-gtest-and-coverage/PlainGoogleQuickTestReferenceGuide1.pdf
