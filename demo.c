#include <stdio.h>
#include "fanctrl.h"

int main() {
    int mode = read_state();
    switch (mode) {
        case -1:
            printf("Failed to open \\\\.\\EnergyDrv\n");
            break;
        case NORMAL:
            printf("FAST mode on\n");
            keep_fast(-1);
            break;
        case FAST:
            printf("NORMAL mode on\n");
            fan_control(NORMAL);
            break;
    }
    printf("Press Enter to exit...");
    getchar();
    return 0;
}

