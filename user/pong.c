#include "user.h"
#include "kernel/syscall.h"

// Declare system call wrappers
extern int enable_raw_mode();
extern int disable_raw_mode();
extern int gotoxy(int x, int y);

#define V 15  // Height of the game board
#define H 40  // Width of the game board

// Function declarations
void playGame();
void gameRules();
void displayMenu();
int gotoxy(int x, int y);
void draw(char map[V][H], int score);
void show(char map[V][H], int score);
void update(char map[V][H], int paddleStart, int paddleEnd, int py, int px);
void input(char map[V][H], int *paddleStart, int *paddleEnd, int *px, int *py, int *modx, int *mody, int *score, int *gameOver);
void edge(char map[V][H]);
void player(char map[V][H], int paddleStart, int paddleEnd);
void ball(char map[V][H], int py, int px);
void handleArrowKeys(int *paddleStart, int *paddleEnd);
void busy_wait(int milliseconds);
int getchar_direct();
int kbhit();

// Game state variables
int playerwin = 0;

int main() {
    int choice;

    while (1) {
        displayMenu();

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

// Function to display the main menu
void displayMenu() {
    printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    printf("\n\n");
    printf("              _______ _______ ______  _______ \n");
    printf("             |   _   |   _   |   _  \\|   _   |\n");
    printf("             |.  1   |.  |   |.  |   |.  |___|\n");
    printf("             |.  ____|.  |   |.  |   |.  |   |\n");
    printf("             |:  |   |:  1   |:  |   |:  1   |\n");
    printf("             |::.|   |::.. . |::.|   |::.. . |\n");
    printf("             `---'   `-------`--- ---`-------'\n");
    printf("\n\n");
    printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    printf("1. Game Rules\n");
    printf("2. Play Game\n");
    printf("3. Quit\n");
}

// Display game rules
void gameRules() {
    clear_screen();
    gotoxy(0, 0);
    printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    printf("|                      Game Rules                          |\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    printf("- Use the up and down arrow keys to move your paddle.\n");
    printf("- Prevent the ball from passing your paddle.\n");
    printf("- The ball will bounce off the right wall.\n");
    printf("- Each hit scores 1 point.\n");
    printf("\nPress any key to return to the main menu...\n");
    getchar_direct();
}

// Main game loop
void playGame() {
    enable_raw_mode();

    char map[V][H];
    int px = H / 2, py = V / 2;
    int modx = -1, mody = -1;
    int paddleStart = V / 2 - 2, paddleEnd = V / 2 + 2;
    int score = 0;
    int gameOver = 0;

    while (1) {
        gameOver = 0;
        draw(map, score);
        input(map, &paddleStart, &paddleEnd, &px, &py, &modx, &mody, &score, &gameOver);
        update(map, paddleStart, paddleEnd, py, px);
        show(map, score);

        busy_wait(500);

        if (gameOver == 1) {
            break;
        }
    }

    disable_raw_mode();
}

// Clear and initialize game board
void draw(char map[V][H], int score) {
    clear_screen();
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < H; j++) {
            map[i][j] = ' ';
        }
    }
}

// Display game state (paddle, ball, and score)
void show(char map[V][H], int score) {
    clear_screen();
    gotoxy(0, 0);
    printf(" --------------------------------------\n");
    printf("  Score: %d\n", score);
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < H; j++) {
            printf("%c", map[i][j]);
        }
        printf("\n");
    }
}

// Update the game state (ball movement, paddle position)
void update(char map[V][H], int paddleStart, int paddleEnd, int py, int px) {
    edge(map);
    player(map, paddleStart, paddleEnd);
    ball(map, px, py);
}

// Handle ball movement, paddle interaction, and scoring
void input(char map[V][H], int *paddleStart, int *paddleEnd, int *px, int *py, int *modx, int *mody, int *score, int *gameOver) {
    if (*py == 1 || *py == V - 2) {
        *mody *= -1;
    }

    // Player loses when the ball reaches the left wall
    if (*px == 1) {
        *gameOver = 1;
        playerwin = 0;
    }

    // Ball bounces off the right wall
    if (*px == H - 2) {
        *modx *= -1;
    }

    // Ball hits the paddle
    if (*px == 4 && *py >= *paddleStart && *py <= *paddleEnd) {
        *modx *= -1;
        (*score)++;
    }

    if (*gameOver == 0) {
        *px += (*modx);
        *py += (*mody);
    }

    handleArrowKeys(paddleStart, paddleEnd);
}

// Draw the edges of the game board
void edge(char map[V][H]) {
    for (int i = 0; i < H; i++) {
        map[0][i] = '-';
        map[V - 1][i] = '_';
    }
    for (int i = 0; i < V; i++) {
        map[i][0] = '|';
        map[i][H - 1] = '|';
    }
}

// Draw the player paddle
void player(char map[V][H], int paddleStart, int paddleEnd) {
    for (int i = paddleStart; i <= paddleEnd; i++) {
        map[i][3] = '#';
    }
}

// Draw the ball
void ball(char map[V][H], int px, int py) {
    map[py][px] = 'O';
}

// Busy-wait to delay game loop
void busy_wait(int milliseconds) {
    volatile int i, j;
    for (i = 0; i < milliseconds * 1000; i++) {
        for (j = 0; j < 100; j++) {}
    }
}

// Directly get a character from user input
int getchar_direct() {
    char c;
    read(0, &c, 1);
    return c;
}

// Check if a key is pressed
int kbhit() {
    int result = nonblock_read();
    return result != 0;
}

// Handle up and down arrow keys to move paddle
void handleArrowKeys(int *paddleStart, int *paddleEnd) {
    if (kbhit()) {
        char key = getchar_direct();
        if (key == '\033') {
            getchar_direct();
            switch(getchar_direct()) {
                case 'A':
                    if (*paddleStart > 1) {
                        (*paddleStart)--;
                        (*paddleEnd)--;
                    }
                    break;
                case 'B':
                    if (*paddleEnd < V - 2) {
                        (*paddleStart)++;
                        (*paddleEnd)++;
                    }
                    break;
            }
        }
    }
}
