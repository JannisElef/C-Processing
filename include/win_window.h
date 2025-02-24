#ifndef __WIN_WINDOW_H
#define __WIN_WINDOW_H

#include <windows.h>
#include "util.h"
#include "print.h"
#include "image.h"
#include "imageFile.h"


#define WINDOW_EVENT_FOCUS    128
#define WINDOW_EVENT_PAUSE    64
#define WINDOW_EVENT_QUIT     1


typedef struct {
    HDC hdc;
    HBITMAP hBitmap;
    HDC memDC;
    HBITMAP oldBitmap;

    char* windowTag;

    int window_x;
    int window_y;

    Image* canvas;

    int mouseX;
    int mouseY;
    int pMouseX;
    int pMouseY;

    /*
		0babcdefgh
        ...
        f: MB left
        g: MB middle
        h: MB right
        0 = no, 1 = yes
    */ char mouseEvent;
    long keyBlock;

    /*
        0babcdefgh
        a: is focused
        b: is paused
        ...
        h: quit
        0 = no, 1 = yes
    */ char windowEvent;

    long frames;

} Window;


int isPointInWindow(Window* win, int x, int y);

int getWindowEvent(Window* win, int tag);

void setWindowEvent(Window* win, int tag, int val);

void checkWindowInputs(Window* win);

int destroyWindow(Window* win);



Window* createWindow(Image* image, char *name, int x, int y) {
    if(x < 0 || y < 0) {
        printFunctionWarning("window out of screen", "createWindow(x: %d, y: %d)", x, y);
        return NULL;
    }
    Window* win = (Window*)malloc(sizeof(Window));

    if (!win) {
        printFunctionError("memory allocation failed", "createWindow(x: %d, y: %d)", x, y);
        free(win);
        return NULL;
    }

    win->windowTag = name;

    win->windowEvent = 0;
    win->mouseEvent = 0;
    win->frames = 0;

    win->hdc = GetDC(NULL);
    win->canvas = image;

    win->window_x = x;
    win->window_y = y;

    win->memDC = CreateCompatibleDC(win->hdc);

    return win;
}

int updateWindow(Window* win) {
    if(getWindowEvent(win, WINDOW_EVENT_QUIT)) {
        destroyWindow(win);
        return 0;
    }
    
    win->hBitmap = CreateBitmap(win->canvas->width, win->canvas->height, 1, 32, win->canvas->pixel);
    win->oldBitmap = (HBITMAP)SelectObject(win->memDC, win->hBitmap);

	BitBlt(win->hdc, win->window_x, win->window_y, win->canvas->width, win->canvas->height, win->memDC, 0, 0, SRCCOPY);

    // // mouse pos
    // POINT mouse;
    // GetCursorPos(&mouse);

    // if(isPointInWindow(win, mouse.x, mouse.y)) {
    //     if(!getWindowEvent(win, WINDOW_EVENT_PAUSE)) { // not paused
    //         win->pMouseX = win->mouseX;
    //         win->pMouseY = win->mouseY;
    //         win->mouseX = mouse.x -win->window_x;
    //         win->mouseY = win->canvas->height -(mouse.y -win->window_y);
    //         // printf("Maus (%ld, %ld) => (%ld, %ld)\n", mouse.x, mouse.y, win->mouseX, win->mouseY);

    //         if(win->mouseEvent & 4) { // mouse pressed
    //             int dx = win->mouseX -win->pMouseX;
	// 			int dy = win->mouseY -win->pMouseY;
					
	// 		    win->window_x += dx;
	// 			win->window_y -= dy;
    //         }
    //         win->frames++;
    //     }
    //     setWindowEvent(win, WINDOW_EVENT_FOCUS, 1);
    // } else { // window is not in focus => inactive        
    //     setWindowEvent(win, WINDOW_EVENT_FOCUS, 0);
    // }

    return 0;
}

int destroyWindow(Window* win) {
    SelectObject(win->memDC, win->oldBitmap);
	DeleteObject(win->hBitmap);
	DeleteDC(win->memDC);

    ReleaseDC(NULL, win->hdc);

    free(win->canvas);
    free(win);
    win = NULL;
    
    return 0;
}

int isPointInWindow(Window* win, int x, int y) {
    return isPIP(x, y, win->window_x, win->window_y, win->canvas->width, win->canvas->height);
}

void setWindowPos(Window* win, int x, int y) {
    win->window_x = x;
    win->window_y = y;
}


void setWindowEvent(Window* win, int tag, int val) {
    if(val) win->windowEvent |= tag;
    else win->windowEvent &= ~tag;
}

int getWindowEvent(Window* win, int tag) {
    return win->windowEvent & tag;
}

short isPressed(short key) {
    return (GetAsyncKeyState(key) & 0x8000); 
}

void checkWindowInputs(Window* win) {

    // if(!getWindowEvent(win, WINDOW_EVENT_FOCUS)) return; // not focused

    if(isPressed(VK_LBUTTON)) {
		win->mouseEvent |= 4;
	} else win->mouseEvent &= ~4;
	if(isPressed(VK_RBUTTON)) {
		win->mouseEvent |= 1;
	} else win->mouseEvent &= ~1;
	if(isPressed(VK_MBUTTON)) {
		win->mouseEvent |= 2;
	} else win->mouseEvent &= ~2;

	// prevent repetetive pressing of keys
	// delay between keypresses in us ⬎
	if(getSysTimeUS() -win->keyBlock >= 333333) {
		win->keyBlock = getSysTimeUS();

		if(isPressed(VK_ESCAPE)) {
		    setWindowEvent(win, WINDOW_EVENT_QUIT, 1);
		} else if(isPressed(VK_RETURN)) {
			saveBMP(win->canvas, "C:\\Users\\eleftherakis\\Downloads\\sampafale1.bmp", 3);
			printInfo("%s: saved image", win->windowTag);
		} else if(isPressed(VK_SPACE)) {
			if(getWindowEvent(win, WINDOW_EVENT_PAUSE)) { // paused
				setWindowEvent(win, WINDOW_EVENT_PAUSE, 0);
				printInfo("%s: unpaused", win->windowTag);
			} else {
                setWindowEvent(win, WINDOW_EVENT_PAUSE, 1);
				printInfo("%s: paused", win->windowTag);
			}
		} else { // no special key pressed => no block needed
			win->keyBlock = 0;
		}
	}
}

#endif