#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>

void window_init(bool fullscreen);
void window_draw(uint8_t *pixels);
void window_free(void);

#endif /* WINDOW_H */
