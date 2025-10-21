#define UNICODE
#define _UNICODE
#include <windows.h>
#include <iostream>
#include <chrono>
#include <thread>


// Window procedure callback
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_INPUT:
            // Close window on any mouse movement
            DestroyWindow(hwnd);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Function to get idle screen time in seconds
DWORD idleScreentime ()
{    
    LASTINPUTINFO lastInput;
    lastInput.cbSize = sizeof(LASTINPUTINFO);
    DWORD currentTime = GetTickCount();
    DWORD idleTime = currentTime - lastInput.dwTime;

    return idleTime/1000;
}

// Function to create and show black screen
void ShowBlackScreen()
{
    // Register window class
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"BlackScreen";
    RegisterClassW(&wc);

    // Create full screen black window
    HWND hwnd = CreateWindowExW(
        WS_EX_TOPMOST,
        L"BlackScreen",
        L"Black Screen",
        WS_POPUP,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL,
        GetModuleHandle(NULL),
        NULL
    );

    // Register for raw mouse input
    RAWINPUTDEVICE rid;
    rid.usUsagePage = 0x01;
    rid.usUsage = 0x02; // Mouse
    rid.dwFlags = RIDEV_INPUTSINK;
    rid.hwndTarget = hwnd;
    RegisterRawInputDevices(&rid, 1, sizeof(rid));

    // Show window and set black background
    SetClassLongPtrW(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(RGB(0, 0, 0)));
    ShowWindow(hwnd, SW_SHOW);

    // Message loop
    MSG msg = {};
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

int main ()
{
    const DWORD idleLimit = 20;
    
    while (true)
    {
        DWORD idleTime = idleScreentime();
        bool isActive = false;

        if (idleTime >= idleLimit && !isActive)
        {
            isActive = true;
            ShowBlackScreen();
            isActive = false;
        }
    }
    return 0;
}