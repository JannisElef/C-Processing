

#include <time.h>
#include <windows.h>

#include "include/sketch.h"


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wconversion"



//compile: " gcc -o main main.c -lgdi32 "



int main(int argc, char* argv[]) {

	if(argc > 1) {
		size_t size = 0;
		for(int i = 0; i < argc; i++) {
			size += sizeof(argv[i]);
		}
		char buffer[size];
		for(int i = 0; i < argc; i++) {
			sprintf(buffer, "%s[%d]%s ", buffer, i, argv[i]);
		}
		printf("\n");
		printTAG("ARGS", 0xFFFF00FF, "%s\n", buffer);
		free(buffer);
	}	

	unsigned long startUS = getSysTimeUS();

	printTAG("SYSTEM", PRINT_COLOR_SYSTEM, "starting...\n");

	root = createProcess("MAIN");

	int ret = start(root);
	
	unsigned long runtime = getSysTimeUS() -startUS;	

	if(ret < 0) printTAG("SYSTEM", PRINT_COLOR_ERROR, "terminated in %d ms (=> %.3f s).\n", (int)floor(runtime/1000.0), runtime/1000000.0);	
	else printTAG("SYSTEM", PRINT_COLOR_SYSTEM, "finished after %d ms (=> %.3f s).\n", (int)floor(runtime/1000.0), runtime/1000000.0);	

	// while(1) {
	// 	printf(".");
	// 	Sleep(1000);
	// }

    return ret;
}

#pragma GCC diagnostic pop