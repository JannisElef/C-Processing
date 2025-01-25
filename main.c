

#include <time.h>
#include <windows.h>

#include "playground.h"

#include "include/print.h"
#include "include/util.h"
#include "include/image.h"
#include "include/win_window.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wconversion"



Window* win;

//compile: " gcc -o main main.c -lgdi32 "


int run = 1;
int pause = 0;
int firstCycle = 1;
int ret = 0;


long keyBlock = 0;
void checkKeys();

int main() {

	unsigned long runStartUS = getSysTimeUS();	
	unsigned long sketchStartUS;	

	unsigned long timeStart;
	unsigned long timeElapsed;	
	long timeToSleep;	
	struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 0;


	printTAG("SYSTEM", PRINT_COLOR_SYSTEM, "starting...\n");


	// god's loop
	do {
		run = 0;

		if(firstCycle) {
			// printInfo("starting setup()");
			settings();	
			ret = setup();
			if(ret != 0) {				
				printFunctionError("an error occured", "setup() returned %d.", ret);
				quit = 1;
			} else {
				// printInfo("finished setup() successfully");	
				global_x = displayWidth/2 -width/2;
				global_y = displayHeight/2 -height/2;

				screen = createImage(width, height);
				win = createWindow(screen, global_x, global_y);					
			}
			sketchStartUS = getSysTimeUS();			
		}
		if(quit == 0) {
			timeStart = getSysTimeUS();
			run = 1;

			if(loop || firstCycle) {
				firstCycle = 0;

				if(!pause) {
					win->canvas = draw();				
					frame++;
				}
			}

			if(checkValid(win->canvas, "draw")) {
				quit = 1;	
				ret = -1;	
			} else { // valid image ⬎
				
				checkKeys();

				updateWindow(win);

				if(!pause) {
					mouseX = win->mouseX;
					mouseY = win->mouseY;
				}

				timeElapsed = getSysTimeUS() - timeStart;		

				timeToSleep = (internal_frameTime -timeElapsed);
				if(timeToSleep > 0) {
					ts.tv_nsec = timeToSleep;
					nanosleep(&ts, NULL);
				}
			}
		} else {
			deleteWindow(win);
		}

	} while(run);
	
	unsigned long sketchRuntime = getSysTimeUS() -sketchStartUS;
	unsigned long runtime = getSysTimeUS() -runStartUS;

	if(ret == 0) printTAG("SYSTEM", PRINT_COLOR_SYSTEM, "finished in %d ms (=> %.3f s).\n", (int)floor(runtime/1000.0), runtime/1000000.0);
	else if(ret < 0) printTAG("SYSTEM", PRINT_COLOR_ERROR, "terminated in %d ms (=> %.3f s).\n", (int)floor(runtime/1000.0), runtime/1000000.0);	
	else printTAG("SYSTEM", PRINT_COLOR_SYSTEM, "aborted by user after %d ms (=> %.3f s).\n", (int)floor(runtime/1000.0), runtime/1000000.0);

	printTAG("SYSTEM", PRINT_COLOR_SYSTEM, "(average fps:  %.3f).\n", (1000.0 /((sketchRuntime/1000.0) /frame)));

	// while(1) {
	// 	printf(".");
	// 	Sleep(1000);
	// }

    return 0;
}

void checkKeys() {
	// prevent repetetive pressing of keys
	// delay between keypresses in us ⬎
	if(getSysTimeUS() -keyBlock >= 333333) {
		keyBlock = getSysTimeUS();

		if(keyPressed(VK_ESCAPE)) {
			quit = 1;
			ret = 1;
		} else if(keyPressed(VK_RETURN)) {
			saveBMP(win->canvas, "C:\\Users\\eleftherakis\\Downloads\\sampafale1.bmp", 3);
			printInfo("saved image");
		} else if(keyPressed(VK_SPACE)) {
			if(pause) {
				pause = 0;
				printInfo("unpaused sketch");
			} else {
				pause = 1;
				printInfo("paused sketch");
			}
		} else { // no special key pressed => no block needed
			keyBlock = 0;
		}
	}
}

#pragma GCC diagnostic pop