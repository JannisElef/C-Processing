#ifndef __PROCESS_H
#define __PROCESS_H


#include <math.h>
#include "print.h"
#include "util.h"
#include "win_window.h"
#include "sketch.h"


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wconversion"


/* internal */
extern void settings();
extern int setup();
extern Image* draw();
/* internal */


static int displayWidth = 1920;
static int displayHeight = 1080;

static int mouseX;
static int mouseY;
static int pMouseX;
static int pMouseY;


typedef struct {
    int ret;    
    int loop;
    int quit;

	int width;
	int height;

    float internal_fps;
    unsigned long internal_frameTime;


    unsigned long runStartUS;

    char* name;
    Window* win;
} Process;


Process* createProcess(char *name) {
	Process* p = (Process*) malloc(sizeof(Process));
	p->name = name;
	return p;
}

int start(Process* p) {
    p->runStartUS = getSysTimeUS();	

    p->internal_fps = 60;
    p->internal_frameTime = 16666;	// default: 60fps

	unsigned long timeStart;
	unsigned long timeElapsed;	
	long timeToSleep;	
	struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 0;


    int firstCycle = 1;
    
    p->quit = 0;

	printTAG(p->name, PRINT_COLOR_SYSTEM, "starting process...\n");


	// god's loop
	do {

		if(firstCycle) {
			// printInfo("starting setup()");
			settings();	
			p->ret = setup();
			if(p->ret != 0) {				
				printFunctionError("an error occured", "setup() returned %d.", p->ret);				
                p->quit = 1;
			} else {
				// printInfo("finished setup() successfully");	
				Image* screen = createImage(p->width, p->height);

				// create window in the middle of the screen				
				int win_x = displayWidth/2 -p->width/2;
				int win_y = displayHeight/2 -p->height/2;
				p->win = createWindow(screen, p->name, win_x, win_y);		

			}			
		}
		if(!p->quit && !getWindowEvent(p->win, WINDOW_EVENT_QUIT)) { // not quit
			timeStart = getSysTimeUS();			

			if(p->loop || firstCycle) {
				firstCycle = 0;

				if(!getWindowEvent(p->win, WINDOW_EVENT_PAUSE)) { // not paused					
					p->win->canvas = draw();
    				p->win->frames++;
				}
			}

			if(checkValid(p->win->canvas, "draw")) {
				// error in draw() => quit with error (ret = -1)				
				p->ret = -1;
                p->quit = 1;	
			} else { // valid image ⬎
				
				POINT mouse;
    			GetCursorPos(&mouse);

				pMouseX = mouseX;
				pMouseY = mouseY;
				mouseX = mouse.x -p->win->window_x;
            	mouseY = mouse.y -p->win->window_y;
				
				if(mouseX >= 0 && mouseY >= 0 && mouseX <= p->width && mouseY <= p->height) {
					// printf("mouse (%d, %d) => %d %d %d %d\n", mouseX, mouseY, p->win->window_x, p->win->window_y, p->win->canvas->width, p->win->canvas->height);
					// mouse in window
					checkWindowInputs(p->win);

					if(!getWindowEvent(p->win, WINDOW_EVENT_PAUSE)) { // not paused
						p->win->mouseX = mouseX;
						p->win->mouseY = mouseY;
						p->win->pMouseY = pMouseY;
						p->win->pMouseY = pMouseY;
					}
					if(p->win->mouseEvent & 4) { // mouse pressed => move window
						int dx = mouseX -pMouseX;
						int dy = mouseY -pMouseY;

						p->win->window_x += dx;
						p->win->window_y += dy;
					}

					setWindowEvent(p->win, WINDOW_EVENT_FOCUS, 1);
				} else setWindowEvent(p->win, WINDOW_EVENT_FOCUS, 0);

				// moved: checkWindowInputs(p->win);

				updateWindow(p->win);

				timeElapsed = getSysTimeUS() - timeStart;		

				timeToSleep = (p->internal_frameTime -timeElapsed);
				if(timeToSleep > 0) {
					ts.tv_nsec = timeToSleep;
					nanosleep(&ts, NULL);
				}
			}
		} else { // quit => let window destroy itself
			updateWindow(p->win);
            p->quit = 1;
		}

	} while(p->quit == 0);
		
	unsigned long runtime = getSysTimeUS() -p->runStartUS;

	printf("\n");
	if(p->ret < 0) printTAG(p->name, PRINT_COLOR_ERROR, "process terminated in %d ms (=> %.3f s).\n", (int)floor(runtime/1000.0), runtime/1000000.0);	
	else printTAG(p->name, PRINT_COLOR_SYSTEM, "process finished after %d ms (=> %.3f s).\n", (int)floor(runtime/1000.0), runtime/1000000.0);	

    return p->ret;
}

#pragma GCC diagnostic pop

#endif