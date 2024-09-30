/* This is the template .c file that I tired to emulate with my FogOS version of pong
   Note: the library imports being used here are the basis for my sys calls that inter-
   face with the VGA/UART console -- causing my user input / screen refresh dilemmas.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // For usleep()
#include <termios.h>  // For terminal control
#include <fcntl.h>    // For non-blocking input

#define V 15  // Adjusted height of the game board
#define H 40  // Adjusted width of the game board

// Function declarations
void playGame();
void gameRules();
void gotoxy(int x, int y);
void clearScreen();
void draw(char map[V][H], int score);
void show(char map[V][H], int score);
void update(char map[V][H], int paddleStart, int paddleEnd, int py, int px);
void input(char map[V][H], int *paddleStart, int *paddleEnd, int *px, int *py, int *modx, int *mody, int *score, int *game_over);
void edge(char map[V][H]);
void player(char map[V][H], int paddleStart, int paddleEnd);
void ball(char map[V][H], int py, int px);
void enableRawMode();
void disableRawMode();
int kbhit();
void handleArrowKeys(int *paddleStart, int *paddleEnd);

struct termios orig_termios; // Store original terminal attributes

int playerwin = 0;

int main() {
    int choice;
    while (1) {
        clearScreen();  // Clear screen on each menu display
        printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
        printf("|                     Ping Pong Game                       |\n");
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
        printf("\n1. Game Rules");
        printf("\n2. Play Game");
        printf("\n3. Quit\n");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                gameRules();
                break;
            case 2:
                playGame();
                break;
            case 3:
                disableRawMode(); // Ensure terminal is restored before quitting
                exit(0);
            default:
                printf("Invalid choice");
        }
    }
    return 0;
}

void gameRules() {
    clearScreen();
    printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    printf("|                      Game Rules                          |\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    printf("\n- Use the up and down arrow keys to move your paddle up and down.");
    printf("\n- The goal is to prevent the ball from passing your paddle on the left.");
    printf("\n- The ball will bounce off the right wall.");
    printf("\n- Every time you hit the ball, you earn 1 point.");
    printf("\n\nPress any key to return to the main menu...\n");
    getchar();
    getchar(); // Wait for user input to go back
}

void playGame() {
    char map[V][H];
    int px = H / 2, py = V / 2; // Ball position
    int modx = -1, mody = -1;   // Ball direction
    int paddleStart = V / 2 - 2, paddleEnd = V / 2 + 2; // Player paddle
    int score = 0;              // Player's score
    int game_over = 0;

    enableRawMode(); // Enable raw mode for real-time input

    while (1) {
        game_over = 0;
        draw(map, score); // Clear the map and display the score
        input(map, &paddleStart, &paddleEnd, &px, &py, &modx, &mody, &score, &game_over);
        update(map, paddleStart, paddleEnd, py, px); // Update game state
        show(map, score); // Show the updated map and score
        usleep(100000); // Sleep for 100ms to slow down the game loop

        if (game_over == 1) {
            break;
        }
    }

    disableRawMode(); // Restore terminal to original state
}

void draw(char map[V][H], int score) {
    int i, j;
    clearScreen();
    for (i = 0; i < V; i++) {
        for (j = 0; j < H; j++) {
            map[i][j] = ' ';
        }
    }
}

void show(char map[V][H], int score) {
    int i, j;
    clearScreen();
    printf(" --------------------------------------\n");   // Row of dashes above the score
    printf("  Score: %d\n", score); // Display the score
    for (i = 0; i < V; i++) {
        for (j = 0; j < H; j++) {
            printf("%c", map[i][j]);
        }
        printf("\n");
    }
}

void update(char map[V][H], int paddleStart, int paddleEnd, int py, int px) {
    edge(map);
    player(map, paddleStart, paddleEnd);
    ball(map, px, py);
}

void input(char map[V][H], int *paddleStart, int *paddleEnd, int *px, int *py, int *modx, int *mody, int *score, int *game_over) {
    // Check if the ball hits top or bottom edges
    if (*py == 1 || *py == V - 2) {
        *mody *= -1;
    }

    // Ball reaches the left wall (player loses)
    if (*px == 1) {
        *game_over = 1;
        playerwin = 0; // Player loses
    }

    // Ball hits the right wall (ball bounces back)
    if (*px == H - 2) {
        *modx *= -1; // Ball bounces off the right wall
    }

    // Ball collides with player paddle
    if (*px == 4) {
        if (*py >= *paddleStart && *py <= *paddleEnd) {
            *modx *= -1; // Ball bounces back from player paddle
            (*score)++;  // Increment player's score when they hit the ball
        }
    }

    // Ball movement
    if (*game_over == 0) {
        *px += (*modx);
        *py += (*mody);
    }

    // Handle paddle movement using the arrow keys
    handleArrowKeys(paddleStart, paddleEnd);
}

void edge(char map[V][H]) {
    for (int i = 0; i < H; i++) {
        map[0][i] = '-';  // Top border remains a hyphen
        map[V - 1][i] = '_';  // Bottom border is now an underscore
    }
    for (int i = 0; i < V; i++) {
        map[i][0] = '|';
        map[i][H - 1] = '|';
    }
}

void player(char map[V][H], int paddleStart, int paddleEnd) {
    for (int i = paddleStart; i <= paddleEnd; i++) {
        map[i][3] = '#'; // Player paddle
    }
}

void ball(char map[V][H], int px, int py) {
    map[py][px] = 'O'; // Ball
}

void gotoxy(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1); // Move the cursor to (x, y)
}

void clearScreen() {
    printf("\033[2J"); // ANSI escape code to clear the screen
    gotoxy(0, 0);      // Move cursor to the top
}

// Enable raw mode for real-time input
void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);   // Get the terminal attributes
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);          // Disable echo and canonical mode
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); // Set the terminal attributes
}

// Disable raw mode and restore the terminal
void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); // Restore original terminal attributes
}

// Check if a key is pressed (non-blocking input)
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);              // Get terminal attributes
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);            // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);     // Set new terminal attributes
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();                              // Check if a key is pressed

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);     // Restore old terminal attributes
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);                       // If a key is pressed, put it back
        return 1;
    }

    return 0;
}

// Handle the up and down arrow keys for paddle movement
void handleArrowKeys(int *paddleStart, int *paddleEnd) {
    if (kbhit()) {
        char key = getchar();
        if (key == '\033') { // First part of the escape sequence
            getchar();       // Skip the '[' part of the sequence
            switch(getchar()) { // Check the final part of the sequence
                case 'A': // Up arrow
                    if (*paddleStart > 1) {
                        *paddleStart -= 1;
                        *paddleEnd -= 1;
                    }
                    break;
                case 'B': // Down arrow
                    if (*paddleEnd < V - 2) {
                        *paddleStart += 1;
                        *paddleEnd += 1;
                    }
                    break;
            }
        }
    }
}
