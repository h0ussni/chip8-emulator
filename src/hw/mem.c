#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "mem.h"

#define MEM_SIZE  4069
#define FONT_SIZE 80

static uint8_t mem[MEM_SIZE];
static uint8_t fontset[FONT_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, /* 0 */
    0x20, 0x60, 0x20, 0x20, 0x70, /* 1 */
    0xF0, 0x10, 0xF0, 0x80, 0xF0, /* 2 */
    0xF0, 0x10, 0xF0, 0x10, 0xF0, /* 3 */
    0x90, 0x90, 0xF0, 0x10, 0x10, /* 4 */
    0xF0, 0x80, 0xF0, 0x10, 0xF0, /* 5 */
    0xF0, 0x80, 0xF0, 0x90, 0xF0, /* 6 */
    0xF0, 0x10, 0x20, 0x40, 0x40, /* 7 */
    0xF0, 0x90, 0xF0, 0x90, 0xF0, /* 8 */
    0xF0, 0x90, 0xF0, 0x10, 0xF0, /* 9 */
    0xF0, 0x90, 0xF0, 0x90, 0x90, /* A */
    0xE0, 0x90, 0xE0, 0x90, 0xE0, /* B */
    0xF0, 0x80, 0x80, 0x80, 0xF0, /* C */
    0xE0, 0x90, 0x90, 0x90, 0xE0, /* D */
    0xF0, 0x80, 0xF0, 0x80, 0xF0, /* E */
    0xF0, 0x80, 0xF0, 0x80, 0x80  /* F */
};

void mem_reset(void) {
    uint16_t i;

    for (i = 0; i < MEM_SIZE; mem[i++] = 0);
    for (i = 0; i < FONT_SIZE; ++i) {
        mem[i] = fontset[i];
    }
}

void mem_write(uint16_t addr, uint8_t val) {
    mem[addr] = val;
}

uint8_t mem_read(uint16_t addr) {
    return mem[addr];
}

bool mem_load_rom(const char *rom) {
    FILE *fp;
    uint16_t i;
    uint16_t size;
    unsigned char *buffer;

    fp = fopen(rom, "rb");
    if (!fp) return false;

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buffer = malloc(size * sizeof(unsigned char));
    fread(buffer, sizeof(*buffer), size, fp);

    for (i = 0; i < size; ++i)
        mem[0x200 | i] = buffer[i];

    fclose(fp);
    free(buffer);

    return true;
}
