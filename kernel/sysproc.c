#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vga.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Pong Add-ons //

/**
 * Reads a character from UART in non-blocking mode.
 *
 * This function reads from the UART input without blocking. If an escape sequence
 * is detected, it processes it as part of an arrow key input.
 *
 * @return The character read, or 0 if no input is available.
 */
uint64
sys_nonblock_read(void)
{
  static int escape_seq = 0;  // Track escape sequence state
  int ch = uartgetc();        // Non-blocking read from UART

  if (ch == -1) {
    return 0;  // No input available
  }

  if (ch == '\033') {  // Escape character
    escape_seq = 1;     // Start escape sequence
    return 0;           // Continue reading
  }

  if (escape_seq == 1 && ch == '[') {
    escape_seq = 2;     // Continue escape sequence
    return 0;
  }

  if (escape_seq == 2) {
    escape_seq = 0;     // End escape sequence
    return ch;          // Return arrow key code ('A' for up, 'B' for down)
  }

  return ch;  // Return character if not part of an escape sequence
}

/**
 * Clears the screen using an ANSI escape sequence.
 *
 * Sends the necessary escape sequence to clear the terminal screen.
 *
 * @return Always returns 0.
 */
uint64
sys_clear_screen(void)
{
  uartputc_sync('\033');  // ESC
  uartputc_sync('[');
  uartputc_sync('2');
  uartputc_sync('J');     // ANSI escape sequence to clear the screen
  return 0;
}

/**
 * Moves the cursor to a specific (x, y) position on the screen.
 *
 * @param x The x-coordinate (column) to move the cursor to.
 * @param y The y-coordinate (row) to move the cursor to.
 *
 * @return Always returns 0.
 */
uint64
sys_gotoxy(void)
{
  int x, y;

  argint(0, &x);  // Get x-coordinate from user space
  argint(1, &y);  // Get y-coordinate from user space

  uartputc_sync('\033');   // ESC
  uartputc_sync('[');
  uartputc_sync('0' + (y + 1));  // Convert y to ASCII (1-based coord. index)
  uartputc_sync(';');
  uartputc_sync('0' + (x + 1));  // Convert x to ASCII (1-based coord. index)
  uartputc_sync('H');            // ANSI escape sequence to move cursor

  return 0;
}

/**
 * Delays the execution for a specified number of milliseconds.
 *
 * @param milliseconds The number of milliseconds to delay.
 *
 * @return Always returns 0.
 */
uint64
sys_delay(void)
{
  int milliseconds;

  argint(0, &milliseconds);  // Get milliseconds from user space
  int ticks_to_sleep = milliseconds / 10;  // Convert milliseconds to ticks

  for (int i = 0; i < ticks_to_sleep; i++) {
    sleep(0, &tickslock);  // Sleep one tick at a time
  }

  return 0;
}

/**
 * Draws a pixel at the specified (x, y) position on the VGA display.
 *
 * This function draws on the VGA screen at the provided coordinates.
 *
 * @param x The x-coordinate (column) to draw the pixel.
 * @param y The y-coordinate (row) to draw the pixel.
 *
 * @return Always returns 0.
 */
uint64
sys_vga_draw(void)
{
  int x, y, color;

  argint(0, &x);     // Get screen x-coordinate from user space
  argint(1, &y);     // Get screen y-coordinate from user space
  argint(2, &color); // Get color
  vga_draw(x, y, color);

  return 0;
}

/**
 * Moves the VGA cursor to a specific (x, y) position.
 *
 * @param x The x-coordinate (column) to move the cursor to.
 * @param y The y-coordinate (row) to move the cursor to.
 *
 * @return Always returns 0.
 */
uint64
sys_cursor_move(void)
{
  int x, y;

  argint(0, &x);  // Get x-coordinate from user space
  argint(1, &y);  // Get y-coordinate from user space

  vga_cursor_move(x, y);

  return 0;
}

/**
 * Enables raw input mode for the terminal.
 *
 * Configures the terminal to operate in raw mode, reading input character-by-character
 * without input echoing.
 *
 * @return Always returns 0.
 */
uint64
sys_enable_raw_mode(void)
{
  enable_raw_mode();  // enable raw mode
  return 0;
}

/**
 * Disables raw input mode and restores the terminal to normal mode.
 *
 * Restores the terminal's normal input behavior, including canonical
 * input processing and input echoing.
 *
 * @return Always returns 0.
 */
uint64
sys_disable_raw_mode(void)
{
  disable_raw_mode();  // disable raw mode
  return 0;
}
