/* A fan control application for Lenovo laptops, e.g. Ideapad, Xiaoxin and etc.
 * 
 * References:
 * https://github.com/bitrate16/FanControl/blob/main/FanControl/FanControl.cpp
 * https://github.com/Soberia/Lenovo-IdeaPad-Z500-Fan-Controller?tab=readme-ov-file#-about
 * https://www.allstone.lt/ideafan/
 */

#include <stdio.h>
#include <time.h>
#include <Windows.h>

#include "fanctrl.h"

volatile int is_keep_fan_running = 0;

static int NORMAL_MODE_EXPECTED_VALUE = -1;

int fan_control(enum FanMode mode) {
    HANDLE hndl = CreateFileW(L"\\\\.\\EnergyDrv", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hndl == INVALID_HANDLE_VALUE) {
        return -1;
    }
    // lpInBuffer value: 06 00 00 00  01 00 00 00  01 00 00 00 ~ [ 6, 1, 1 ] (inv endian)
    DWORD inBuffer[3] = { 6, 1 };
    inBuffer[2] = mode;
    DWORD bytesReturned = 0;

    DeviceIoControl(hndl, 0x831020C0, inBuffer, sizeof(inBuffer), NULL, 0, &bytesReturned, NULL);
    CloseHandle(hndl);

    return 1;
}

enum FanMode read_state() {
    if (NORMAL_MODE_EXPECTED_VALUE == -1) {
        // Set fan spinning mode to NORMAL to get NORMAL_MODE_EXPECTED_VALUE at the first run
        fan_control(NORMAL);
    }
    HANDLE hndl = CreateFileW(L"\\\\.\\EnergyDrv", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hndl == INVALID_HANDLE_VALUE) {
        return -1;
    }
    // lpInBuffer value: 0E 00 00 00 ~ [ 14 ] (inv endian)
    DWORD inBuffer[1] = { 14 };
    DWORD outBuffer[1];
    DWORD bytesReturned = 0;

    DeviceIoControl(hndl, 0x831020C4, inBuffer, sizeof(inBuffer), outBuffer, sizeof(outBuffer), &bytesReturned, NULL);
    CloseHandle(hndl);

    if (NORMAL_MODE_EXPECTED_VALUE == -1) {
        // Set this value when the fan is in normal mode at the first run
        NORMAL_MODE_EXPECTED_VALUE = outBuffer[0];
    }
    return outBuffer[0] == NORMAL_MODE_EXPECTED_VALUE ? NORMAL : FAST;
}

void keep_fan_running() {
    is_keep_fan_running = 1;
    const int interval = 9000; // ms, fine-tuned, see https://www.allstone.lt/ideafan/
    while (is_keep_fan_running) {
        while (read_state() != FAST) {
            fan_control(FAST);
            Sleep(10);
        }
        DWORD start = GetTickCount();

        for (int i = 0; i < interval / 1000 - 1; ++i) {
            Sleep(1000);
            if (!is_keep_fan_running) {
                fan_control(NORMAL);
                return;
            }
        }
        int delta = GetTickCount() - start;
        if (interval - delta > 0) {
            Sleep(interval - delta);
        }
        fan_control(NORMAL); // Reset the fan to NORMAL mode, than switch to FAST mode as soon as possible.
    }
    fan_control(NORMAL);
}

