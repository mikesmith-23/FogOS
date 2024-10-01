# FogOS

Project 1: Pong Command Integration for FogOS

Name: Michael H. Smith

Project: Terminal-Based Pong Game

Description:

This project implements a text-based version of the classic Atari game, Pong. 
A single-player version of the game, the user controls a paddle and attempts 
to prevent the ball from passing their paddle while the ball bounces back 
from the opposite wall.

The Pong command is designed to integrate with the existing OS and utilizes 
several system calls for input handling and screen manipulation. The 
implementation faced challenges with real-time user input, and while it is 
functional, some areas—such as non-blocking input—still require further 
refinement.

Features:

* Real-Time Input: The game relies on custom system calls to handle user input 
  in real time, although there are still challenges in making this fully 
  non-blocking.

* Basic Game Mechanics: The player controls a paddle using the arrow keys, and 
  the ball bounces back and forth in the terminal window.

* Game Logic: The game ends when the player fails to prevent the ball from 
  passing the paddle, and the score is displayed in the terminal.

Files Modified/Created:

Modified:

* Makefile: Updated to compile pong.c along with the rest of the user programs 
  and link the necessary system calls.

* kernel/console.c: Adjustments made for raw mode and real-time input handling 
  in conjunction with the new system calls.

* kernel/defs.h: Updated with new function prototypes related to input handling 
  for the Pong game.

* kernel/syscall.c: Integrated new system calls for cursor movement, clearing 
  the screen, and handling non-blocking input.

* kernel/syscall.h: Added definitions for new system calls required for the 
  Pong command.

* kernel/sysproc.c: Additional system calls implemented to handle user input 
  and terminal manipulations.

* user/ulib.c: System calls integrated here to allow the Pong game to interact 
  with the terminal (e.g., moving the cursor and clearing the screen).

* user/user.h: Function prototypes updated to include new system calls and 
  utilities needed for the Pong game.

* user/usys.pl: System call numbers updated to accommodate new functions.

Created:

* kernel/rawmode.c: Created to handle raw input mode for the terminal, enabling 
  real-time input during the Pong game.

* kernel/vga.c & kernel/vga.h: Implemented to support VGA-like cursor movement 
  and screen clearing features in the terminal.

* user/pong.c: Main game logic and implementation of the Pong game, handling 
  game mechanics, input, and rendering.

Challenges:

The main challenge encountered in this project was implementing non-blocking 
input. Due to the complexities involved in handling user input within a 
terminal environment, input is still partially blocking. Furthermore, due to 
the issues with input handling, the initially planned difficulty modes (easy, 
medium, hard) were not implemented, as the underlying game mechanics were not 
yet fully stable.

Future Enhancements:

* Non-Blocking Input: Further work is needed to ensure that the game can 
  process input without stalling, allowing for smoother and more responsive 
  gameplay.

* Difficulty Modes: Once the input handling is refined, flags can be added to 
  adjust the difficulty by altering the ball's speed and the game's refresh 
  rate.

* Multiplayer Mode: A future enhancement could include adding a multiplayer 
  mode, allowing two players to control separate paddles.

* Game Logic Improvements: The ball's trajectory and speed could be adjusted 
  to make the gameplay more dynamic and challenging.

Lessons Learned:

Through the process of developing the Pong command, I gained a deeper 
understanding of how to implement system calls in the xv6 OS. Specifically, I 
learned how to manipulate the terminal by moving the cursor and clearing the 
screen via system calls, as well as enabling and disabling raw input mode for 
real-time user interaction.

Additional Notes:

The repository includes a fully-functional C template of the Pong game logic 
in the docs folder, which was used as a reference during this development.
