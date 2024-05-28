# C++ Twitch IRC Bot
This repository contains projects that show how to use the Twitch IRC bot included in my C++ game engine [motor](https://github.com/aconstlink/motor)

[![CMake on multiple platforms](https://github.com/aconstlink/twitch_irc_bot/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/aconstlink/twitch_irc_bot/actions/workflows/cmake-multi-platform.yml)

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

There are two ways of using this program as a bot. You could use the bot with your own user account as the broadcaster or you could create a Twitch bot account und let the bot run using this account.

### Use your broadcaster account

This means, the bot writes into the chat with your user name. You need to fill the following data in the confidential file:
- client_id
- broadcaster_name

You can make a public client application in your Twitch developer console. Please follow this [guide](https://dev.twitch.tv/docs/authentication/register-app/). It really is very simple. After you created the application, you can press the manage button and copy the client_id for that client application. Paste it into the confidential file.

The rest of the required login information is derived from that. 

Try the sample application in this repo first. Compile and start the console application. Once running, a verification URI will be presented to you. Copy and paste it into you browser. You will need to authorize the client application created above. The bot will automatically login once you authorized the Twitch app. In essence, the program will get user token from Twitch along with a refresh Token for renewing expired user tokens automatically. 

### Use a bot account

You do everything from above but authorizing the Twitch client application using the bot account. So login with the bot account, browse the verification uri and authorize. 

But before, please add the following data to the confidential file first:
- client_id
- broadcaster_name
- bot_name


Thats it.


