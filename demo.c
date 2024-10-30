#include <stdio.h>
#include "fanctrl.h"

int main() {
    int mode = read_state();
    if (mode == -1) {
        printf("Failed to find \\\\.\\EnergyDrv\n");
        return 1;
    }

    if (mode == NORMAL) {
        printf("FAST mode on\n");
        keep_fast(-1);
    } else {
        printf("NORMAL mode on\n");
        fan_control(NORMAL);
    }
    return 0;
}

