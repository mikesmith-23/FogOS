struct stat;

// system calls
int fork(void);
int exit(int) __attribute__((noreturn));
int wait(int*);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(const char*, char**);
int open(const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);

// pong.c
int nonblock_read(void);	// For non-blocking input
int clear_screen(void);         // Clears the terminal window
int gotoxy(int x, int y);	// Moves cursor to (x, y)
int delay(int milliseconds);    // Delays execution for specified milliseconds
int cursor_move(int x, int y);  // Moves the cursor to (x, y) for VGA
int enable_raw_mode(void);      // Enables raw input mode
int disable_raw_mode(void);     // Disables raw input mode

// ulib.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void fprintf(int, const char*, ...);
void printf(const char*, ...);
char* gets(char*, int max);
int fgets(int fd, char*, int max);
int getline(char **lineptr, uint *n, int fd);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);
int memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
