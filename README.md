# Matchmaking API

### How to Run the Project
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
	```source /home/deb/Desktop/game_api_db_sql.sql;```

	Exit Console: ```quit```

4. Install C++ Connector for MySQL

	```sudo apt install libmysqlcppconn-dev```

5. Install Boost for Crow\
	```sudo apt-get install libboost-all-dev```

6. Restart the VM

7. Run Project\
	Navigate to Project Directory
	```
	bazel build //main:main
	bazel-bin/main/main
	```

The project does a simple SQL query and then starts the server
So you should see something like the following:

```
Let's have MySQL count from 10 to 1...
	... MySQL counts: 10
	... MySQL counts: 9
	... MySQL counts: 8
	... MySQL counts: 7
	... MySQL counts: 6
	... MySQL counts: 5
	... MySQL counts: 4
	... MySQL counts: 3
	... MySQL counts: 2
	... MySQL counts: 1
(2022-10-08 15:51:17) [INFO    ] Crow/1.0 server is running at http://0.0.0.0:18080 using 2 threads
(2022-10-08 15:51:17) [INFO    ] Call `app.loglevel(crow::LogLevel::Warning)` to hide Info level logs.
```


	
