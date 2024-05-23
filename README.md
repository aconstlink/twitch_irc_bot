# C++ Twitch IRC Bot
This repository contains projects that show how to use the Twitch IRC bot included in my C++ game engine [motor](https://github.com/aconstlink/motor)

## About
The bot is implemented in C++ and does build out of the box. The engine has very little dependencies and can run on Windows and Linux. The goal of the bot is to interact with users in the Twitch stream chat. This bot is intended to be used by the broadcaster itself. The bot uses the "Device Code Grant Flow" so all the data needed is stored on the device used and does not require any server.

## Dependencies
Runing the bot requires [Curl](https://curl.se/) for secure connection to Twitch and the OAuth handshaking. Furthermore, Curl is also used to interact with the [Twitch endpoints](https://dev.twitch.tv/docs/api/reference/) for even more user interaction! So if Curl is present on your system, you are good to go. 

Curl is not required for building the project nor does the engine require it. It is just used for Twitch communication at run-time.

Besides that, all you need is in the project included.

## Required User Information
You need to provide some Twitch related information in order to use the bot. 
- Client ID
- Client Secret (only for confidential clients)
- Channel Name
- Nick Name
- Broadcasater ID
- Bot ID
  
## How does it work
You as the broadcaster is using the bot. You should not pass the bot to someone else due to confidential data used if you are using a confidential client application. If you are using a public client application, you can actually pass your bot around! You could even ship it with your game.

### 1 Create a Twitch Account for your bot
The regular way to create you a bot is to create new Twitch account. For this account, go to its developer console and create a confidential/public client application with a client_id and an optional client_secret. -We need to create a confidential client because otherwise the process of refreshing the user token will not work. The refresh token will not refresh the user token and is quickly expired. Also the device_code will expire quickly, so the user need to regularly and manually authorize the bot.-

Creating a separate Twitch account for your bot is optional. You could also use your own account where you broadcast.

### 2 Compile and start the program
The program will create a twitch.confidentials file where you need to add all the data we created. Because the first run, i.e. no twitch.confidentials file exist, the program will terminate.

### 3 Add the data
Add the client_id and the client_secret to the twitch.confidentials file. Along with it, add you channel name, i.e. where you broadcast and a nick name. Either the broadcaster or the bot name. Depending on which account the bot will run.

### 4 Get the user ids
We need the user_id from the broadcaster and the bot. In case you are using the bot on your broadcasting account, the broadcaster_id is the same as the bot_id. 
[Here](https://discuss.dev.twitch.com/t/broadcaster-id-how-get/42682) is one way to get it. 

You could also start the program with the already set user credentials. The program will automatically determin the user token, refresh token and device code for you. The IRC bot will work now. You can write something in the you channels' chat. Using the console application will print the fact to the console that you wrote something. In that print is your user id. You can copy it from there.



