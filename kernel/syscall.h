// System call numbers
#define SYS_fork    1
#define SYS_exit    2
#define SYS_wait    3
#define SYS_pipe    4
#define SYS_read    5
#define SYS_kill    6
#define SYS_exec    7
#define SYS_fstat   8
#define SYS_chdir   9
#define SYS_dup    10
#define SYS_getpid 11
#define SYS_sbrk   12
#define SYS_sleep  13
#define SYS_uptime 14
#define SYS_open   15
#define SYS_write  16
#define SYS_mknod  17
#define SYS_unlink 18
#define SYS_link   19
#define SYS_mkdir  20
#define SYS_close  21

// Pong system calls
#define SYS_nonblock_read 22  // For non-blocking input reading
#define SYS_clear_screen  23  // For clearing the terminal screen
#define SYS_gotoxy        24  // For moving the cursor
#define SYS_delay         25  // For delaying (replacing usleep)
#define SYS_vga_draw	  26  // For drawing on the VGA terminal
#define SYS_cursor_move   27  // For moving the cursor (VGA)

// Raw mode
#define SYS_enable_raw_mode 28
#define SYS_disable_raw_mode 29
