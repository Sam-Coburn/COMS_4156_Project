CREATE USER 'dbuser' IDENTIFIED BY '123';
GRANT ALL PRIVILEGES ON *.* TO 'dbuser';

CREATE SCHEMA test_matchmaking_api_db;

CREATE SCHEMA matchmaking_api_db;
USE matchmaking_api_db;

CREATE TABLE Developers (
    developer_email varchar(255) NOT NULL,
  	developer_password varchar(255) NOT NULL,
  	PRIMARY KEY (developer_email)
);

CREATE TABLE Players (
    player_email varchar(255) NOT NULL,
  	PRIMARY KEY (player_email)
);

CREATE TABLE Game_Details (
    game_id int NOT NULL auto_increment,
	developer_email varchar(255) NOT NULL,
  	game_name varchar(255) NOT NULL,
  	game_parameter1_name varchar(255),
  	game_parameter1_weight float,
  	game_parameter2_name varchar(255),
  	game_parameter2_weight float,
  	game_parameter3_name varchar(255),
  	game_parameter3_weight float,
  	game_parameter4_name varchar(255),
  	game_parameter4_weight float,
  	category varchar(255),
  	players_per_team int,
  	teams_per_match int,
  	PRIMARY KEY (game_id),
	FOREIGN KEY (developer_email) REFERENCES Developers(developer_email) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE Player_Game_Ratings (
  	player_email varchar(255) NOT NULL,
    game_id int NOT NULL,
  	game_parameter1_value float,
  	game_parameter2_value float,
  	game_parameter3_value float,
  	game_parameter4_value float,
  	PRIMARY KEY (player_email, game_id),
  	FOREIGN KEY (player_email) REFERENCES Players(player_email) ON DELETE CASCADE ON UPDATE CASCADE,
  	FOREIGN KEY (game_id) REFERENCES Game_Details(game_id) ON DELETE CASCADE ON UPDATE CASCADE
);