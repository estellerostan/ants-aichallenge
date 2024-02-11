# [AI Challenge Ants](http://ants.aichallenge.org/)

## Using VS Code
There are two parts to set up the project with VS Code on Windows.

## bot
How to build the `.exe`:
1. Inside VS Code, open the `bot` folder
1. Install [MinGW](https://code.visualstudio.com/docs/cpp/config-mingw)   
1. Install [Makefile Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.makefile-tools) for VS Code
1. Use the `Makefile: Build the current target` command

## tools
How to test the bot:   
Run: `.\play_one_game.cmd .\MyBot.exe` from the `tools` folder.

Note: `play_one_game.cmd` was updated to remove `RandomBot.py` to allow `MyBot.exe` to play.

## Using Visual Studio
There are some settings in `bot/Bot.vcxproj.user` that allows to automatically build and run the tools with Visual Studio to test faster than with VS Code.
