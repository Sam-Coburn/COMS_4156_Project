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
	FOREIGN KEY (developer_email) REFERENCES Developers(developer_email) ON DELETE CASCADE
);

CREATE TABLE Player_Game_Ratings (
  	player_email varchar(255) NOT NULL,
    game_id int NOT NULL,
  	game_parameter1_value int,
  	game_parameter2_value int,
  	game_parameter3_value int,
  	game_parameter4_value int,
  	PRIMARY KEY (player_email, game_id),
  	FOREIGN KEY (player_email) REFERENCES Players(player_email) ON DELETE CASCADE,
  	FOREIGN KEY (game_id) REFERENCES Game_Details(game_id) ON DELETE CASCADE
);

INSERT INTO Developers VALUES('developer@chess.com', 'super_secret_password');
INSERT INTO Developers VALUES('techlead@apex.com', 'another_password');
INSERT INTO Developers VALUES('admin@overwatch.com', 'password123');

INSERT INTO Game_Details (game_name, developer_email, game_parameter1_name, game_parameter1_weight, game_parameter2_name,
                        game_parameter2_weight, game_parameter3_name, game_parameter3_weight,
                        game_parameter4_name, game_parameter4_weight, category, players_per_team,
                        teams_per_match)
VALUES('Apex Legends', 'techlead@apex.com', 'kills', 1.0, 'deaths', -.8, 'assists', .5, 'heals', .5, 'Battle Royale', 3, 20);

INSERT INTO Game_Details (game_name, developer_email, game_parameter1_name, game_parameter1_weight, game_parameter2_name,
                        game_parameter2_weight, game_parameter3_name, game_parameter3_weight,
                        game_parameter4_name, game_parameter4_weight, category, players_per_team,
                        teams_per_match)
VALUES('Chess','developer@chess.com', 'rating', 1, '', 0, '', 0, '', 0, 'Strategy', 1, 2);

INSERT INTO Game_Details (game_name, developer_email, game_parameter1_name, game_parameter1_weight, game_parameter2_name,
                        game_parameter2_weight, game_parameter3_name, game_parameter3_weight,
                        game_parameter4_name, game_parameter4_weight, category, players_per_team,
                        teams_per_match)
VALUES('Overwatch','admin@overwatch.com', 'kills', 1, 'deaths', -1, 'assists', .5, 'heals', .8, 'FPS', 5, 2);

INSERT INTO Players VALUES('apex_player@gmail.com');
INSERT INTO Players VALUES('apex_and_chess_player@gmail.com');
INSERT INTO Players VALUES('chess_player@gmail.com');

INSERT INTO Player_Game_Ratings VALUES('apex_player@gmail.com', 1, 100, 50, 25, 10);
INSERT INTO Player_Game_Ratings VALUES('chess_player@gmail.com', 2, 1236, 0, 0, 0);
INSERT INTO Player_Game_Ratings VALUES('apex_and_chess_player@gmail.com', 1, 500, 343, 37, 82);
INSERT INTO Player_Game_Ratings VALUES('apex_and_chess_player@gmail.com', 2, 1321, 0, 0, 0);