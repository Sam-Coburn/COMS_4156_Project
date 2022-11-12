
# postman-testing folder
## Contents
  
 1.  `README.md` (this file)
Contains example input/output for each endpoint in plain text

  

2) `Matchmaking API Test Requests Deployed Version.postman_collection.json`
Contains exact postman tests used during our demo; you should be able import this file into postman.

  

## Example Input for Each Endpoint

 **1. Sign up -**  `POST /signup`
Adds a developer to the database with the specified credentials.
- Route: `http://localhost:18080/signup`
- Request Body:
```
{
    "developer_email": "emily",
    "developer_password" : "emily_fake_password"
}
```

 **2. Log in -** `POST /login`
Due to the nature of our authentication this endpoint essentially just verifies credentials are correct
- Route: `http://localhost:18080/login`
- Request Body:
```
{
    "developer_email": "emily",
    "developer_password" : "emily_fake_password"
}
```
 **3. Delete Account -** `DELETE /login`
Deletes a developer from database provided the credentials match developer trying to be deleted
- Route: `http://localhost:18080/login`
- Request Body:
```
{
    "developer_email": "emily",
    "developer_password" : "emily_fake_password"
}
```
 **4. Add Game -** `POST /games`
Add a game to developer's account with specified credentials
- Route: `http://localhost:18080/games`
- Request Body:
```
{
    "developer_email":"emily",
    "developer_password":"emily_fake_password",

			"category" : "casual",
			"name" : "uno",
			"parameters" : 
			[
				"wins",
				"loses",
				"nope",
				""
			],
			"players_per_team" : 1,
			"teams_per_match" : 4,
			"weights" : 
			[
				1.0,
				1.0,
				1.0,
				0.0
			]
		}
```
 **5. Get All Games -** `GET /games`
Retrieve a list of all games associated with developer's account with the 
specified credentials
- Route: `http://localhost:18080/games?`
- Request Body:
```
{
    "developer_email": "emily",
    "developer_password" : "emily_fake_password"
}
```
 **6. Add Players -** `POST /game/<int>/players`
Adds players' stats for a game.
- Route: `http://127.0.0.1:18080/game/16/players`
- Request Body:
```
{
    "developer_email":"emily",
    "developer_password":"emily_fake_password",
    "poker_player@gmail.com": { "game_parameter1_value" : 1, "game_parameter2_value" : 5, "game_parameter3_value" : 4, "game_parameter4_value" :0 }, 
    "poker_player2@gmail.com": { "game_parameter1_value" : 2, "game_parameter2_value" : 2, "game_parameter3_value" : 2, "game_parameter4_value" :0 },
    "poker_player3@gmail.com": { "game_parameter1_value" : 1, "game_parameter2_value" : 3, "game_parameter3_value" : 9, "game_parameter4_value" :0 },
    "poker_player4@gmail.com": { "game_parameter1_value" : 1, "game_parameter2_value" : 3, "game_parameter3_value" : 9, "game_parameter4_value" :0 },
    "poker_player5@gmail.com": { "game_parameter1_value" : 1, "game_parameter2_value" : 3, "game_parameter3_value" : 9, "game_parameter4_value" :0 },
    "poker_player6@gmail.com": { "game_parameter1_value" : 1, "game_parameter2_value" : 3, "game_parameter3_value" : 9, "game_parameter4_value" :0 }
}
```
 **7. Match Players into Lobbies -** `POST /matchmake`
Use the given list of player emails to sort players into game lobbies using the given game's parameters.
- Route: `http://localhost:18080/matchmake`
- Request Body:
```
{
    "developer_email": "emily",
    "developer_password" : "emily_fake_password",
    "game_id":16,
    "player_emails": ["poker_player@gmail.com", "poker_player2@gmail.com", "poker_player3@gmail.com", "poker_player4@gmail.com", "poker_player5@gmail.com", "poker_player6@gmail.com"]
}
```
 **8. Get Game Details -** `GET /game/<int>`
Returns game details in JSON format. Details include, but are not limited to, game name, players per team, etc.
- Route: `http://localhost:18080/game/16`
- Request Body:
```
{
    "developer_email": "emily",
    "developer_password" : "emily_fake_password"
}
```
 **9. Delete a Game -**  `DELETE /game/<int>`
Deletes a game (and its details).
- Route: `http://localhost:18080/game/16`
- Request Body:
```
{
    "developer_email": "emily",
    "developer_password" : "emily_fake_password"
}
```
 **10. Get All Game Players -**`GET /games/{game-id}/players`
Gets a list of players and player details for a game.
- Route: `http://127.0.0.1:18080/game/16/players`
- Request Body:
```
{
    "developer_email": "emily",
    "developer_password" : "emily_fake_password"
}
```
 **11. Get Specific Game Player** `GET /games/{game-id}/players/{player-id}`
Gets player stats for a game.
- Route: `http://127.0.0.1:18080/game/16/players/poker_player@gmail.com`
- Request Body:
```
{
    "developer_email": "emily",
    "developer_password" : "emily_fake_password"
}
```
 **12. Delete Players** `DELETE /game/<int>/players`
Removes players' stats for a game.
- Route: `http://127.0.0.1:18080/game/16/players`
- Request Body:
```
{
    "developer_email": "emily",
    "developer_password" : "emily_fake_password",
    "player_emails": ["poker_player@gmail.com", "poker_player2@gmail.com", "poker_player3@gmail.com", "poker_player4@gmail.com", "poker_player5@gmail.com", "poker_player6@gmail.com"]
}
```