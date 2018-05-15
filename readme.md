Description
==============
This application represents a simple and quick implementation of the game GO on the LaFortuna board. Created by: Alexandru Amarandei Stanescu.

Installation
==============
To run the project, install the AVR tool chain then run Klaus's universal makefile.

Links
==============
GO rules: https://www.britgo.org/files/rules/GoQuickRef.pdf
The game ends when both players pass consecutively. Then, the player with more points wins.

Gameplay
==============
- 2 players are required to play the game.
- The board is 10 by 10.
- The stones (pieces) are represented by squares.
- Player one places red squares and player two blue squares.
- To move, use the directional buttons on the board.
- To place a stone, use the central button.
- The current selected position is displayed with a smaller, green square.
- To skip a turn, press center three times while on an occupied position.
- The score is presented on the right of the board.

Algorithms used 
==============
BFS and DFS for exploring the board.

Could do
==============
- Add themes
- Check for commands within a timer with interrupts
- Change board size
- Reduce memory usage

This File Was Added on 15/05/2018
--------------