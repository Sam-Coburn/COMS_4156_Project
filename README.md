# Matchmaking API
## Endpoints
- `POST /login`
    - Description:
      Due to the nature of our authentication this endpoint
      essentially just verifies credentials are correct
    - Request Body:
      - `developer_email:string`
      - `developer_password:string`
      
- `DELETE /login`
    - Description
      Deletes a devloper from database provided the
      credentials match developer trying to be deleted
    - Request Body:
      - `developer_email:string`
      - `developer_password:string`
    
- `POST /signup`
    - Description:
      Adds a developer to the database with the
      specified credentials
    - Request Body:
      - `developer_email:string`
      - `developer_password:string`

- `POST /games`
    - Description:
      Add a game to developer's account
      specified credentials
    - Request Body:
      - `developer_email:string`
      - `developer_password:string`
      - `name:string`
      - `category:string`
      - `parameters:Array<String>`
      - `weights:Array<Float>`
      - `teams_per_match:Integer`
      - `players_per_team:Integer`

- `GET /games`
    - Description:
      Retrieve a list of all games assocaited with developer's account
      specified credentials
    - Request Body:
      - `developer_email:string`
      - `developer_password:string`

- `POST /matchmake`
    - Description:
      Use the given list of player emails to sort players into game lobbies
      using the given game's parameters.
    - Request Body:
      - `developer_email:string`
      - `developer_password:string`
      - `game_id:int`
      - `player_emails:vector<string>`
      
- `GET /game/<int>`
    - Description:
      Returns game details in JSON format. Details include, but are not limited to, game name, players per team, etc.
    - Request Parameters:
      - `game_id:int`
    - Request Body:
      - `developer_email:string`
      - `developer_password:string`
 
 - `DELETE /game/<int>`
    - Description:
      Deletes a game (and its details).
    - Request Parameters:
      - `game_id:int`
    - Request Body:
      - `developer_email:string`
      - `developer_password:string`

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

### Style Checker
Once cpplint is installed run:
`cpplint --linelength=120 ./main/* ./api-endpoints/* ./testing/* &> style-check.txt`

## Repo Structure

    └── COMS_4156_Project
       ├── crow
       │   ├── BUILD
       │   └── crow-all.h
       ├── main
       │   ├── BUILD
       │   ├── db-service.cpp
       │   ├── db-service.h
       │   └── main.cpp
       ├── testing
       │   ├── BUILD
       │   └── test.ccp
       └── WORKSPACE

## Install Bazel and jsoncpp Library
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
Install Jsconcpp:
```sudo apt-get install libjsoncpp-dev```

**Platform: Ubuntu 22.04**

    sudo apt install npm
    sudo npm install -g @bazel/bazelisk

General Install Instructions:
https://bazel.build/install

Install Jsconcpp:
```sudo apt-get install libjsoncpp-dev```

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

5. Install Boost for Crow\
	```sudo apt-get install libboost-all-dev```

6. Restart the VM

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
## Example Run of Project
Navigate to Project Directory\
Run:

    bazel build //main:main
    bazel-bin/main/main


When you start the project you should see something like the following:
```
(2022-10-10 15:01:13) [INFO    ] Crow/1.0 server is running at http://0.0.0.0:18080 using 2 threads
(2022-10-10 15:01:13) [INFO    ] Call `app.loglevel(crow::LogLevel::Warning)` to hide Info level logs.

```
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
