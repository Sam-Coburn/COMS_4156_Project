# Matchmaking API


### How to Run the Project
**Platform: Debian 11**
1. Install Bazel: https://bazel.build/install/ubuntu
```
	sudo apt install apt-transport-https curl gnupg
	curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor >bazel-archive-keyring.gpg
	sudo mv bazel-archive-keyring.gpg /usr/share/keyrings
	echo "deb [arch=amd64 signed-by=/usr/share/keyrings/bazel-archive-keyring.gpg] https://storage.googleapis.com/bazel-apt stable jdk1.8" | sudo tee /etc/apt/sources.list.d/bazel.list
	
	sudo apt update && sudo apt install bazel
	sudo apt update && sudo apt full-upgrade
```

2. Install MySQL: https://computingforgeeks.com/how-to-install-mysql-8-0-on-debian/
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
	```source /home/deb/Desktop/matchmaking_api_db_sql.sql;```

	Exit Console: ```quit```

4. Install C++ Connector for MySQL

	```sudo apt install libmysqlcppconn-dev```

5. Install Boost for Crow\
	```sudo apt-get install libboost-all-dev```

6. Restart the VM

7. Run Project\
	Navigate to Project Directory\
	Run:
	```
	bazel build //main:main
	bazel-bin/main/main
	```

The project does some simple SQL queries and then starts the server
So you should see something like the following:

```
Connecting to MYSQL to get player with email: apex_player@gmail.com
Player: player_email = apex_player@gmail.com
Successfully retrieved player with email: apex_player@gmail.com

Connecting to MYSQL to get developer with email: developer@chess.com
Developer: developer_email = developer@chess.com, developer_password = super_secret_password
Successfully retrieved developer with email: developer@chess.com

Connecting to MYSQL to get game with with: 2
GameDetails:
game_id = 2, developer_email = developer@chess.com, game_name = Chess
game_parameter1_name = rating, game_parameter1_weight = 1.000000
category = Strategy, players_per_team = 1, teams_per_match = 2
Successfully retrieved game with game_id: 2

Connecting to MYSQL to get rating in game with game_id: 1 for player with email: apex_player@gmail.com
Player Game Ratings:
player_email = apex_player@gmail.comgame_id = 1
game_parameter1_value = 100
game_parameter2_value = 50
game_parameter3_value = 25
game_parameter4_value = 10
Successfully retrieved rating in game with game_id: 1 for player with email: apex_player@gmail.com

Connecting to MYSQL to get rating in game with game_id: 1 for player with email: apex_player@gmail.com
Joined Player Game Ratings:
game_id = 1, developer_email = techlead@apex.com, game_name = Apex Legends
player_email = apex_player@gmail.com
game_parameter1_name = kills, game_parameter1_weight = 1.000000, game_parameter1_value = 100
game_parameter2_name = deaths, game_parameter2_weight = -0.800000, game_parameter2_value = 50
game_parameter3_name = assists, game_parameter3_weight = 0.500000, game_parameter3_value = 25
game_parameter4_name = heals, game_parameter4_weight = 0.500000, game_parameter4_value = 10
category = Battle Royale, players_per_team = 3, teams_per_match = 20
Successfully retrieved rating in game with game_id: 1 for player with email: apex_player@gmail.com

Connecting to MYSQL to get all players
Player: player_email = apex_and_chess_player@gmail.com
Player: player_email = apex_player@gmail.com
Player: player_email = chess_player@gmail.com
Successfully retrieved all players

Connecting to MYSQL to get all developers
Developer: developer_email = admin@overwatch.com, developer_password = password123
Developer: developer_email = developer@chess.com, developer_password = super_secret_password
Developer: developer_email = techlead@apex.com, developer_password = another_password
Successfully retrieved all developers

Connecting to MYSQL to get all game details
GameDetails:
game_id = 1, developer_email = techlead@apex.com, game_name = Apex Legends
game_parameter1_name = kills, game_parameter1_weight = 1.000000
game_parameter2_name = deaths, game_parameter2_weight = -0.800000
game_parameter3_name = assists, game_parameter3_weight = 0.500000
game_parameter4_name = heals, game_parameter4_weight = 0.500000
category = Battle Royale, players_per_team = 3, teams_per_match = 20
GameDetails:
game_id = 2, developer_email = developer@chess.com, game_name = Chess
game_parameter1_name = rating, game_parameter1_weight = 1.000000
category = Strategy, players_per_team = 1, teams_per_match = 2
GameDetails:
game_id = 3, developer_email = admin@overwatch.com, game_name = Overwatch
game_parameter1_name = kills, game_parameter1_weight = 1.000000
game_parameter2_name = deaths, game_parameter2_weight = -1.000000
game_parameter3_name = assists, game_parameter3_weight = 0.500000
game_parameter4_name = heals, game_parameter4_weight = 0.800000
category = FPS, players_per_team = 5, teams_per_match = 2
Successfully retrieved all game details

Connecting to MYSQL to get all player ratings for a game
Joined Player Game Ratings:
game_id = 1, developer_email = techlead@apex.com, game_name = Apex Legends
player_email = apex_and_chess_player@gmail.com
game_parameter1_name = kills, game_parameter1_weight = 1.000000, game_parameter1_value = 500
game_parameter2_name = deaths, game_parameter2_weight = -0.800000, game_parameter2_value = 343
game_parameter3_name = assists, game_parameter3_weight = 0.500000, game_parameter3_value = 37
game_parameter4_name = heals, game_parameter4_weight = 0.500000, game_parameter4_value = 82
category = Battle Royale, players_per_team = 3, teams_per_match = 20
Joined Player Game Ratings:
game_id = 1, developer_email = techlead@apex.com, game_name = Apex Legends
player_email = apex_player@gmail.com
game_parameter1_name = kills, game_parameter1_weight = 1.000000, game_parameter1_value = 100
game_parameter2_name = deaths, game_parameter2_weight = -0.800000, game_parameter2_value = 50
game_parameter3_name = assists, game_parameter3_weight = 0.500000, game_parameter3_value = 25
game_parameter4_name = heals, game_parameter4_weight = 0.500000, game_parameter4_value = 10
category = Battle Royale, players_per_team = 3, teams_per_match = 20
Successfully retrieved all player game ratings

(2022-10-10 15:01:13) [INFO    ] Crow/1.0 server is running at http://0.0.0.0:18080 using 2 threads
(2022-10-10 15:01:13) [INFO    ] Call `app.loglevel(crow::LogLevel::Warning)` to hide Info level logs.

```


	
