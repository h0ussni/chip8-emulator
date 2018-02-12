#ifndef MEM_H
#define MEM_H

#include <stdbool.h>

void mem_reset(void);
void mem_write(uint16_t addr, uint8_t val);
uint8_t mem_read(uint16_t addr);
bool mem_load_rom(const char *rom);

#endif /* MEM_H */
