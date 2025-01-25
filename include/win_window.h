#ifndef __WIN_WINDOW_H
#define __WIN_WINDOW_H

#include <windows.h>
#include "util.h"
#include "print.h"
#include "image.h"


typedef struct {
    HDC hdc;
    HBITMAP hBitmap;
    HDC memDC;
    HBITMAP oldBitmap;

    int x;
    int y;

    Image* canvas;
    int mouseX;
    int mouseY;

} Window;


Window* createWindow(Image* image, int x, int y) {
    if(x < 0 || y < 0) {
        printFunctionError("invalid parameter(s)", "createWindow(x: %d, y: %d)", x, y);
        return NULL;
    }
    Window* win = (Window*)malloc(sizeof(Window));

    if (!win) {
        printFunctionError("memory allocation failed", "createWindow(x: %d, y: %d)", x, y);
        free(win);
        return NULL;
    }

    win->hdc = GetDC(NULL);
    win->canvas = image;

    win->x = x;
    win->y = y;

    win->memDC = CreateCompatibleDC(win->hdc);

    return win;
}

int updateWindow(Window* win) {
    win->hBitmap = CreateBitmap(win->canvas->width, win->canvas->height, 1, 32, win->canvas->pixel);
    win->oldBitmap = (HBITMAP)SelectObject(win->memDC, win->hBitmap);

	BitBlt(win->hdc, win->x, win->y, win->canvas->width, win->canvas->height, win->memDC, 0, 0, SRCCOPY);

    // mouse pos
    POINT mouse;
    GetCursorPos(&mouse);
    win->mouseX = mouse.x -win->x;
    win->mouseY = mouse.y -win->y;

    // printf("Maus (%ld, %ld) => (%ld, %ld)\n", mouse.x, mouse.y, win->mouseX, win->mouseY);

    return 0;
}

int deleteWindow(Window* win) {
    SelectObject(win->memDC, win->oldBitmap);
	DeleteObject(win->hBitmap);
	DeleteDC(win->memDC);

    ReleaseDC(NULL, win->hdc);

    free(win);
    win = NULL;
    
    return 0;
}

void setWindowPos(Window* win, int x, int y) {
    win->x = x;
    win->y = y;
}

short keyPressed(short key) {
    return GetAsyncKeyState(key); 
}

#endif