#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <stdio.h>
#include <pthread.h>
#include <Windows.h>
#include <tchar.h>

#include "fanctrl.h"
#include "../res/resource.h"

#define WM_TRAYICON (WM_USER + 1)

#define VERSION "v0.3"

enum TrayMenuIDs {
    ID_TRAY_APP_ICON = 1001,
    ID_TRAY_STATE,
    ID_TRAY_LOW_SPEED,
    ID_TRAY_HIGH_SPEED,
    ID_TRAY_NORMAL_SPEED,
    ID_TRAY_ABOUT,
    ID_TRAY_EXIT,
};

enum HotKeyIDs {
    HOTKEY_LOW_SPEED,
    HOTKEY_HIGH_SPEED,
    HOTKEY_NORMAL_SPEED,
};

typedef struct {
    LPCWSTR app_name;
    LPCWSTR note;
    LPCWSTR program_is_running;
    LPCWSTR failed_to_open_driver;
    LPCWSTR state;
    LPCWSTR menu_at_low_speed;
    LPCWSTR menu_at_high_speed;
    LPCWSTR menu_at_normal_speed;
    LPCWSTR menu_low_speed;
    LPCWSTR menu_high_speed;
    LPCWSTR menu_normal_speed;
    LPCWSTR menu_about;
    LPCWSTR menu_exit;
    LPCWSTR about_text;
} LangResources;

const LangResources en_US = {
    TEXT("Lenovo Fan Control"),
    TEXT("Note"),
    TEXT("The program is running."),
    TEXT("Failed to open \\\\.\\EnergyDrv. Unsupported device or something wrong with Lenovo ACPI-Compliant Virtual Power Controller driver."),
    TEXT("State"),
    TEXT("Low Speed"),
    TEXT("High Speed"),
    TEXT("Normal Speed"),
    TEXT("Low Speed\tCtrl+Alt+F10"),
    TEXT("High Speed\tCtrl+Alt+F11"),
    TEXT("Normal Speed\tCtrl+Alt+F12"),
    TEXT("About"),
    TEXT("Exit"),
    TEXT("Lenovo Fan Control " VERSION "\n\n\
Control fan for Lenovo laptops with Lenovo ACPI-Compliant Virtual Power Controller driver on Windows.\n\n\
Open Source: https://github.com/jiarandiana0307/Lenovo-Fan-Control\n\n\
Disclaimer: This program is not responsible for possible damage of any kind, use it at your own risk.")
};

const LangResources zh_CN = {
    TEXT("联想风扇控制"),
    TEXT("提示"),
    TEXT("程序已经在运行中。"),
    TEXT("无法访问\\\\.\\EnergyDrv。本设备不支持或Lenovo ACPI-Compliant Virtual Power Controller驱动异常。"),
    TEXT("状态"),
    TEXT("低转速"),
    TEXT("高转速"),
    TEXT("正常转速"),
    TEXT("低转速\tCtrl+Alt+F10"),
    TEXT("高转速\tCtrl+Alt+F11"),
    TEXT("正常转速\tCtrl+Alt+F12"),
    TEXT("关于"),
    TEXT("退出"),
    TEXT("联想风扇控制 " VERSION "\n\n\
在Windows上通过Lenovo ACPI-Compliant Virtual Power Controller驱动控制联想笔记本电脑的风扇。\n\n\
本程序已开源：https://github.com/jiarandiana0307/Lenovo-Fan-Control\n\n\
免责声明：本程序不对任何可能的损坏负责，风险自担。")
};

const LangResources* lang = &en_US;

NOTIFYICONDATA nid;
HMENU hMenu;
pthread_t keep_fan_speed_low_thread;
pthread_t keep_fan_running_thread;

void* keep_fan_speed_low_func(void *arg) {
    keep_fan_speed_low();
}

void* keep_fan_running_func(void *arg) {
    keep_fan_running();
}

void toggle_fan_low_speed() {
    ModifyMenu(hMenu, ID_TRAY_STATE, MF_STRING | MF_DISABLED, ID_TRAY_STATE, lang->menu_at_low_speed);
    if (!is_keep_fan_speed_low) {
        pthread_create(&keep_fan_speed_low_thread, NULL, keep_fan_speed_low_func, NULL);
    }
    _stprintf(nid.szTip, 64, TEXT("%s " VERSION "\n%s: %s"), lang->app_name, lang->state, lang->menu_at_low_speed);
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void toggle_fan_high_speed() {
    ModifyMenu(hMenu, ID_TRAY_STATE, MF_STRING | MF_DISABLED, ID_TRAY_STATE, lang->menu_at_high_speed);
    if (!is_keep_fan_running) {
        pthread_create(&keep_fan_running_thread, NULL, keep_fan_running_func, NULL);
    }
    _stprintf(nid.szTip, 64, TEXT("%s " VERSION "\n%s: %s"), lang->app_name, lang->state, lang->menu_at_high_speed);
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void toggle_fan_normal_speed() {
    ModifyMenu(hMenu, ID_TRAY_STATE, MF_STRING | MF_DISABLED, ID_TRAY_STATE, lang->menu_at_normal_speed);
    if (is_keep_fan_running) {
        is_keep_fan_running = 0;
    }
    if (is_keep_fan_speed_low) {
        is_keep_fan_speed_low = 0;
    }
    fan_control(NORMAL);
    _stprintf(nid.szTip, 64, TEXT("%s " VERSION "\n%s: %s"), lang->app_name, lang->state, lang->menu_at_normal_speed);
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            nid.cbSize = sizeof(NOTIFYICONDATA);
            nid.hWnd = hwnd;
            nid.uID = ID_TRAY_APP_ICON;
            nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
            nid.uCallbackMessage = WM_TRAYICON;
            nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON));
            _tcscpy(nid.szTip, lang->app_name);
            Shell_NotifyIcon(NIM_ADD, &nid);

            hMenu = CreatePopupMenu();
            AppendMenu(hMenu, MF_STRING | MF_DISABLED, ID_TRAY_STATE, lang->menu_at_high_speed);
            AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenu(hMenu, MF_STRING, ID_TRAY_LOW_SPEED, lang->menu_low_speed);
            AppendMenu(hMenu, MF_STRING, ID_TRAY_HIGH_SPEED, lang->menu_high_speed);
            AppendMenu(hMenu, MF_STRING, ID_TRAY_NORMAL_SPEED, lang->menu_normal_speed);
            AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenu(hMenu, MF_STRING, ID_TRAY_ABOUT, lang->menu_about);
            AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, lang->menu_exit);

            toggle_fan_high_speed();
            break;
        }

        case WM_TRAYICON: {
            if (lParam == WM_LBUTTONUP || lParam == WM_RBUTTONUP) {
                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hwnd);
                TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, 
                              pt.x, pt.y, 0, hwnd, NULL);
                PostMessage(hwnd, WM_NULL, 0, 0);
            }
            break;
        }

        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case ID_TRAY_EXIT:
                    Shell_NotifyIcon(NIM_DELETE, &nid);
                    DestroyWindow(hwnd);
                    break;

                case ID_TRAY_LOW_SPEED:
                    toggle_fan_low_speed();
                    break;

                case ID_TRAY_HIGH_SPEED:
                    toggle_fan_high_speed();
                    break;

                case ID_TRAY_NORMAL_SPEED:
                    toggle_fan_normal_speed();
                    break;

                case ID_TRAY_ABOUT:
                    MessageBox(hwnd, lang->about_text, lang->menu_about, MB_OK | MB_ICONINFORMATION);
                    break;
            }
            break;
        }

        case WM_HOTKEY:
            switch (wParam) {
                case HOTKEY_LOW_SPEED:
                    toggle_fan_low_speed();
                    break;

                case HOTKEY_HIGH_SPEED:
                    toggle_fan_high_speed();
                    break;

                case HOTKEY_NORMAL_SPEED:
                    toggle_fan_normal_speed();
                    break;
            }
            break;

        case WM_DESTROY:
            if (read_state() != NORMAL) {
                fan_control(NORMAL);
            }
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    LANGID system_lang = GetUserDefaultLangID();
    if (PRIMARYLANGID(system_lang) == LANG_CHINESE) {
        lang = &zh_CN;
    }

    HANDLE hMutex = CreateMutex(NULL, TRUE, TEXT("LenovoFanControlMutex"));
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(hMutex);
        MessageBox(NULL, lang->program_is_running, TEXT("提示"), MB_OK | MB_ICONINFORMATION);
        return 0;
    }

    if (read_state() == -1) {
        MessageBox(NULL, lang->failed_to_open_driver, lang->app_name, MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("LenovoFanControlClass");

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, TEXT("Window Registration Failed!"), TEXT("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    HWND hwnd = CreateWindowEx(0, TEXT("LenovoFanControlClass"), lang->app_name, 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
    if (hwnd == NULL) {
        MessageBox(NULL, TEXT("Window Creation Failed!"), TEXT("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    RegisterHotKey(hwnd, HOTKEY_LOW_SPEED, MOD_CONTROL | MOD_ALT, VK_F10);
    RegisterHotKey(hwnd, HOTKEY_HIGH_SPEED, MOD_CONTROL | MOD_ALT, VK_F11);
    RegisterHotKey(hwnd, HOTKEY_NORMAL_SPEED, MOD_CONTROL | MOD_ALT, VK_F12);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterHotKey(hwnd, HOTKEY_LOW_SPEED);
    UnregisterHotKey(hwnd, HOTKEY_HIGH_SPEED);
    UnregisterHotKey(hwnd, HOTKEY_NORMAL_SPEED);

    if (hMutex) {
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
    }
    return msg.wParam;
}
