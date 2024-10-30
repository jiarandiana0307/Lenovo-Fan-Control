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

    if (outBuffer[0] == 3) {
        return FAST;
    }
    return NORMAL;
}

void keep_fast(int duration) {
    int interval = 9000; // ms, fine-tuned, see https://www.allstone.lt/ideafan/
    time_t start = time(NULL);
    while (1) {
        while (read_state() != FAST) {
            fan_control(FAST);
            Sleep(10);
        }
        if (duration > 0) {
            int left_time = duration - (time(NULL) - start) * 1000;
            if (left_time < interval) {
                Sleep(left_time);
                while (read_state() != NORMAL) {
                    fan_control(NORMAL);
                    Sleep(10);
                }
                break;
            }
        }
        Sleep(interval);
        fan_control(NORMAL); // Reset the fan to NORMAL mode, than switch to FAST mode as soon as possible.
    }
}

