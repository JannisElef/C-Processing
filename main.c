

#include <time.h>

#include "playground.h"

#include "include/util.h"
#include "include/print.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wconversion"


int main() {	

	int run = 1;
	
	int firstCycle = 1;

	unsigned long runStartUS = getSysTimeUS();	

	unsigned long timeStart;
	unsigned long timeElapsed;	
	long timeToSleep;	
	struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 0;

	int ret = 0;

	printTAG("SYSTEM", PRINT_COLOR_SYSTEM, "starting...\n");

	// god's loop
	do {
		run = 0;

		if(firstCycle) {
			// printInfo("starting setup()");	
			ret = setup();
			if(ret != 0) {				
				printFunctionError("an error occured", "setup() returned %d.", ret);
			} else {
				// printInfo("finished setup() successfully");	
			}
		}
		if(ret == 0) {
			timeStart = getSysTimeUS();

			ret = draw();
			if(ret != 0) {
				quit = 1;
				printFunctionError("an error occured", "draw() returned %d.", ret);
			}

			timeElapsed = getSysTimeUS() - timeStart;		

			if(loop) {
				run = 1;
				if(quit) run = 0;
			}

			timeToSleep = (internal_frameTime -timeElapsed);
			if(timeToSleep > 0) {
				ts.tv_nsec = timeToSleep;
				nanosleep(&ts, NULL);
			}
		}

	} while(run);
	
	unsigned long t = getSysTimeUS() -runStartUS;
	if(ret == 0) printTAG("SYSTEM", PRINT_COLOR_SYSTEM, "finished successfully in %d ms (=> %.3f s).\n", (int)floor(t/1000.0), t/1000000.0);
	else printTAG("SYSTEM", PRINT_COLOR_ERROR, "terminated in %d ms (=> %.3f s).\n", (int)floor(t/1000.0), t/1000000.0);	

    return 0;
}

#pragma GCC diagnostic pop