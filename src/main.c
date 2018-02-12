#include <SDL2/SDL.h>
#include "ui/window.h"
#include "ui/controls.h"
#include "hw/cpu.h"
#include "hw/mem.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("No ch8 app given\n");
        return 1;
    }

    cpu_reset();
    if (!mem_load_rom(argv[1])) {
        printf("No ch8 app found\n");
        return 1;
    }

    window_init(argc > 2 && strcmp(argv[2], "-f") == 0);

    for (;;) {
        cpu_cycle();
        window_draw(cpu_pixels());
        if (controls_poll_event(cpu_keys()))
            break;
    }

    window_free();
    return 0;
}
