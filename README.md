# AI Challenge Ants
There are two parts to set up the project.

# bot
How to build the `.exe` on Windows:
1. Inside VS Code, open the `bot` folder
1. Install [MinGW](https://code.visualstudio.com/docs/cpp/config-mingw)   
1. Install [Makefile Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.makefile-tools) for VS Code
1. Use the `Makefile: Build the current target` command

# tools
How to test the bot on Windows:   
Run: `.\play_one_game.cmd .\MyBot.exe` from the `tools` folder.

Note: `play_one_game.cmd` was updated to remove `RandomBot.py` to allow `MyBot.exe` to play.
