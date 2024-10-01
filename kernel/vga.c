#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "memlayout.h"
#include "stdint.h"

// VGA screen dimensions
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// VGA video memory address
#define VGA_MEMORY (0xB8000 + KERNBASE)  // Offset for VGA memory

// VGA color codes
#define VGA_COLOR_BLACK 0
#define VGA_COLOR_WHITE 15

// VGA screen buffer is 80x25 characters, each with two bytes (char and attribute)
volatile uint16_t *vga_buffer = (uint16_t *) VGA_MEMORY;

// Set a character with color at x, y position on the VGA screen
void vga_draw(int x, int y, int color) {
    if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT) {
        // Out of bounds check
        return;
    }

    // Calculate the position in the VGA buffer
    int offset = y * VGA_WIDTH + x;

    // Draw a blank space with the provided color
    vga_buffer[offset] = (color << 8) | ' ';
}

// Memory-mapped I/O for RISC-V
void vga_cursor_move(int x, int y) {
    // TODO: figure out outb logic
    // Apparently, RISC-V doesn't support outb for cursor movement.
    return;
}

// Clear the VGA screen w/ spaces and background color
void vga_clear_screen() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            vga_draw(x, y, VGA_COLOR_BLACK);  // Clear the screen w/ black background
        }
    }
}
