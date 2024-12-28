#ifndef __WIN_WINDOW_H
#define __WIN_WINDOW_H

#include <windows.h>
#include "util.h"
#include "print.h"
#include "image.h"


typedef struct {
    HWND hwnd;
    HINSTANCE hInstance;
    Image* canvas;
    HBITMAP hBitmap;
} Window;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            Window* win = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            if (win && win->canvas && win->canvas->pixel) {
                HDC memDC = CreateCompatibleDC(hdc);
                HGDIOBJ oldBitmap = SelectObject(memDC, win->hBitmap);

                #warning neue pixel array struktur (1 int, nicht mehr 3/4 byte)

                BitBlt(hdc, 0, 0, win->canvas->width, win->canvas->height, memDC, 0, 0, SRCCOPY);
                SelectObject(memDC, oldBitmap);
                DeleteDC(memDC);
            }
            EndPaint(hwnd, &ps);
        }
        return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

Window createWindow(char* name, int width, int height) {
    Window win;
    win.hInstance = GetModuleHandle(NULL);
    const char CLASS_NAME[] = "Sample Window Class";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = win.hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    win.hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        name,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL,
        NULL,
        win.hInstance,
        NULL
    );

    if (win.hwnd == NULL) {
        printFunctionError("couldn't create window", "createWindow(name: \"%s\", h: %d, w: %d)", name, width, height);
    }

    Image canvas_;
    createImage(&canvas_, width, height);
    win.canvas = &canvas_;

    HDC hdc = GetDC(win.hwnd);
    win.hBitmap = CreateCompatibleBitmap(hdc, width, height);

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // Negative height to indicate top-down bitmap
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24; // 24 bits per pixel (RGB)
    bmi.bmiHeader.biCompression = BI_RGB;

    #warning neue pixel array struktur (1 int, nicht mehr 3/4 byte)

    SetDIBits(hdc, win.hBitmap, 0, height, win.canvas->pixel, &bmi, DIB_RGB_COLORS);

    ReleaseDC(win.hwnd, hdc);

    SetWindowLongPtr(win.hwnd, GWLP_USERDATA, (LONG_PTR)&win);

    ShowWindow(win.hwnd, SW_SHOW);
    return win;
}

void drawWindow(Window* win) {
    InvalidateRect(win->hwnd, NULL, FALSE);
}

void handleWindow(Window* win) {
    MSG msg = {0};
    while (1) {        
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) {
                return;
            }
        }
                
        drawWindow(win);
    }
}

#endif