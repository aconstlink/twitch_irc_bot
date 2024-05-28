# C++ Twitch IRC Bot
This repository contains projects that show how to use the Twitch IRC bot included in my C++ game engine [motor](https://github.com/aconstlink/motor)

[![CMake on multiple platforms](https://github.com/aconstlink/twitch_irc_bot/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/aconstlink/twitch_irc_bot/actions/workflows/cmake-multi-platform.yml)

## About
The bot is implemented in C++ and does build out of the box. The engine has very little dependencies and can run on Windows and Linux. The goal of the bot is to interact with users in a Twitch stream chat. This bot is intended to be used by the broadcaster itself but could be used on any other channel. The bot uses the "Device Code Grant Flow" so all the data needed is stored on the device used and does not require any server.

If you want to use the bot, you need to use my game engine [motor](https://github.com/aconstlink/motor). There are certainly other IRC bot implementations out there. Twitch has a integrations for popular engines too.

## Dependencies
Runing the bot requires [Curl](https://curl.se/) for secure connection to Twitch and the OAuth handshaking. Furthermore, Curl is also used to interact with the [Twitch endpoints](https://dev.twitch.tv/docs/api/reference/) for even more user interaction! So if Curl is present on your system, you are good to go. 

Curl is not required for building the project nor does the engine require it. It is just used for Twitch communication at run-time.

Besides that, all you need is in the project included.

## First off

Go into the folder "working/Twitch" and duplicate the "credentials.store" and rename it to "bot_data.json". This will be the confidentials file! Do not show that anywhere and do not distribute it after the program started. It will contain confidential data. But you need to edit it in a bit.

## How does it work

There are two ways of using this program as a bot. You could use the bot with your own user account as the broadcaster or you could create a Twitch bot account und let the bot run using that account and also post as such.

### Option 1: Use your broadcaster account

This means, the bot will login and write into the chat with your user name. You need to fill the following data in the confidential file:
- client_id
- broadcaster_name

You can make a public client application in your Twitch developer console. Please follow this [guide](https://dev.twitch.tv/docs/authentication/register-app/). It really is very simple. After you created the application, you can press the manage button and copy the ```client_id``` for that client application. Paste it into the confidential file.

The rest of the required login information is derived from that. 

Try the sample application in this repo first. Compile and start the console application. Once running, a verification URI will be presented to you. Copy and paste it into you browser. You will need to authorize the client application created above. The bot will automatically login once you authorized the Twitch app. In essence, the program will get a user token from Twitch along with a refresh Token for renewing expired user tokens automatically. 

![By Broadcaster](https://github.com/aconstlink/twitch_irc_bot/blob/main/images/as_user.jpg)
The image shows me using the build-in ```!echo``` command which just echos back to the person posted using my user name! The echo command is used for testing purpose.

### Option 2: Use a bot account

You do everything from above but authorizing the Twitch client application using the bot account. So login with the bot account, create a Twitch client application in the bot accounts dev console. After starting the program and being presented the verification URI, browse it and authorize using the bot account. 

But before, please add the following data to the confidential file first:
- client_id
- broadcaster_name
- bot_name
  
![By Bot](https://github.com/aconstlink/twitch_irc_bot/blob/main/images/as_bot.jpg)
The image shows me using the build-in ```!echo``` command which just echos back to the person posted using the bots' user name!

## More Complex Use Cases

This project also contains a visual example. The program just executes more commands. You can spawn triangles or circles like so:

``` !spawn triangles num_tris ```  
``` !spawn circles num_circs ```  

The following images shows an example of the program ``` 02_ui_bot ``` 

![More Complex](https://github.com/aconstlink/twitch_irc_bot/blob/main/images/more_complex.jpg)

## Futhermore

The bot can be used in any other channel using the account authorized the bots' scopes. If the account used for the bot becomes a moderator it can do anything a moderator can do. 


