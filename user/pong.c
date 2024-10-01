#include "user.h"
#include "kernel/syscall.h"

// Declare the sys call wrappers
extern int enable_raw_mode();
extern int disable_raw_mode();
extern int gotoxy(int x, int y);

#define V 15  // Adjusted height of the game board
#define H 40  // Adjusted width of the game board

// Function declarations
void playGame();
void gameRules();
int gotoxy(int x, int y);  // Must return int, as per user.h
void clearScreen();
void draw(char map[V][H], int score);
void show(char map[V][H], int score);
void update(char map[V][H], int paddleStart, int paddleEnd, int py, int px);
void input(char map[V][H], int *paddleStart, int *paddleEnd, int *px, int *py, int *modx, int *mody, int *score, int *gol);
void edge(char map[V][H]);
void player(char map[V][H], int paddleStart, int paddleEnd);
void ball(char map[V][H], int py, int px);
void handleArrowKeys(int *paddleStart, int *paddleEnd);
void busy_wait(int milliseconds);
int getchar_direct();
int kbhit();

// Game variables
int playerwin = 0;

int main() {
    int choice;

    while (1) {
        printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
        printf("|                     Ping Pong Game                       |");
        printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
        printf("\n1. Game Rules");
        printf("\n2. Play Game");
        printf("\n3. Quit\n");

        choice = getchar_direct() - '0';  // Convert char to int

        switch (choice) {
            case 1:
                gameRules();
                break;
            case 2:
                playGame();
                break;
            case 3:
                return 0;
            default:
                printf("Invalid choice\n");
        }
    }
    return 0;
}

// Display game rules
void gameRules() {
    clearScreen();
    gotoxy(0, 0);
    printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    printf("|                      Game Rules                          |\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    printf("- Use the up and down arrow keys to move your paddle up and down.\n");
    printf("- The goal is to prevent the ball from passing your paddle on the left.\n");
    printf("- The ball will bounce off the right wall.\n");
    printf("- Every time you hit the ball, you earn 1 point.\n");
    printf("\nPress any key to return to the main menu...\n");

    getchar_direct(); // Wait for user input
}

void playGame() {

    enable_raw_mode(); // Enable raw mode before game start

    char map[V][H];
    int px = H / 2, py = V / 2; // Ball position
    int modx = -1, mody = -1;   // Ball direction
    int paddleStart = V / 2 - 2, paddleEnd = V / 2 + 2; // Player paddle
    int score = 0;              // Player  score
    int gol = 0;

    while (1) {
        gol = 0;
        draw(map, score); // Clear the map and display the score
        input(map, &paddleStart, &paddleEnd, &px, &py, &modx, &mody, &score, &gol);
        update(map, paddleStart, paddleEnd, py, px); // Update game state
        show(map, score); // Show the updated map and score

        busy_wait(500); // Slow down or speed up the game loop (measured in ms between updates)

        if (gol == 1) {
            break;
        }
    }

    // Disable raw mode after game ends
    disable_raw_mode();
}

// Function to draw the game board
void draw(char map[V][H], int score) {
    int i, j;
    clearScreen();
    for (i = 0; i < V; i++) {
        for (j = 0; j < H; j++) {
            map[i][j] = ' ';
        }
    }
}

// Function to display the game state
void show(char map[V][H], int score) {
    int i, j;
    clearScreen();
    gotoxy(0, 0); // Reset cursor position
    printf(" --------------------------------------\n");   // Dispalys row of dashes above the score
    printf("  Score: %d\n", score);
    for (i = 0; i < V; i++) {
        for (j = 0; j < H; j++) {
            printf("%c", map[i][j]);
        }
	printf("\n");
    }
}

// Function to pdate the game state (ball and paddle)
void update(char map[V][H], int paddleStart, int paddleEnd, int py, int px) {
    edge(map);
    player(map, paddleStart, paddleEnd);
    ball(map, px, py);
}

// Function to handle user input and ball movement
void input(char map[V][H], int *paddleStart, int *paddleEnd, int *px, int *py, int *modx, int *mody, int *score, int *gol) {
    // Check if the ball hits top or bottom edges
    if (*py == 1 || *py == V - 2) {
        *mody *= -1;
    }

    // Ball reaches the left wall (player loses)
    if (*px == 1) {
        *gol = 1;
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
    if (*gol == 0) {
        *px += (*modx);
        *py += (*mody);
    }

    // Handle paddle movement using the arrow keys
    handleArrowKeys(paddleStart, paddleEnd);
}

// Function to draw the edges of the game board
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

// Function to draw the player paddle
void player(char map[V][H], int paddleStart, int paddleEnd) {
    for (int i = paddleStart; i <= paddleEnd; i++) {
        map[i][3] = '#'; // Player paddle
    }
}

// Function to draw the ball
void ball(char map[V][H], int px, int py) {
    map[py][px] = 'O'; // Ball
}

// Function to clear the screen
void clearScreen() {
    clear_screen();  // Call without sys_ prefix
}

// Simple busy-wait delay function
void busy_wait(int milliseconds) {
    volatile int i, j;
    for (i = 0; i < milliseconds * 1000; i++) {
        for (j = 0; j < 100; j++) {
            // Busy-wait to simulate delay
        }
    }
}

// Direct implementation of getchar using read
int getchar_direct() {
    char c;
    read(0, &c, 1); // Read one character from stdin (fd 0)
    return c;
}

// Check if a key is pressed (non-blocking input)
int kbhit() {
    int result = nonblock_read(); // Call without sys_ prefix
    return result != 0; // Return true if there's input
}

// Handle the up and down arrow keys for paddle movement
void handleArrowKeys(int *paddleStart, int *paddleEnd) {
    if (kbhit()) {
        char key = getchar_direct();
        if (key == '\033') { // First part of the escape sequence
            getchar_direct();       // Skip the '[' part of the sequence
            switch(getchar_direct()) { // Check the final part of the sequence
                case 'A': // Up arrow key
                    if (*paddleStart > 1) {
                        (*paddleStart)--;
                        (*paddleEnd)--;
                    }
                    break;
                case 'B': // Down arrow key
                    if (*paddleEnd < V - 2) {
                        (*paddleStart)++;
                        (*paddleEnd)++;
                    }
                    break;
            }
	}
    }
}
