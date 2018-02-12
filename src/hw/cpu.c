#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "cpu.h"
#include "mem.h"

#define CARRY_FLAG  0xF  /* 15 */

#define VREG_SIZE  16
#define STACK_SIZE 16
#define PX_SIZE    2048
#define KEY_SIZE   16

static uint8_t  v[VREG_SIZE];
static uint16_t stack[STACK_SIZE];
static uint8_t  pixels[PX_SIZE];
static uint8_t  keys[KEY_SIZE];

static uint16_t reg_pc;
static uint16_t reg_sp;
static uint16_t reg_i;

static bool    draw_flag;
static uint8_t delay_timer;
static uint8_t sound_timer;

uint8_t *cpu_pixels(void) {
    if (!draw_flag)
        return NULL;

    draw_flag = false;
    return pixels;
}

uint8_t *cpu_keys(void) {
    return keys;
}

void cpu_clear_pixels(void) {
    uint16_t i;
    for (i = 0; i < PX_SIZE; pixels[i++] = 0);
}

uint16_t cpu_stack_pop(void) {
    if (reg_sp == 0)
        return 0;

    --reg_sp;
    return stack[reg_sp];
}

void cpu_stack_push(uint16_t val) {
    stack[reg_sp] = val;
    ++reg_sp;
}

void cpu_reset(void) {
    uint8_t i;

    mem_reset();
    cpu_clear_pixels();
    srand(time(NULL));

    for (i = 0; i < VREG_SIZE; v[i++] = 0);
    for (i = 0; i < STACK_SIZE; stack[i++] = 0);
    for (i = 0; i < KEY_SIZE; keys[i++] = 0);

    reg_pc = 0x200;
    reg_sp = 0;
    reg_i = 0;

    draw_flag   = true;
    delay_timer = 0;
    sound_timer = 0;
}

uint16_t cpu_get_opcode(void) {
    return (mem_read(reg_pc) << 8) | mem_read(reg_pc + 1);
}

void opc_0(uint16_t opcode, uint8_t vx, uint8_t vy) {
    switch (opcode & 0x000F) {
        /* Clears the screen */
        case 0x0000: {
            cpu_clear_pixels();
            draw_flag = true;
        } break;
        /* Returns from a subroutine */
        case 0x000E: {
            reg_pc = cpu_stack_pop();
        } break;
        /* Calls RCA 1802 program at address NNN */
        default: {
            reg_pc = opcode & 0x0FFF;
            reg_pc -= 2;
        } break;
    }
}

void opc_1(uint16_t opcode, uint8_t vx, uint8_t vy) {
    /* Jumps to address NNN */
    reg_pc = opcode & 0x0FFF;
    reg_pc -= 2;
}

void opc_2(uint16_t opcode, uint8_t vx, uint8_t vy) {
    /* Calls subroutine at NNN */
    cpu_stack_push(reg_pc);
    reg_pc = opcode & 0x0FFF;
    reg_pc -= 2;
}

void opc_3(uint16_t opcode, uint8_t vx, uint8_t vy) {
    /* Skips the next instruction if VX equals NN */
    reg_pc += ((opcode & 0x00FF) == v[vx]) ? 2 : 0;
}

void opc_4(uint16_t opcode, uint8_t vx, uint8_t vy) {
    /* Skips the next instruction if VX doesn't equals NN */
    reg_pc += ((opcode & 0x00FF) == v[vx]) ? 0 : 2;
}

void opc_5(uint16_t opcode, uint8_t vx, uint8_t vy) {
    /* Skips the next instruction if VX equals VY */
    reg_pc += (v[vx] == v[vy]) ? 2 : 0;
}

void opc_6(uint16_t opcode, uint8_t vx, uint8_t vy) {
    /* Sets VX to NN */
    v[vx] = (opcode & 0x00FF);
}

void opc_7(uint16_t opcode, uint8_t vx, uint8_t vy) {
    /* Adds NN to VX */
    v[vx] += (opcode & 0x00FF);
}

void opc_8(uint16_t opcode, uint8_t vx, uint8_t vy) {
    switch (opcode & 0x000F) {
        /* Sets VX to the value of VY */
        case 0x0000: {
            v[vx] = v[vy];
        } break;
        /* Sets VX to VX or VY */
        case 0x0001: {
            v[vx] |= v[vy];
        } break;
        /* Sets VX to VX and VY */
        case 0x0002: {
            v[vx] &= v[vy];
        } break;
        /* Sets VX to VX xor VY */
        case 0x0003: {
            v[vx] ^= v[vy];
        } break;
        /* Adds VY to VX. VF is set to 1
            when there's a carry, and to 0 when there isn't */
        case 0x0004: {
            v[CARRY_FLAG] = v[vy] > v[vx];
            v[vx] += v[vy];
        } break;
        /* VY is subtracted from VX. VF is set to 0
            when there's a borrow, and 1 when there isn't */
        case 0x0005: {
            v[CARRY_FLAG] = v[vx] >= v[vy];
            v[vx] -= v[vy];
        } break;
        /* Shifts VX right by one. VF is set to the value of
            the least significant bit of VX before the shift */
        case 0x0006: {
            v[CARRY_FLAG] = v[vx] & 0x1;
            v[vx] >>= 1;
        } break;
        /* Sets VX to VY minus VX. VF is set to 0
            when there's a borrow, and 1 when there isn't */
        case 0x0007: {
            v[CARRY_FLAG] = v[vx] <= v[vy];
            v[vx] = v[vy] - v[vx];
        } break;
        /* Shifts VX left by one. VF is set to the value of
            the most significant bit of VX before the shift */
        case 0x000E: {
            v[CARRY_FLAG] = v[vx] >> 7;
            v[vx] <<= 1;
        } break;
    }
}

void opc_9(uint16_t opcode, uint8_t vx, uint8_t vy) {
    /* Skips the next instruction if VX doesn't equal VY */
    reg_pc += (v[vx] != v[vy]) ? 2 : 0;
}

void opc_A(uint16_t opcode, uint8_t vx, uint8_t vy) {
    /* Sets I to the address NNN */
    reg_i = opcode & 0x0FFF;
}

void opc_B(uint16_t opcode, uint8_t vx, uint8_t vy) {
    /* Jumps to the address NNN plus V0 */
    reg_pc = (opcode & 0x0FFF) + v[0x0];
    reg_pc -= 2;
}

void opc_C(uint16_t opcode, uint8_t vx, uint8_t vy) {
    /* Sets VX to the result of a bitwise and
        operation on a random number and NN */
    v[vx] = (rand() % 0xFF) & (opcode & 0x00FF);
}

void opc_D(uint16_t opcode, uint8_t vx, uint8_t vy) {
    /* Sprites stored in memory at location in index register (I),
        8bits wide. Wraps around the screen. If when drawn,
        clears a pixel, register VF is set to 1 otherwise it is zero.
        All drawing is XOR drawing (i.e. it toggles the screen pixels).
        Sprites are drawn starting at position VX, VY. N is
        the number of 8bit rows that need to be drawn. If N is
        greater than 1, second line continues at position VX, VY+1,
        and so on */

    uint8_t pixel = 0;
    uint8_t xline = 0;
    uint8_t yline = 0;

    v[CARRY_FLAG] = 0;

    for (yline = 0; yline < (opcode & 0x000F); ++yline) {
        pixel = mem_read(reg_i + yline);
        for (xline = 0; xline < 8; ++xline) {
            if ((pixel & (0x80 >> xline))) {
                #define pos (v[vx] + xline + ((v[vy] + yline) * 64))
                if (pixels[pos])
                    v[CARRY_FLAG] = 1;

                pixels[pos] ^= 1;
                #undef pos
            }
        }
    }

    draw_flag = true;
}

void opc_E(uint16_t opcode, uint8_t vx, uint8_t vy) {
    switch (opcode & 0x000F) {
        /* Skips the next instruction if the key stored
            in VX is pressed */
        case 0x000E: {
            reg_pc += (keys[v[vx]]) ? 2 : 0;
        } break;
        /* Skips the next instruction if the key stored
            in VX isn't pressed */
        case 0x0001: {
            reg_pc += (keys[v[vx]]) ? 0 : 2;
        } break;
    }
}

void opc_F(uint16_t opcode, uint8_t vx, uint8_t vy) {
    uint8_t i;
    bool pressed = false;

    switch (opcode & 0x00FF) {
        /* Sets VX to the value of the delay timer */
        case 0x0007: {
            v[vx] = delay_timer;
        } break;
        /* A key press is awaited, and then stored in VX */
        case 0x000A: {
            pressed = false;
            for (i = 0; i < KEY_SIZE; ++i) {
                if (keys[i]) {
                    pressed = true;
                    v[vx] = i;
                }
            }
            if (!pressed) {
                return;
            }
        } break;
        /* Sets the delay timer to VX */
        case 0x0015: {
            delay_timer = v[vx];
        } break;
        /* Sets the sound timer to VX */
        case 0x0018: {
            sound_timer = v[vx];
        } break;
        /* Adds VX to I */
        case 0x001E: {
            v[CARRY_FLAG] = (reg_i + v[vx]) > 0xFFF;
            reg_i += v[vx];
        } break;
        /* Sets I to the location of the sprite for the
            character in VX. Characters 0-F (in hexadecimal)
            are represented by a 4x5 font */
        case 0x0029: {
            reg_i = v[vx] * 0x5;
        } break;
        /* Stores the binary-coded decimal representation
            of VX, with the most significant of three digits
            at the address in I, the middle digit at I plus 1,
            and the least significant digit at I plus 2.
            (In other words, take the decimal representation of VX,
            place the hundreds digit in memory at location in I,
            the tens digit at location I+1, and the ones digit
            at location I+2.) */
        case 0x0033: {
            mem_write(reg_i,     (v[vx] / 100));
            mem_write(reg_i + 1, (v[vx] / 10) % 10);
            mem_write(reg_i + 2, (v[vx] % 100) % 10);
        } break;
        /* Stores V0 to VX (including VX) in memory
            starting at address I */
        case 0x0055: {
            for (i = 0; i <= vx; ++i) {
                mem_write(reg_i + i, v[i]);
            }
            reg_i += v[vx] + 1;
        } break;
        /* Fills V0 to VX (including VX) with values from
            memory starting at address I */
        case 0x0065: {
            for (i = 0; i <= vx; ++i) {
                v[i] = mem_read(reg_i + i);
            }
            reg_i += v[vx] + 1;
        } break;
    }
}

static void (*instructions[16])(uint16_t, uint8_t, uint8_t) = {
    opc_0, opc_1, opc_2, opc_3, opc_4,
    opc_5, opc_6, opc_7, opc_8, opc_9,
    opc_A, opc_B, opc_C, opc_D, opc_E,
    opc_F
};

void cpu_cycle(void) {
    uint16_t opcode = cpu_get_opcode();
    uint8_t vx = (opcode & 0x0F00) >> 8;
	uint8_t vy = (opcode & 0x00F0) >> 4;

    printf("OPCODE: 0x%04X\n", opcode);

    if (opcode == 0x0000) {
        goto END_OF_CYCLE;
    }

    instructions[(opcode & 0xF000) >> 12](opcode, vx, vy);
    reg_pc += 2;

END_OF_CYCLE:

    if (delay_timer > 0) {
        --delay_timer;
    }

    if (sound_timer > 0) {
        if (sound_timer == 1) {
            printf("\a\n");
        }
        --sound_timer;
    }
}
