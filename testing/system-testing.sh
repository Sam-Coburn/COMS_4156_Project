#!/bin/bash

# Variables
# URL=http://127.0.0.1:80
URL=http://0.0.0.0:8080

developer1_email='nintendo.systest@gmail.com'
developer1_password='nintendo_systest_pwd'

developer2_email='speed.systest@gmail.com'
developer2_password='speed_systest_pwd'

developer3_email='switch.systest@gmail.com'
developer3_password='switch_systest_pwd'

game1_name="SuperSmashBros"
game1_category="Nintendo"
game1_parameter1_name="numHits"
game1_parameter2_name="numKills"
game1_parameter3_name="livesLeft"
game1_parameter4_name="energyLeft"
game1_parameter1_weight=0.6
game1_parameter2_weight=0.2
game1_parameter3_weight=0.15
game1_parameter4_weight=0.05
game1_teams_per_match=6
game1_players_per_team=1

game2_name="SuperMarioOdyssey"
game2_category="Nintendo"
game2_parameter1_name="damage"
game2_parameter2_name="numCoins"
game2_parameter3_name="timeLeft"
game2_parameter4_name="numGifts"
game2_parameter1_weight=0.5
game2_parameter2_weight=0.25
game2_parameter3_weight=0.15
game2_parameter4_weight=0.1
game2_teams_per_match=4
game2_players_per_team=1

game3_name="Speed"
game3_category="Cards"
game3_parameter1_name="roundsWon"
game3_parameter2_name="roundsLost"
game3_parameter3_name=""
game3_parameter4_name=""
game3_parameter1_weight=0.8
game3_parameter2_weight=0.2
game3_parameter3_weight=0
game3_parameter4_weight=0
game3_teams_per_match=2
game3_players_per_team=1

game4_name="MarioParty"
game4_category="Nintendo"
game4_parameter1_name="starsWon"
game4_parameter2_name="tokensWon"
game4_parameter3_name="gamesWon"
game4_parameter4_name="spacesMoved"
game4_parameter1_weight=0.5
game4_parameter2_weight=0.2
game4_parameter3_weight=0.2
game4_parameter4_weight=0.1
game4_teams_per_match=4
game4_players_per_team=1

player1_email="yoshi@gmail.com"
player1_game_parameter1_value=30
player1_game_parameter2_value=2
player1_game_parameter3_value=1
player1_game_parameter4_value=0

player2_email="waluigi@gmail.com"
player2_game_parameter1_value=35
player2_game_parameter2_value=4
player2_game_parameter3_value=0
player2_game_parameter4_value=0

player3_email="bowser@gmail.com"
player3_game_parameter1_value=40
player3_game_parameter2_value=3
player3_game_parameter3_value=1
player3_game_parameter4_value=2

player4_email="peach@gmail.com"
player4_game_parameter1_value=30
player4_game_parameter2_value=6
player4_game_parameter3_value=4
player4_game_parameter4_value=1

player5_email="toad@gmail.com"
player5_game_parameter1_value=10
player5_game_parameter2_value=10
player5_game_parameter3_value=1
player5_game_parameter4_value=1

player6_email="princesspeach@gmail.com"
player6_game_parameter1_value=40
player6_game_parameter2_value=2
player6_game_parameter3_value=1
player6_game_parameter4_value=0

player7_email="princessdaisy@gmail.com"
player7_game_parameter1_value=50
player7_game_parameter2_value=1
player7_game_parameter3_value=0
player7_game_parameter4_value=0

player8_email="kirby@gmail.com"
player8_game_parameter1_value=32
player8_game_parameter2_value=2
player8_game_parameter3_value=0
player8_game_parameter4_value=1

player9_email="link@gmail.com"
player9_game_parameter1_value=20
player9_game_parameter2_value=3
player9_game_parameter3_value=1
player9_game_parameter4_value=1

players=("${player1_email}" "${player2_email}" "${player3_email}"
    "${player4_email}" "${player5_email}" "${player6_email}" "${player7_email}")

num_errors=0

# -------------------------------------------------------
# SIGNING UP AND LOGGING IN
# -------------------------------------------------------

echo "Developers are signing up..."

# Developer 1 signs up
status=$(
    curl -s -X POST ${URL}/signup \
        -H 'Content-Type: application/json' \
        -d "{ \"developer_email\": \"${developer1_email}\", \
            \"developer_password\": \"${developer1_password}\" }"
)
echo $status

# Developer 2 signs up
status=$(
    curl -s -X POST ${URL}/signup \
        -H 'Content-Type: application/json' \
        -d "{ \"developer_email\": \"${developer2_email}\", \
            \"developer_password\": \"${developer2_password}\" }"
)
echo $status

# Developer 3 signs up
status=$(
    curl -s -X POST ${URL}/signup \
        -H 'Content-Type: application/json' \
        -d "{ \"developer_email\": \"${developer3_email}\", \
            \"developer_password\": \"${developer3_password}\" }"
)
echo $status

echo "Developers are logging in..."

# Developer 1 logs in
status=$(
    curl -s -X POST ${URL}/login \
        -H 'Content-Type: application/json' \
        -d "{ \"developer_email\": \"${developer1_email}\", \
            \"developer_password\": \"${developer1_password}\" }"
)
echo $status
token1=${status#*:}

# Developer 2 logs in
status=$(
    curl -s -X POST ${URL}/login \
        -H 'Content-Type: application/json' \
        -d "{ \"developer_email\": \"${developer2_email}\", \
            \"developer_password\": \"${developer2_password}\" }"
)
echo $status
token2=${status#*:}

# Developer 3 logs in
status=$(
    curl -s -X POST ${URL}/login \
        -H 'Content-Type: application/json' \
        -d "{ \"developer_email\": \"${developer3_email}\", \
            \"developer_password\": \"${developer3_password}\" }"
)
echo $status
token3=${status#*:}

# -------------------------------------------------------
# GETTING, CREATING, AND UPDATING GAMES
# -------------------------------------------------------

echo "Developer makes sure they don't own any games yet..."

# Developer 1 gets their games (shouldn't have any games yet)
status=$(
    curl -s -w "%{http_code}" -X GET ${URL}/games \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}"
)

# TEST: Developer 1 shouldn't have any games
if [[ $status -ne 204 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (DeveloperDoesntOwnAnyGamesTest): Developer 1 shouldn't have any games"
fi

echo "Developer adds two games..."

# Developer 1 adds games
status=$(
    curl -s -X POST ${URL}/games \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}" \
        -d '{
            "name": "'${game1_name}'",
            "parameters": ["'${game1_parameter1_name}'", "'${game1_parameter2_name}'", "'${game1_parameter3_name}'", "'${game1_parameter4_name}'"],
            "weights": ['${game1_parameter1_weight}', '${game1_parameter2_weight}', '${game1_parameter3_weight}', '${game1_parameter4_weight}'],
            "category": "'${game1_category}'", 
            "teams_per_match": '${game1_teams_per_match}',
            "players_per_team": '${game1_players_per_team}'
        }'
)
game1_id=$status

status=$(
    curl -s -X POST ${URL}/games \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}" \
        -d '{
            "name": "'${game2_name}'",
            "parameters": ["'${game2_parameter1_name}'", "'${game2_parameter2_name}'", "'${game2_parameter3_name}'", "'${game2_parameter4_name}'"],
            "weights": ['${game2_parameter1_weight}', '${game2_parameter2_weight}', '${game2_parameter3_weight}', '${game2_parameter4_weight}'],
            "category": "'${game2_category}'", 
            "teams_per_match": '${game2_teams_per_match}',
            "players_per_team": '${game2_players_per_team}'
        }'
)
game2_id=$status

echo "Developer 2 adds a game..."

# Developer 2 adds a game
status=$(
    curl -s -X POST ${URL}/games \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token2}" \
        -d '{
            "name": "'${game3_name}'",
            "parameters": ["'${game3_parameter1_name}'", "'${game3_parameter2_name}'", "'${game3_parameter3_name}'", "'${game3_parameter4_name}'"],
            "weights": ['${game3_parameter1_weight}', '${game3_parameter2_weight}', '${game3_parameter3_weight}', '${game3_parameter4_weight}'],
            "category": "'${game3_category}'", 
            "teams_per_match": '${game3_teams_per_match}',
            "players_per_team": '${game3_players_per_team}'
        }'
)
game3_id=$status

echo "Developers get their games..."

# Developer 1 gets their games
status=$(
    curl -s -w "%{stdout}{\"code\": %{http_code}}" \
        -X GET ${URL}/games \
        -H 'Accept: application/json' \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}"
)
echo $status
code=$( jq -r  '.code' <<< "${status}" )
code=($code)
code=${code[1]}
body=$( jq -r  '.games' <<< "${status}" )
body_length=$( jq -r 'length' <<< ${body} )
body_length=$( echo ${body_length} | awk '{print $1;}' )
id1=$( jq -r  '.[0].id' <<< "${body}" )
id1=($id1)
id1=${id1[0]}
id1=$((id1))
name1=$( jq -r  '.[0].name' <<< "${body}" )
name1=($name1)
name1=${name1[0]}
category1=$( jq -r  '.[0].category' <<< "${body}" )
category1=($category1)
category1=${category1[0]}
players_per_team1=$( jq -r  '.[0].players_per_team' <<< "${body}" )
players_per_team1=($players_per_team1)
players_per_team1=${players_per_team1[0]}
teams_per_match1=$( jq -r  '.[0].teams_per_match' <<< "${body}" )
teams_per_match1=($teams_per_match1)
teams_per_match1=${teams_per_match1[0]}
parameters1=$( jq -r  '.[0].parameters' <<< "${body}" )
parameter1_name1=$( jq -r  '.[0]' <<< "${parameters1}" )
parameter1_name1=($parameter1_name1)
parameter1_name1=${parameter1_name1[0]}
parameter2_name1=$( jq -r  '.[1]' <<< "${parameters1}" )
parameter2_name1=($parameter2_name1)
parameter2_name1=${parameter2_name1[0]}
parameter3_name1=$( jq -r  '.[2]' <<< "${parameters1}" )
parameter3_name1=($parameter3_name1)
parameter3_name1=${parameter3_name1[0]}
parameter4_name1=$( jq -r  '.[3]' <<< "${parameters1}" )
parameter4_name1=($parameter4_name1)
parameter4_name1=${parameter4_name1[0]}
weights1=$( jq -r '.[0].weights' <<< "${body}" )
parameter1_weight1=$( jq -r  '.[0]' <<< "${weights1}" )
parameter1_weight1=($parameter1_weight1)
parameter1_weight1=${parameter1_weight1[0]}
parameter1_weight1_diff=$( awk 'BEGIN{printf ("%.3f", '$parameter1_weight1'-'$game1_parameter1_weight')}' )
parameter2_weight1=$( jq -r  '.[1]' <<< "${weights1}" )
parameter2_weight1=($parameter2_weight1)
parameter2_weight1=${parameter2_weight1[0]}
parameter2_weight1_diff=$( awk 'BEGIN{printf ("%.3f", '$parameter2_weight1'-'$game1_parameter2_weight')}' )
parameter3_weight1=$( jq -r  '.[2]' <<< "${weights1}" )
parameter3_weight1=($parameter3_weight1)
parameter3_weight1=${parameter3_weight1[0]}
parameter3_weight1_diff=$( awk 'BEGIN{printf ("%.3f", '$parameter3_weight1'-'$game1_parameter3_weight')}' )
parameter4_weight1=$( jq -r  '.[3]' <<< "${weights1}" )
parameter4_weight1=($parameter4_weight1)
parameter4_weight1=${parameter4_weight1[0]}
parameter4_weight1_diff=$( awk 'BEGIN{printf ("%.3f", '$parameter4_weight1'-'$game1_parameter4_weight')}' )

id2=$( jq -r  '.[1].id' <<< "${body}" )
id2=($id2)
id2=${id2[0]}
id2=$((id2))
name2=$( jq -r  '.[1].name' <<< "${body}" )
name2=($name2)
name2=${name2[1]}
category2=$( jq -r  '.[1].category' <<< "${body}" )
category2=($category2)
category2=${category2[1]}
players_per_team2=$( jq -r  '.[1].players_per_team' <<< "${body}" )
players_per_team2=($players_per_team2)
players_per_team2=${players_per_team2[1]}
teams_per_match2=$( jq -r  '.[1].teams_per_match' <<< "${body}" )
teams_per_match2=($teams_per_match2)
teams_per_match2=${teams_per_match2[1]}
parameters2=$( jq -r  '.[1].parameters' <<< "${body}" )
parameter1_name2=$( jq -r  '.[0]' <<< "${parameters2}" )
parameter1_name2=($parameter1_name2)
parameter1_name2=${parameter1_name2[1]}
parameter2_name2=$( jq -r  '.[1]' <<< "${parameters2}" )
parameter2_name2=($parameter2_name2)
parameter2_name2=${parameter2_name2[1]}
parameter3_name2=$( jq -r  '.[2]' <<< "${parameters2}" )
parameter3_name2=($parameter3_name2)
parameter3_name2=${parameter3_name2[1]}
parameter4_name2=$( jq -r  '.[3]' <<< "${parameters2}" )
parameter4_name2=($parameter4_name2)
parameter4_name2=${parameter4_name2[1]}
weights2=$( jq -r  '.[1].weights' <<< "${body}" )
parameter1_weight2=$( jq -r  '.[0]' <<< "${weights2}" )
parameter1_weight2=($parameter1_weight2)
parameter1_weight2=${parameter1_weight2[1]}
parameter1_weight2_diff=$( awk 'BEGIN{printf ("%.3f", '$parameter1_weight2'-'$game2_parameter1_weight')}' )
parameter2_weight2=$( jq -r  '.[1]' <<< "${weights2}" )
parameter2_weight2=($parameter2_weight2)
parameter2_weight2=${parameter2_weight2[1]}
parameter2_weight2_diff=$( awk 'BEGIN{printf ("%.3f", '$parameter2_weight2'-'$game2_parameter2_weight')}' )
parameter3_weight2=$( jq -r  '.[2]' <<< "${weights2}" )
parameter3_weight2=($parameter3_weight2)
parameter3_weight2=${parameter3_weight2[1]}
parameter3_weight2_diff=$( awk 'BEGIN{printf ("%.3f", '$parameter3_weight2'-'$game2_parameter3_weight')}' )
parameter4_weight2=$( jq -r  '.[3]' <<< "${weights2}" )
parameter4_weight2=($parameter4_weight2)
parameter4_weight2=${parameter4_weight2[0]}
parameter4_weight2_diff=$( awk 'BEGIN{printf ("%.3f", '$parameter4_weight2'-'$game2_parameter4_weight')}' )

# TEST: Developer 1 should have two games
if [ $code -eq 200 ]; then
    if [[ $body_length -ne 2 ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR (DeveloperOwnsTwoGamesTest): Developer 1 should have 2 games, but has " $body_length
    elif ([[ $id1 -eq $game1_id ]] && [[ $id2 -eq $game2_id ]]); then
        if [[ $name1 != $game1_name ]]; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game1 has incorrect name. Expected" $game1_name", but received" $name1
        fi
        if [[ $category1 != $game1_category ]]; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game1 has incorrect category. Expected" $game1_category", but received" $category1
        fi
        if [[ $players_per_team1 -ne $game1_players_per_team ]]; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game1 has incorrect players per team. Expected" $game1_players_per_team", but received" $players_per_team1
        fi
        if [[ $teams_per_match1 -ne $game1_teams_per_match ]]; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game1 has incorrect teams per match. Expected" $game1_teams_per_match", but received" $teams_per_match1
        fi
        if [[ $parameter1_name1 != $game1_parameter1_name ]]; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game1 has incorrect parameter1 name. Expected" $game1_parameter1_name", but received" $parameter1_name1
        fi
        if [[ $parameter2_name1 != $game1_parameter2_name ]]; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game1 has incorrect parameter2 name. Expected" $game1_parameter2_name", but received" $parameter2_name1
        fi
        if [[ $parameter3_name1 != $game1_parameter3_name ]]; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game1 has incorrect parameter3 name. Expected" $game1_parameter3_name", but received" $parameter3_name1
        fi
        if [[ $parameter4_name1 != $game1_parameter4_name ]]; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game1 has incorrect parameter4 name. Expected" $game1_parameter4_name", but received" $parameter4_name1
        fi
        if awk "BEGIN {exit !($parameter1_weight1_diff >= 0.05)}"; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game1 has incorrect parameter1 weight. Expected" $game1_parameter1_weight", but received" $parameter1_weight1
        fi
        if awk "BEGIN {exit !($parameter2_weight1_diff >= 0.05)}"; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game1 has incorrect parameter2 weight. Expected" $game1_parameter2_weight", but received" $parameter2_weight1
        fi
        if awk "BEGIN {exit !($parameter3_weight1_diff >= 0.05)}"; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game1 has incorrect parameter3 weight. Expected" $game1_parameter3_weight", but received" $parameter3_weight1
        fi
        if awk "BEGIN {exit !($parameter4_weight1_diff >= 0.05)}"; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game1 has incorrect parameter4 weight. Expected" $game1_parameter4_weight", but received" $parameter4_weight1
        fi
    elif ([[ $id1 -eq $game2_id ]] && [[ $id2 -eq $game1_id ]]); then
        if [[ $name2 != $game2_name ]]; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game2 has incorrect name. Expected" $game2_name", but received" $name2
        fi
        if [[ $category2 != $game2_category ]]; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game2 has incorrect category. Expected" $game2_category", but received" $category2
        fi
        if [[ $players_per_team2 -ne $game2_players_per_team ]]; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game2 has incorrect players per team. Expected" $game2_players_per_team", but received" $players_per_team2
        fi
        if [[ $teams_per_match2 -ne $game2_teams_per_match ]]; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game2 has incorrect teams per match. Expected" $game2_teams_per_match", but received" $teams_per_match2
        fi
        if [[ $parameter1_name2 != $game2_parameter1_name ]]; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game2 has incorrect parameter1 name. Expected" $game2_parameter1_name", but received" $parameter1_name2
        fi
        if [[ $parameter2_name2 != $game2_parameter2_name ]]; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game2 has incorrect parameter2 name. Expected" $game2_parameter2_name", but received" $parameter2_name2
        fi
        if [[ $parameter3_name2 != $game2_parameter3_name ]]; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game2 has incorrect parameter3 name. Expected" $game2_parameter3_name", but received" $parameter3_name2
        fi
        if [[ $parameter4_name2 != $game2_parameter4_name ]]; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game2 has incorrect parameter4 name. Expected" $game2_parameter4_name", but received" $parameter4_name2
        fi
        if awk "BEGIN {exit !($parameter1_weight2_diff >= 0.05)}"; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game2 has incorrect parameter1 weight. Expected" $game2_parameter1_weight", but received" $parameter1_weight2
        fi
        if awk "BEGIN {exit !($parameter2_weight2_diff >= 0.05)}"; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game2 has incorrect parameter2 weight. Expected" $game2_parameter2_weight", but received" $parameter2_weight2
        fi
        if awk "BEGIN {exit !($parameter3_weight2_diff >= 0.05)}"; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game2 has incorrect parameter3 weight. Expected" $game2_parameter3_weight", but received" $parameter3_weight2
        fi
        if awk "BEGIN {exit !($parameter4_weight2_diff >= 0.05)}"; then
            num_errors=$((num_errors+1))
            echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s game2 has incorrect parameter4 weight. Expected" $game2_parameter4_weight", but received" $parameter4_weight2
        fi
    else 
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1 is not being given correct games"
    fi
else
    num_errors=$((num_errors+1))
    echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1 did not get their two games (Error code " $code")"
fi

# Developer 2 gets their game
status=$(
    curl -s -w "%{stdout}{\"code\": %{http_code}}" \
        -X GET ${URL}/games \
        -H 'Accept: application/json' \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token2}"
)
echo $status
code=$( jq -r  '.code' <<< "${status}" )
code=($code)
code=${code[1]}
body=$( jq -r  '.games' <<< "${status}" )
body_length=$( jq -r 'length' <<< ${body} )
body_length=$( echo ${body_length} | awk '{print $1;}' )
id1=$( jq -r  '.[0].id' <<< "${body}" )
id1=($id1)
id1=${id1[0]}
id1=$((id1))
name1=$( jq -r  '.[0].name' <<< "${body}" )
name1=($name1)
name1=${name1[0]}
category1=$( jq -r  '.[0].category' <<< "${body}" )
category1=($category1)
category1=${category1[0]}
players_per_team1=$( jq -r  '.[0].players_per_team' <<< "${body}" )
players_per_team1=($players_per_team1)
players_per_team1=${players_per_team1[0]}
teams_per_match1=$( jq -r  '.[0].teams_per_match' <<< "${body}" )
teams_per_match1=($teams_per_match1)
teams_per_match1=${teams_per_match1[0]}
parameters1=$( jq -r  '.[0].parameters' <<< "${body}" )
parameter1_name1=$( jq -r  '.[0]' <<< "${parameters1}" )
parameter1_name1=($parameter1_name1)
parameter1_name1=${parameter1_name1[0]}
parameter2_name1=$( jq -r  '.[1]' <<< "${parameters1}" )
parameter2_name1=($parameter2_name1)
parameter2_name1=${parameter2_name1[0]}
parameter3_name1=$( jq -r  '.[2]' <<< "${parameters1}" )
parameter3_name1=($parameter3_name1)
parameter3_name1=${parameter3_name1[0]}
parameter4_name1=$( jq -r  '.[3]' <<< "${parameters1}" )
parameter4_name1=($parameter4_name1)
parameter4_name1=${parameter4_name1[0]}
weights1=$( jq -r '.[0].weights' <<< "${body}" )
parameter1_weight1=$( jq -r  '.[0]' <<< "${weights1}" )
parameter1_weight1=($parameter1_weight1)
parameter1_weight1=${parameter1_weight1[0]}
parameter1_weight1_diff=$( awk 'BEGIN{printf ("%.3f", '$parameter1_weight1'-'$game3_parameter1_weight')}' )
parameter2_weight1=$( jq -r  '.[1]' <<< "${weights1}" )
parameter2_weight1=($parameter2_weight1)
parameter2_weight1=${parameter2_weight1[0]}
parameter2_weight1_diff=$( awk 'BEGIN{printf ("%.3f", '$parameter2_weight1'-'$game3_parameter2_weight')}' )
parameter3_weight1=$( jq -r  '.[2]' <<< "${weights1}" )
parameter3_weight1=($parameter3_weight1)
parameter3_weight1=${parameter3_weight1[0]}
parameter3_weight1_diff=$( awk 'BEGIN{printf ("%.3f", '$parameter3_weight1'-'$game3_parameter3_weight')}' )
parameter4_weight1=$( jq -r  '.[3]' <<< "${weights1}" )
parameter4_weight1=($parameter4_weight1)
parameter4_weight1=${parameter4_weight1[0]}
parameter4_weight1_diff=$( awk 'BEGIN{printf ("%.3f", '$parameter4_weight1'-'$game3_parameter4_weight')}' )

# TEST: Developer 2 should have 1 game
if [ $code -eq 200 ]; then
    if [[ $body_length -ne 1 ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR (DeveloperOwnsOneGameTest): Developer 2 should have 1 game, but has " $body_length
    fi
    if [[ ${id1} -ne ${game3_id} ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 2s game has incorrect id. Expected" $game3_id", but received" $id1
    fi
    if [[ $name1 != $game3_name ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 2s game has incorrect name. Expected" $game3_name", but received" $name1
    fi
    if [[ $category1 != $game3_category ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 2s game has incorrect category. Expected" $game3_category", but received" $category1
    fi
    if [[ $players_per_team1 -ne $game3_players_per_team ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 2s game has incorrect players per team. Expected" $game3_players_per_team", but received" $players_per_team1
    fi
    if [[ $teams_per_match1 -ne $game3_teams_per_match ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 2s game has incorrect teams per match. Expected" $game3_teams_per_match", but received" $teams_per_match1
    fi
    if [[ $parameter1_name1 != $game3_parameter1_name ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 2s game has incorrect parameter1 name. Expected" $game3_parameter1_name", but received" $parameter1_name1
    fi
    if [[ $parameter2_name1 != $game3_parameter2_name ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 2s game has incorrect parameter2 name. Expected" $game3_parameter2_name", but received" $parameter2_name1
    fi
    if [[ $parameter3_name1 != "null" ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 2s game has incorrect parameter3 name. Expected" $game3_parameter3_name", but received" $parameter3_name1
    fi
    if [[ $parameter4_name1 != "null" ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 2s game has incorrect parameter4 name. Expected" $game3_parameter4_name", but received" $parameter4_name1
    fi
    if awk "BEGIN {exit !($parameter1_weight1_diff >= 0.05)}"; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 2s game has incorrect parameter1 weight. Expected" $game3_parameter1_weight", but received" $parameter1_weight1
    fi
    if awk "BEGIN {exit !($parameter2_weight1_diff >= 0.05)}"; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 2s game has incorrect parameter2 weight. Expected" $game3_parameter2_weight", but received" $parameter2_weight1
    fi
else
    num_errors=$((num_errors+1))
    echo "ERROR (DeveloperOwnsOneGameTest): Developer 2 did not get their game (Error code " $code")"
fi

echo "Developer updates one of their games..."

# # Developer 1 updates one of their games
game1_parameter3_name="livesRemaining"
game1_parameter4_name="energyRemaining"
game1_parameter3_weight=0.11
game1_parameter4_weight=0.09

status=$(
    curl -s -X PUT ${URL}/games/${game1_id} \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}" \
        -d '{
            "game_parameter3_name": "'${game1_parameter3_name}'",
            "game_parameter4_name": "'${game1_parameter4_name}'",
            "game_parameter3_weight": '${game1_parameter3_weight}',
            "game_parameter4_weight": '${game1_parameter4_weight}'
        }'
)

echo "Developer confirms their game has been updated ..."

# Developer 1 gets their game to confirm their game has been updated properly
status=$(
    curl -s -w "%{stdout}{\"code\": %{http_code}}" \
        -X GET ${URL}/games/${game1_id} \
        -H 'Accept: application/json' \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}"
)
echo $status
code=$( jq -r  '.code' <<< "${status}" )
code=($code)
code=${code[1]}
id1=$( jq -r  '.game_id' <<< "${status}" )
id1=($id1)
id1=${id1[0]}
id1=$((id1))
name1=$( jq -r  '.game_name' <<< "${status}" )
name1=($name1)
name1=${name1[0]}
category1=$( jq -r  '.category' <<< "${status}" )
category1=($category1)
category1=${category1[0]}
players_per_team1=$( jq -r  '.players_per_team' <<< "${status}" )
players_per_team1=($players_per_team1)
players_per_team1=${players_per_team1[0]}
teams_per_match1=$( jq -r  '.teams_per_match' <<< "${status}" )
teams_per_match1=($teams_per_match1)
teams_per_match1=${teams_per_match1[0]}
parameter1_name1=$( jq -r  '.game_parameter1_name' <<< "${status}" )
parameter1_name1=($parameter1_name1)
parameter1_name1=${parameter1_name1[0]}
parameter2_name1=$( jq -r  '.game_parameter2_name' <<< "${status}" )
parameter2_name1=($parameter2_name1)
parameter2_name1=${parameter2_name1[0]}
parameter3_name1=$( jq -r  '.game_parameter3_name' <<< "${status}" )
parameter3_name1=($parameter3_name1)
parameter3_name1=${parameter3_name1[0]}
parameter4_name1=$( jq -r  '.game_parameter4_name' <<< "${status}" )
parameter4_name1=($parameter4_name1)
parameter4_name1=${parameter4_name1[0]}
parameter1_weight1=$( jq -r  '.game_parameter1_weight' <<< "${status}" )
parameter1_weight1=($parameter1_weight1)
parameter1_weight1=${parameter1_weight1[0]}
parameter1_weight1_diff=$( awk 'BEGIN{printf ("%.3f", '$parameter1_weight1'-'$game1_parameter1_weight')}' )
parameter2_weight1=$( jq -r  '.game_parameter2_weight' <<< "${status}" )
parameter2_weight1=($parameter2_weight1)
parameter2_weight1=${parameter2_weight1[0]}
parameter2_weight1_diff=$( awk 'BEGIN{printf ("%.3f", '$parameter2_weight1'-'$game1_parameter2_weight')}' )
parameter3_weight1=$( jq -r  '.game_parameter3_weight' <<< "${status}" )
parameter3_weight1=($parameter3_weight1)
parameter3_weight1=${parameter3_weight1[0]}
parameter3_weight1_diff=$( awk 'BEGIN{printf ("%.3f", '$parameter3_weight1'-'$game1_parameter3_weight')}' )
parameter4_weight1=$( jq -r  '.game_parameter4_weight' <<< "${status}" )
parameter4_weight1=($parameter4_weight1)
parameter4_weight1=${parameter4_weight1[0]}
parameter4_weight1_diff=$( awk 'BEGIN{printf ("%.3f", '$parameter4_weight1'-'$game1_parameter4_weight')}' )

# TEST: Developer 1s game should have been updated correctly
if [ $code -eq 200 ]; then
    if [[ $id1 -ne $game1_id ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR (DeveloperUpdatedGameTest): Developer 1s updated game has incorrect id. Expected" $game1_id", but received" $id1
    fi
    if [[ $name1 != $game1_name ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s updated game has incorrect name. Expected" $game1_name", but received" $name1
    fi
    if [[ $category1 != $game1_category ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s updated game has incorrect category. Expected" $game1_category", but received" $category1
    fi
    if [[ $players_per_team1 -ne $game1_players_per_team ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s updated game has incorrect players per team. Expected" $game1_players_per_team", but received" $players_per_team1
    fi
    if [[ $teams_per_match1 -ne $game1_teams_per_match ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s updated game has incorrect teams per match. Expected" $game1_teams_per_match", but received" $teams_per_match1
    fi
    if [[ $parameter1_name1 != $game1_parameter1_name ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s updated game has incorrect parameter1 name. Expected" $game1_parameter1_name", but received" $parameter1_name1
    fi
    if [[ $parameter2_name1 != $game1_parameter2_name ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s updated game has incorrect parameter2 name. Expected" $game1_parameter2_name", but received" $parameter2_name1
    fi
    if [[ $parameter3_name1 != $game1_parameter3_name ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s updated game has incorrect parameter3 name. Expected" $game1_parameter3_name", but received" $parameter3_name1
    fi
    if [[ $parameter4_name1 != $game1_parameter4_name ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s updated game has incorrect parameter4 name. Expected" $game1_parameter4_name", but received" $parameter4_name1
    fi
    if awk "BEGIN {exit !($parameter1_weight1_diff >= 0.05)}"; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s updated game has incorrect parameter1 weight. Expected" $game1_parameter1_weight", but received" $parameter1_weight1
    fi
    if awk "BEGIN {exit !($parameter2_weight1_diff >= 0.05)}"; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s updated game has incorrect parameter2 weight. Expected" $game1_parameter2_weight", but received" $parameter2_weight1
    fi
    if awk "BEGIN {exit !($parameter3_weight1_diff >= 0.05)}"; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s updated game has incorrect parameter3 weight. Expected" $game1_parameter3_weight", but received" $parameter3_weight1
    fi
    if awk "BEGIN {exit !($parameter4_weight1_diff >= 0.05)}"; then
        num_errors=$((num_errors+1))
        echo "ERROR  (DeveloperOwnsTwoGamesTest): Developer 1s updated game has incorrect parameter4 weight. Expected" $game1_parameter4_weight", but received" $parameter4_weight1
    fi
else
    num_errors=$((num_errors+1))
    echo "ERROR (DeveloperUpdatedGameTest): Developer 1 did not update their game correctly (Error code " $code")"
fi

# -------------------------------------------------------
# GETTING, ADDING, AND UPDATING PLAYER STATS
# -------------------------------------------------------

echo "Developer makes sure they don't own any players..."

# Developer 1 is getting all of their players for their game (no players should exist)
status=$(
    curl -s -w "%{http_code}" -X GET ${URL}/games/${game1_id}/players \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}"
)

# TEST: No players should exist for this game
if [ $status != 204 ]; then
    num_errors=$((num_errors+1))
    echo "ERROR (GameDoesntHavePlayersTest): There should be no players for this game with game_id ${game1_id}"
fi

echo "Developer 1 adds players..."

# Developer 1 adds players
status=$(
    curl -s -w "%{http_code}" -X POST ${URL}/games/${game1_id}/players \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}" \
        -d '{
            "'${player1_email}'": {
                "game_parameter1_value": '${player1_game_parameter1_value}',
                "game_parameter2_value": '${player1_game_parameter2_value}',
                "game_parameter3_value": '${player1_game_parameter3_value}',
                "game_parameter4_value": '${player1_game_parameter4_value}'
            },
            "'${player2_email}'": {
                "game_parameter1_value": '${player2_game_parameter1_value}',
                "game_parameter2_value": '${player2_game_parameter2_value}',
                "game_parameter3_value": '${player2_game_parameter3_value}',
                "game_parameter4_value": '${player2_game_parameter4_value}'
            },
            "'${player3_email}'": {
                "game_parameter1_value": '${player3_game_parameter1_value}',
                "game_parameter2_value": '${player3_game_parameter2_value}',
                "game_parameter3_value": '${player3_game_parameter3_value}',
                "game_parameter4_value": '${player3_game_parameter4_value}'
            },
            "'${player4_email}'": {
                "game_parameter1_value": '${player4_game_parameter1_value}',
                "game_parameter2_value": '${player4_game_parameter2_value}',
                "game_parameter3_value": '${player4_game_parameter3_value}',
                "game_parameter4_value": '${player4_game_parameter4_value}'
            },
            "'${player5_email}'": {
                "game_parameter1_value": '${player5_game_parameter1_value}',
                "game_parameter2_value": '${player5_game_parameter2_value}',
                "game_parameter3_value": '${player5_game_parameter3_value}',
                "game_parameter4_value": '${player5_game_parameter4_value}'
            },
            "'${player6_email}'": {
                "game_parameter1_value": '${player6_game_parameter1_value}',
                "game_parameter2_value": '${player6_game_parameter2_value}',
                "game_parameter3_value": '${player6_game_parameter3_value}',
                "game_parameter4_value": '${player6_game_parameter4_value}'
            },
            "'${player7_email}'": {
                "game_parameter1_value": '${player7_game_parameter1_value}',
                "game_parameter2_value": '${player7_game_parameter2_value}',
                "game_parameter3_value": '${player7_game_parameter3_value}',
                "game_parameter4_value": '${player7_game_parameter4_value}'
            }
        }'
)

echo "Developer 1 gets their players to confirm they were added correctly..."

# Developer 1 is getting all of their players for their game
status=$(
    curl -s -X GET ${URL}/games/${game1_id}/players \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}"
)
echo $status
keys=$( jq 'keys | .[]' <<< "${status}" )
num_players=$( jq -r 'length' <<< ${status} )
num_players=$( echo ${num_players} | awk '{print $1;}' )

# TEST: Developer 1 should have 7 players for game SuperSmashBros
missing_players=""
if [[ $num_players -ne ${#players[@]} ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR  (AddedPlayersCorrectlyTest): Developer 1 has incorrect number of players for game1. Expected" ${#players[@]}", but received" $num_players
else
    for k in $keys
    do
        if [[ ! ("${keys[*]}" =~ $p) ]]; then
            if [[ -z $missing_players ]]; then
                missing_players="${p}"
            else
                missing_players="${missing_players}, ${p}"
            fi
        else
            key_game_parameter1_value=$( jq -r  '.['${k}'].'${game1_parameter1_name}'' <<< "${status}" )
            key_game_parameter2_value=$( jq -r  '.['${k}'].'${game1_parameter2_name}'' <<< "${status}" )
            key_game_parameter3_value=$( jq -r  '.['${k}'].'${game1_parameter3_name}'' <<< "${status}" )
            key_game_parameter4_value=$( jq -r  '.['${k}'].'${game1_parameter4_name}'' <<< "${status}" )

            if [[ $(echo $k | tr -d '"') == "${player1_email}" ]]; then
                player_game_parameter1_value=$player1_game_parameter1_value
                player_game_parameter2_value=$player1_game_parameter2_value
                player_game_parameter3_value=$player1_game_parameter3_value
                player_game_parameter4_value=$player1_game_parameter4_value
            elif [[ $(echo $k | tr -d '"') == "${player2_email}" ]]; then
                player_game_parameter1_value=$player2_game_parameter1_value
                player_game_parameter2_value=$player2_game_parameter2_value
                player_game_parameter3_value=$player2_game_parameter3_value
                player_game_parameter4_value=$player2_game_parameter4_value
            elif [[ $(echo $k | tr -d '"') == "${player3_email}" ]]; then
                player_game_parameter1_value=$player3_game_parameter1_value
                player_game_parameter2_value=$player3_game_parameter2_value
                player_game_parameter3_value=$player3_game_parameter3_value
                player_game_parameter4_value=$player3_game_parameter4_value
            elif [[ $(echo $k | tr -d '"') == "${player4_email}" ]]; then
                player_game_parameter1_value=$player4_game_parameter1_value
                player_game_parameter2_value=$player4_game_parameter2_value
                player_game_parameter3_value=$player4_game_parameter3_value
                player_game_parameter4_value=$player4_game_parameter4_value
            elif [[ $(echo $k | tr -d '"') == "${player5_email}" ]]; then
                player_game_parameter1_value=$player5_game_parameter1_value
                player_game_parameter2_value=$player5_game_parameter2_value
                player_game_parameter3_value=$player5_game_parameter3_value
                player_game_parameter4_value=$player5_game_parameter4_value
            elif [[ $(echo $k | tr -d '"') == "${player6_email}" ]]; then
                player_game_parameter1_value=$player6_game_parameter1_value
                player_game_parameter2_value=$player6_game_parameter2_value
                player_game_parameter3_value=$player6_game_parameter3_value
                player_game_parameter4_value=$player6_game_parameter4_value
            elif [[ $(echo $k | tr -d '"') == "${player7_email}" ]]; then
                player_game_parameter1_value=$player7_game_parameter1_value
                player_game_parameter2_value=$player7_game_parameter2_value
                player_game_parameter3_value=$player7_game_parameter3_value
                player_game_parameter4_value=$player7_game_parameter4_value
            fi

            if [[ $key_game_parameter1_value -ne $player_game_parameter1_value ]]; then
                num_errors=$((num_errors+1))
                echo "ERROR  (AddedPlayersCorrectlyTest): Developer 1 has incorrect parameter1 value for player" $k". Expected" $player_game_parameter1_value", but received" $key_game_parameter1_value
            fi
            if [[ $key_game_parameter2_value -ne $player_game_parameter2_value ]]; then
                num_errors=$((num_errors+1))
                echo "ERROR  (AddedPlayersCorrectlyTest): Developer 1 has incorrect parameter2 value for player" $k". Expected" $player_game_parameter2_value", but received" $key_game_parameter2_value
            fi
            if [[ $key_game_parameter3_value -ne $player_game_parameter3_value ]]; then
                num_errors=$((num_errors+1))
                echo "ERROR  (AddedPlayersCorrectlyTest): Developer 1 has incorrect parameter3 value for player" $k". Expected" $player_game_parameter3_value", but received" $key_game_parameter3_value
            fi
            if [[ $key_game_parameter4_value -ne $player_game_parameter4_value ]]; then
                num_errors=$((num_errors+1))
                echo "ERROR  (AddedPlayersCorrectlyTest): Developer 1 has incorrect parameter4 value for player" $k". Expected" $player_game_parameter4_value", but received" $key_game_parameter4_value
            fi
        fi
    done

    if [[ -n $missing_players ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR  (AddedPlayersCorrectlyTest): Developer 1 should have the following players, but they are not found in the database:" $missing_players
    fi
fi

echo "Developer updates player stats..."

# # Developer 1 updates players stats for a game (and adds stats for new players)
player1_game_parameter1_value=25
player2_game_parameter1_value=25

status=$(
    curl -s -X PUT ${URL}/games/${game1_id}/players/${player1_email} \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}" \
        -d '{
            "game_parameter1_value": '${player1_game_parameter1_value}',
            "game_parameter2_value": '${player1_game_parameter2_value}',
            "game_parameter3_value": '${player1_game_parameter3_value}',
            "game_parameter4_value": '${player1_game_parameter4_value}'
        }'
)

status=$(
    curl -s -X PUT ${URL}/games/${game1_id}/players/${player2_email} \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}" \
        -d '{
            "game_parameter1_value": '${player2_game_parameter1_value}',
            "game_parameter2_value": '${player2_game_parameter2_value}',
            "game_parameter3_value": '${player2_game_parameter3_value}',
            "game_parameter4_value": '${player2_game_parameter4_value}'
        }'
)

echo "Developer confirms it cannot add players that don't exist..."

status=$(
    curl -s -X PUT ${URL}/games/${game1_id}/players/${player8_email} \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}" \
        -d '{
            "game_parameter1_value": '${player8_game_parameter1_value}',
            "game_parameter2_value": '${player8_game_parameter2_value}',
            "game_parameter3_value": '${player8_game_parameter3_value}',
            "game_parameter4_value": '${player8_game_parameter4_value}'
        }'
)
echo $status

if [[ "$status" != "The given player does not have ratings for this game" ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (UpdatedPlayerThatDoesntExistTest): Developer should not be able to update player that doesnt exist"
fi

echo "Developer 1 adds players and confirms their previously updated players were updated correctly..."

status=$(
    curl -s -X POST ${URL}/games/${game1_id}/players \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}" \
        -d '{
            "'${player8_email}'": {
                "game_parameter1_value": '${player8_game_parameter1_value}',
                "game_parameter2_value": '${player8_game_parameter2_value}',
                "game_parameter3_value": '${player8_game_parameter3_value}',
                "game_parameter4_value": '${player8_game_parameter4_value}'
            },
            "'${player9_email}'": {
                "game_parameter1_value": '${player9_game_parameter1_value}',
                "game_parameter2_value": '${player9_game_parameter2_value}',
                "game_parameter3_value": '${player9_game_parameter3_value}',
                "game_parameter4_value": '${player9_game_parameter4_value}'
            }
        }'
)

players=(${players[@]} "${player8_email}" "${player9_email}")

# Developer 1 is getting all of their players for their game
status=$(
    curl -s -X GET ${URL}/games/${game1_id}/players \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}"
)
echo $status
keys=$( jq 'keys | .[]' <<< "${status}" )
num_players=$( jq -r 'length' <<< ${status} )
num_players=$( echo ${num_players} | awk '{print $1;}' )

# TEST: Developer 1 should have updated stats for players
missing_players=""
if [[ $num_players -ne ${#players[@]} ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR  (UpdatedPlayerStatsTest): Developer 1 has incorrect number of players for game1. Expected" ${#players[@]}", but received" $num_players
else
    for k in $keys
    do
        if [[ ! ("${keys[*]}" =~ $p) ]]; then
            if [[ -z $missing_players ]]; then
                missing_players="${p}"
            else
                missing_players="${missing_players}, ${p}"
            fi
        else
            key_game_parameter1_value=$( jq -r  '.['${k}'].'${game1_parameter1_name}'' <<< "${status}" )
            key_game_parameter2_value=$( jq -r  '.['${k}'].'${game1_parameter2_name}'' <<< "${status}" )
            key_game_parameter3_value=$( jq -r  '.['${k}'].'${game1_parameter3_name}'' <<< "${status}" )
            key_game_parameter4_value=$( jq -r  '.['${k}'].'${game1_parameter4_name}'' <<< "${status}" )

            if [[ $(echo $k | tr -d '"') == "${player1_email}" ]]; then
                player_game_parameter1_value=$player1_game_parameter1_value
                player_game_parameter2_value=$player1_game_parameter2_value
                player_game_parameter3_value=$player1_game_parameter3_value
                player_game_parameter4_value=$player1_game_parameter4_value
            elif [[ $(echo $k | tr -d '"') == "${player2_email}" ]]; then
                player_game_parameter1_value=$player2_game_parameter1_value
                player_game_parameter2_value=$player2_game_parameter2_value
                player_game_parameter3_value=$player2_game_parameter3_value
                player_game_parameter4_value=$player2_game_parameter4_value
            elif [[ $(echo $k | tr -d '"') == "${player3_email}" ]]; then
                player_game_parameter1_value=$player3_game_parameter1_value
                player_game_parameter2_value=$player3_game_parameter2_value
                player_game_parameter3_value=$player3_game_parameter3_value
                player_game_parameter4_value=$player3_game_parameter4_value
            elif [[ $(echo $k | tr -d '"') == "${player4_email}" ]]; then
                player_game_parameter1_value=$player4_game_parameter1_value
                player_game_parameter2_value=$player4_game_parameter2_value
                player_game_parameter3_value=$player4_game_parameter3_value
                player_game_parameter4_value=$player4_game_parameter4_value
            elif [[ $(echo $k | tr -d '"') == "${player5_email}" ]]; then
                player_game_parameter1_value=$player5_game_parameter1_value
                player_game_parameter2_value=$player5_game_parameter2_value
                player_game_parameter3_value=$player5_game_parameter3_value
                player_game_parameter4_value=$player5_game_parameter4_value
            elif [[ $(echo $k | tr -d '"') == "${player6_email}" ]]; then
                player_game_parameter1_value=$player6_game_parameter1_value
                player_game_parameter2_value=$player6_game_parameter2_value
                player_game_parameter3_value=$player6_game_parameter3_value
                player_game_parameter4_value=$player6_game_parameter4_value
            elif [[ $(echo $k | tr -d '"') == "${player7_email}" ]]; then
                player_game_parameter1_value=$player7_game_parameter1_value
                player_game_parameter2_value=$player7_game_parameter2_value
                player_game_parameter3_value=$player7_game_parameter3_value
                player_game_parameter4_value=$player7_game_parameter4_value
            elif [[ $(echo $k | tr -d '"') == "${player8_email}" ]]; then
                player_game_parameter1_value=$player8_game_parameter1_value
                player_game_parameter2_value=$player8_game_parameter2_value
                player_game_parameter3_value=$player8_game_parameter3_value
                player_game_parameter4_value=$player8_game_parameter4_value
            elif [[ $(echo $k | tr -d '"') == "${player9_email}" ]]; then
                player_game_parameter1_value=$player9_game_parameter1_value
                player_game_parameter2_value=$player9_game_parameter2_value
                player_game_parameter3_value=$player9_game_parameter3_value
                player_game_parameter4_value=$player9_game_parameter4_value
            fi

            if [[ $key_game_parameter1_value -ne $player_game_parameter1_value ]]; then
                num_errors=$((num_errors+1))
                echo "ERROR  (UpdatedPlayerStatsTest): Developer 1 has incorrect parameter1 value for player" $k". Expected" $player_game_parameter1_value", but received" $key_game_parameter1_value
            fi
            if [[ $key_game_parameter2_value -ne $player_game_parameter2_value ]]; then
                num_errors=$((num_errors+1))
                echo "ERROR  (UpdatedPlayerStatsTest): Developer 1 has incorrect parameter2 value for player" $k". Expected" $player_game_parameter2_value", but received" $key_game_parameter2_value
            fi
            if [[ $key_game_parameter3_value -ne $player_game_parameter3_value ]]; then
                num_errors=$((num_errors+1))
                echo "ERROR  (UpdatedPlayerStatsTest): Developer 1 has incorrect parameter3 value for player" $k". Expected" $player_game_parameter3_value", but received" $key_game_parameter3_value
            fi
            if [[ $key_game_parameter4_value -ne $player_game_parameter4_value ]]; then
                num_errors=$((num_errors+1))
                echo "ERROR  (UpdatedPlayerStatsTest): Developer 1 has incorrect parameter4 value for player" $k". Expected" $player_game_parameter4_value", but received" $key_game_parameter4_value
            fi
        fi
    done

    if [[ -n $missing_players ]]; then
        num_errors=$((num_errors+1))
        echo "ERROR  (UpdatedPlayerStatsTest): Developer 1 should have the following players, but they are not found in the database:" $missing_players
    fi
fi

echo "Developer deletes a player..."

# Developer 1 is deleting 1 player
status=$(
    curl -s -X DELETE ${URL}/games/${game1_id}/players/${player2_email} \
        -H "Authorization: ${token1}" \
)
echo $status

echo "Developer tries to access the player it just deleted..."

# Developer 1 is trying to access the player it just deleted
status=$(
    curl -s -w "%{http_code}" -X GET ${URL}/games/${game1_id}/players/${player2_email} \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}" \
        -d '{
            "player_emails": [
                "'${player2_email}'"
            ]
        }'
)
echo $status

# TEST: Developer 1 should not be able to access this player
if [[ $status -ne 204 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (AccessingDeletedPlayerTest): Player stats for deleted player ${player2_email} should not be accessed"
fi

delete=$player2_email
players=( "${players[@]/$delete}" )

# -------------------------------------------------------
# MATCHMAKING
# -------------------------------------------------------

echo "Performing basic matchmaking..."

# Perform basic matchmaking
player_ranked1=${player7_email} # 0.6(50) + 0.2(1) + 0.11(0) + 0.09(0) = 30.2
player_ranked2=${player3_email} # 0.6(40) + 0.2(3) + 0.11(1) + 0.09(2) = 24.89
player_ranked3=${player6_email} # 0.6(40) + 0.2(2) + 0.11(1) + 0.09(0) = 24.51
player_ranked4=${player4_email} # 0.6(30) + 0.2(6) + 0.11(4) + 0.09(1) = 19.73
player_ranked5=${player8_email} # 0.6(32) + 0.2(2) + 0.11(0) + 0.09(1) = 19.69
player_ranked6=${player1_email} # 0.6(30) + 0.2(2) + 0.11(1) + 0.09(0) = 18.51
player_ranked7=${player9_email} # 0.6(20) + 0.2(3) + 0.11(1) + 0.09(1) = 12.8
player_ranked8=${player5_email} # 0.6(10) + 0.2(10) + 0.11(1) + 0.09(1) = 8.2

status=$(
    curl -s -w "%{stdout}{\"code\": %{http_code}}" -X POST ${URL}/matchmake \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}" \
        -d '{
            "game_id": '${game1_id}',
            "player_emails": [
                "'${player1_email}'", "'${player3_email}'", "'${player4_email}'", "'${player5_email}'",
                "'${player6_email}'", "'${player7_email}'", "'${player8_email}'", "'${player9_email}'"
            ],
            "matchmaking_type": "basic"
        }'
)
echo $status
code=$( jq -r '.code' <<< ${status} )
code=($code)
code=${code[1]}
player_team1=$( jq -r '.Game_1.Team_1 | .[0]' <<< ${status} )
player_team1=($player_team1)
player_team1=${player_team1[0]}
player_team2=$( jq -r '.Game_1.Team_2 | .[0]' <<< ${status} )
player_team2=($player_team2)
player_team2=${player_team2[0]}
player_team3=$( jq -r '.Game_1.Team_3 | .[0]' <<< ${status} )
player_team3=($player_team3)
player_team3=${player_team3[0]}
player_team4=$( jq -r '.Game_1.Team_4 | .[0]' <<< ${status} )
player_team4=($player_team4)
player_team4=${player_team4[0]}
player_team5=$( jq -r '.Game_1.Team_5 | .[0]' <<< ${status} )
player_team5=($player_team5)
player_team5=${player_team5[0]}
player_team6=$( jq -r '.Game_1.Team_6 | .[0]' <<< ${status} )
player_team6=($player_team6)
player_team6=${player_team6[0]}
player_overflow1=$( jq -r '.overflow[0]' <<< ${status} )
player_overflow1=($player_overflow1)
player_overflow2=$( jq -r '.overflow[1]' <<< ${status} )
player_overflow2=($player_overflow2)

# TEST: Developer 1 is performing basic matchmaking
if [[ $player_team1 != $player_ranked1 ]]; then
    echo player_team1: $player_team1
    echo player_ranked1: $player_ranked1
    num_errors=$((num_errors+1))
    echo "ERROR (BasicMatchmakingTest): Incorrect team setup for team1. Expected" $player_ranked1", but received" $player_team1
fi
if [[ $player_team2 != $player_ranked2 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (BasicMatchmakingTest): Incorrect team setup for team2. Expected" $player_ranked2", but received" $player_team2
fi
if [[ $player_team3 != $player_ranked3 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (BasicMatchmakingTest): Incorrect team setup for team3. Expected" $player_ranked3", but received" $player_team3
fi
if [[ $player_team4 != $player_ranked4 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (BasicMatchmakingTest): Incorrect team setup for team4. Expected" $player_ranked4", but received" $player_team4
fi
if [[ $player_team5 != $player_ranked5 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (BasicMatchmakingTest): Incorrect team setup for team5. Expected" $player_ranked5", but received" $player_team5
fi
if [[ $player_team6 != $player_ranked6 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (BasicMatchmakingTest): Incorrect team setup for team6. Expected" $player_ranked6", but received" $player_team6
fi
if [[ $player_overflow1 != $player_ranked7 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (BasicMatchmakingTest): Incorrect player[0] for overflow. Expected" $player_ranked7", but received" $player_overflow1
fi
if [[ $player_overflow2 != $player_ranked8 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (BasicMatchmakingTest): Incorrect player[1] for overflow. Expected" $player_ranked8", but received" $player_overflow2
fi

# Perform advanced matchmaking
# Developer 2 adds players which are also present in developer 1s game1, but in game with different category
echo "Performing advanced matchmaking where there are no other games in this category..."

status=$(
    curl -s -X POST ${URL}/games/${game3_id}/players \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token2}" \
        -d '{
            "'${player1_email}'": {
                "game_parameter1_value": "75",
                "game_parameter2_value": "50"
            },
            "'${player3_email}'": {
                "game_parameter1_value": "100",
                "game_parameter2_value": "5"
            },
            "'${player4_email}'": {
                "game_parameter1_value": "150",
                "game_parameter2_value": "4"
            }
        }'
)
echo $status

player_ranked1=${player7_email} # 0.6(50) + 0.2(1) + 0.11(0) + 0.09(0) = 30.2
player_ranked2=${player3_email} # 0.6(40) + 0.2(3) + 0.11(1) + 0.09(2) = 24.89
player_ranked3=${player6_email} # 0.6(40) + 0.2(2) + 0.11(1) + 0.09(0) = 24.51
player_ranked4=${player4_email} # 0.6(30) + 0.2(6) + 0.11(4) + 0.09(1) = 19.73
player_ranked5=${player8_email} # 0.6(32) + 0.2(2) + 0.11(0) + 0.09(1) = 19.69
player_ranked6=${player1_email} # 0.6(30) + 0.2(2) + 0.11(1) + 0.09(0) = 18.51
player_ranked7=${player9_email} # 0.6(20) + 0.2(3) + 0.11(1) + 0.09(1) = 12.8
player_ranked8=${player5_email} # 0.6(10) + 0.2(10) + 0.11(1) + 0.09(1) = 8.2

status=$(
    curl -s -w "%{stdout}{\"code\": %{http_code}}" -X POST ${URL}/matchmake \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}" \
        -d '{
            "game_id": '${game1_id}',
            "player_emails": [
                "'${player1_email}'", "'${player3_email}'", "'${player4_email}'", "'${player5_email}'",
                "'${player6_email}'", "'${player7_email}'", "'${player8_email}'", "'${player9_email}'"
            ],
            "matchmaking_type": "advanced"
        }'
)
echo $status
code=$( jq -r '.code' <<< ${status} )
code=($code)
code=${code[1]}
player_team1=$( jq -r '.Game_1.Team_1 | .[0]' <<< ${status} )
player_team1=($player_team1)
player_team1=${player_team1[0]}
player_team2=$( jq -r '.Game_1.Team_2 | .[0]' <<< ${status} )
player_team2=($player_team2)
player_team2=${player_team2[0]}
player_team3=$( jq -r '.Game_1.Team_3 | .[0]' <<< ${status} )
player_team3=($player_team3)
player_team3=${player_team3[0]}
player_team4=$( jq -r '.Game_1.Team_4 | .[0]' <<< ${status} )
player_team4=($player_team4)
player_team4=${player_team4[0]}
player_team5=$( jq -r '.Game_1.Team_5 | .[0]' <<< ${status} )
player_team5=($player_team5)
player_team5=${player_team5[0]}
player_team6=$( jq -r '.Game_1.Team_6 | .[0]' <<< ${status} )
player_team6=($player_team6)
player_team6=${player_team6[0]}
player_overflow1=$( jq -r '.overflow[0]' <<< ${status} )
player_overflow1=($player_overflow1)
player_overflow2=$( jq -r '.overflow[1]' <<< ${status} )
player_overflow2=($player_overflow2)

# TEST: Developer 1 is performing advanced matchmaking but with no other games in the same category
if [[ $player_team1 != $player_ranked1 ]]; then
    echo player_team1: $player_team1
    echo player_ranked1: $player_ranked1
    num_errors=$((num_errors+1))
    echo "ERROR (AdvancedMatchmakingDiffCategoryTest): Incorrect team setup for team1. Expected" $player_ranked1", but received" $player_team1
fi
if [[ $player_team2 != $player_ranked2 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (AdvancedMatchmakingDiffCategoryTest): Incorrect team setup for team2. Expected" $player_ranked2", but received" $player_team2
fi
if [[ $player_team3 != $player_ranked3 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (AdvancedMatchmakingDiffCategoryTest): Incorrect team setup for team3. Expected" $player_ranked3", but received" $player_team3
fi
if [[ $player_team4 != $player_ranked4 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (AdvancedMatchmakingDiffCategoryTest): Incorrect team setup for team4. Expected" $player_ranked4", but received" $player_team4
fi
if [[ $player_team5 != $player_ranked5 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (AdvancedMatchmakingDiffCategoryTest): Incorrect team setup for team5. Expected" $player_ranked5", but received" $player_team5
fi
if [[ $player_team6 != $player_ranked6 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (AdvancedMatchmakingDiffCategoryTest): Incorrect team setup for team6. Expected" $player_ranked6", but received" $player_team6
fi
if [[ $player_overflow1 != $player_ranked7 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (AdvancedMatchmakingDiffCategoryTest): Incorrect player[0] for overflow. Expected" $player_ranked7", but received" $player_overflow1
fi
if [[ $player_overflow2 != $player_ranked8 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (AdvancedMatchmakingDiffCategoryTest): Incorrect player[1] for overflow. Expected" $player_ranked8", but received" $player_overflow2
fi

echo "Developer 3 adds a game..."

# Developer 3 adds a game
status=$(
    curl -s -X POST ${URL}/games \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token3}" \
        -d '{
            "name": "'${game4_name}'",
            "parameters": ["'${game4_parameter1_name}'", "'${game4_parameter2_name}'", "'${game4_parameter3_name}'", "'${game4_parameter4_name}'"],
            "weights": ['${game4_parameter1_weight}', '${game4_parameter2_weight}', '${game4_parameter3_weight}', '${game4_parameter4_weight}'],
            "category": "'${game4_category}'", 
            "teams_per_match": '${game4_teams_per_match}',
            "players_per_team": '${game4_players_per_team}'
        }'
)
game4_id=$status

# Developer 3 adds players which are also present in developer 1's game1, and is in game with same category
echo "Developer adds players..."
status=$(
    curl -s -X POST ${URL}/games/${game4_id}/players \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token3}" \
        -d '{
            "'${player1_email}'": {
                "game_parameter1_value": 75,
                "game_parameter2_value": 50,
                "game_parameter3_value": 0,
                "game_parameter4_value": 0
            },
            "'${player3_email}'": {
                "game_parameter1_value": 100,
                "game_parameter2_value": 5,
                "game_parameter3_value": 0,
                "game_parameter4_value": 0
            },
            "'${player4_email}'": {
                "game_parameter1_value": 150,
                "game_parameter2_value": 4,
                "game_parameter3_value": 0,
                "game_parameter4_value": 0
            }
        }'
)
echo $status

player_ranked1=${player3_email} # avg rank = (2 + 2) / 2 = 2 (speed: 100(0.8) + 5(0.2) = 81)
player_ranked2=${player4_email} # avg rank = (4 + 1) / 2 = 2.5 (speed: 150(0.8) + 4(0.2) = 120.8)
player_ranked3=${player7_email} # avg rank = (1 + 8) / 2 = 4.5
player_ranked4=${player1_email} # avg rank = (6 + 3) / 2 = 4.5 (speed: 75(0.8) + 50(0.2) = 70)
player_ranked5=${player6_email} # avg rank = (3 + 8) / 2 = 5.5
player_ranked6=${player8_email} # avg rank = (5 + 8) / 2 = 6.5
player_ranked7=${player9_email} # avg rank = (7 + 8) / 2 = 7.5
player_ranked8=${player5_email} # avg rank = (8 + 8) / 2 = 8

echo "Performing advanced matchmaking where there are is another game in this category..."

status=$(
    curl -s -w "%{stdout}{\"code\": %{http_code}}" -X POST ${URL}/matchmake \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}" \
        -d '{
            "game_id": '${game1_id}',
            "player_emails": [
                "'${player1_email}'", "'${player3_email}'", "'${player4_email}'", "'${player5_email}'",
                "'${player6_email}'", "'${player7_email}'", "'${player8_email}'", "'${player9_email}'"
            ],
            "matchmaking_type": "advanced"
        }'
)
echo $status
code=$( jq -r '.code' <<< ${status} )
code=($code)
code=${code[1]}
player_team1=$( jq -r '.Game_1.Team_1 | .[0]' <<< ${status} )
player_team1=($player_team1)
player_team1=${player_team1[0]}
player_team2=$( jq -r '.Game_1.Team_2 | .[0]' <<< ${status} )
player_team2=($player_team2)
player_team2=${player_team2[0]}
player_team3=$( jq -r '.Game_1.Team_3 | .[0]' <<< ${status} )
player_team3=($player_team3)
player_team3=${player_team3[0]}
player_team4=$( jq -r '.Game_1.Team_4 | .[0]' <<< ${status} )
player_team4=($player_team4)
player_team4=${player_team4[0]}
player_team5=$( jq -r '.Game_1.Team_5 | .[0]' <<< ${status} )
player_team5=($player_team5)
player_team5=${player_team5[0]}
player_team6=$( jq -r '.Game_1.Team_6 | .[0]' <<< ${status} )
player_team6=($player_team6)
player_team6=${player_team6[0]}
player_overflow1=$( jq -r '.overflow[0]' <<< ${status} )
player_overflow1=($player_overflow1)
player_overflow2=$( jq -r '.overflow[1]' <<< ${status} )
player_overflow2=($player_overflow2)

# TEST: Developer 1 is performing advanced matchmaking with other games in the same category
if [[ $player_team1 != $player_ranked1 ]]; then
    echo player_team1: $player_team1
    echo player_ranked1: $player_ranked1
    num_errors=$((num_errors+1))
    echo "ERROR (AdvancedMatchmakingSameCategoryTest): Incorrect team setup for team1. Expected" $player_ranked1", but received" $player_team1
fi
if [[ $player_team2 != $player_ranked2 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (AdvancedMatchmakingSameCategoryTest): Incorrect team setup for team2. Expected" $player_ranked2", but received" $player_team2
fi
if [[ $player_team3 != $player_ranked3 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (AdvancedMatchmakingSameCategoryTest): Incorrect team setup for team3. Expected" $player_ranked3", but received" $player_team3
fi
if [[ $player_team4 != $player_ranked4 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (AdvancedMatchmakingSameCategoryTest): Incorrect team setup for team4. Expected" $player_ranked4", but received" $player_team4
fi
if [[ $player_team5 != $player_ranked5 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (AdvancedMatchmakingSameCategoryTest): Incorrect team setup for team5. Expected" $player_ranked5", but received" $player_team5
fi
if [[ $player_team6 != $player_ranked6 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (AdvancedMatchmakingSameCategoryTest): Incorrect team setup for team6. Expected" $player_ranked6", but received" $player_team6
fi
if [[ $player_overflow1 != $player_ranked7 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (AdvancedMatchmakingSameCategoryTest): Incorrect player[0] for overflow. Expected" $player_ranked7", but received" $player_overflow1
fi
if [[ $player_overflow2 != $player_ranked8 ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (AdvancedMatchmakingSameCategoryTest): Incorrect player[1] for overflow. Expected" $player_ranked8", but received" $player_overflow2
fi

# -------------------------------------------------------
# CLEANING UP
# -------------------------------------------------------

# Delete players for game1
echo "Deleting player stats..."
for p in ${players[@]}; do
    echo "Deleting player p: ${p}"
    status=$(
        curl -s -X DELETE ${URL}/games/${game1_id}/players/${p} \
            -H "Authorization: ${token1}" \
    )
    echo $status
done

# Developer 1 gets all players for their game to make sure all were deleted
status=$(
    curl -s -w "%{http_code}" -X GET ${URL}/games/${game1_id}/players \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}"
)

# TEST: No players should exist for this game
if [ $status != 204 ]; then
    num_errors=$((num_errors+1))
    echo "ERROR (GameDeletedAllPlayersTest): There should be no players for this game with game_id ${game1_id}"
fi

players3=($player1_email, $player3_email, $player4_email)

echo "Players for game 3"
status=$(
    curl -s -X GET ${URL}/games/${game3_id}/players \
        -H "Authorization: ${token1}"
)
echo $status

echo "Players for game 4"
status=$(
    curl -s -X GET ${URL}/games/${game4_id}/players \
        -H "Authorization: ${token1}"
)
echo $status

# Delete players for game3
for p in ${players3[@]}; do
    echo "Deleting player p: ${p}"
    status=$(
        curl -s -X DELETE ${URL}/games/${game3_id}/players/${p} \
            -H "Authorization: ${token2}" \
    )
    echo $status
done

# Delete players for game4
for p in ${players3[@]}; do
    echo "Deleting player p: ${p}"
    status=$(
        curl -s -X DELETE ${URL}/games/${game4_id}/players/${p} \
            -H "Authorization: ${token3}" \
    )
    echo $status
done

echo "Delete games..."

# Delete games
status=$(
    curl -s -X DELETE ${URL}/games/${game1_id} \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}"
)
echo $status

status=$(
    curl -s -X DELETE ${URL}/games/${game2_id} \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}"
)
echo $status

status=$(
    curl -s -X DELETE ${URL}/games/${game3_id} \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token2}"
)
echo $status

status=$(
    curl -s -X DELETE ${URL}/games/${game4_id} \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token3}"
)
echo $status

# Developer 1 gets their games (shouldn't have games anymore)
status=$(
    curl -s -w "%{http_code}" -X GET ${URL}/games \
        -H "Authorization: ${token1}"
)

# TEST: Developer 1 shouldn't have any games
if [ "$status" -ne 204 ]
then
    num_errors=$((num_errors+1))
    echo "ERROR (DeveloperDeletedGamesTest): Developer 1 shouldn't have any games"
fi

# Developer 3 gets their games (shouldn't have games anymore)
status=$(
    curl -s -w "%{http_code}" -X GET ${URL}/games \
        -H "Authorization: ${token3}"
)

# TEST: Developer 3 shouldn't have any games
if [ "$status" -ne 204 ]
then
    num_errors=$((num_errors+1))
    echo "ERROR (DeveloperDeletedGamesTest): Developer 3 shouldn't have any games"
fi

echo "Delete developers..."

# Delete developers
status=$(
    curl -s -X DELETE ${URL}/login \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token1}"
)
echo $status

status=$(
    curl -s -X DELETE ${URL}/login \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token2}"
)
echo $status

status=$(
    curl -s -X DELETE ${URL}/login \
        -H 'Content-Type: application/json' \
        -H "Authorization: ${token3}"
)
echo $status

# Developer 2 tries to log in (shouldn't be able to)
echo "Developer tries to login after deleting account..."

status=$(
    curl -s -X POST ${URL}/login \
        -H 'Content-Type: application/json' \
        -d "{ \"developer_email\": \"${developer2_email}\", \
            \"developer_password\": \"${developer2_password}\" }"
)
echo $status

if [[ "$status" != "Developer does not exist" ]]; then
    num_errors=$((num_errors+1))
    echo "ERROR (DeveloperLoggingInAfterDeletionTest): Developer shouldn't be able to log-in; account was recently deleted"
fi

# Exiting script if there is any errors
if [[ $num_errors -gt 0 ]]; then
    echo "***** SCRIPT FAILED WITH ${num_errors} ERRORS *****"
    exit 1
else 
    echo "***** SCRIPT SUCCEEDED *****"
fi