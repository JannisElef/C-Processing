#ifndef __PRINT_H
#define __PRINT_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "color.h"


#define PRINT_COLOR_SYSTEM  0x00FF00
#define PRINT_COLOR_INFO    0x00FFFF
#define PRINT_COLOR_WARNING 0xFFFF00
#define PRINT_COLOR_ERROR   0xFF0000


const char *prefix = "\x1b["; // hex Escape Code
char infix[64]     = "";
const char *suffix = "m";

int textCol = 0;       // ANSI code
int backgroundCol = 0; // ANSI code


const char *reset = "\x1b[0m";

//                    ⬐black   ⬐red    ⬐green   ⬐yellow ⬐blue   ⬐magenta ⬐cyan    ⬐white     
const int palette[8] = {0x000000, 0xFF0000, 0x00FF00, 0xFFFF00, 0x0000FF, 0xFF00FF, 0x00FFFF, 0xFFFFFF};

int styleFlag = 0;
/* 0b0000|0000
     abcd efxx

a → bold
b → dim
c → italic
d → underline
e → hidden
f → strikethrough
x → don't care

*/

void setStyle(int flag) { 
    styleFlag = flag;    
}

void resetStyle() { 
    setStyle(0);
}


int getClosestANSIcolor(int col) {
    int closestDistance = 1000;
    int closestIndex = 0;
    for(int i = 0; i < 8; i++) {
        int dist = getColDistance(palette[i], col);
        if(dist < closestDistance) {
            closestDistance = dist;
            closestIndex = i;
        }
    }

    return 30 + closestIndex;
}

void setTextColor(int col, char bright) {
    textCol = getClosestANSIcolor(col);
    if(bright) textCol += 60;
    if(col == 0) textCol = -1; // reset
}

void setBackgroundColor(int col, char bright) {
    backgroundCol = getClosestANSIcolor(col) + 10;
    if(bright) backgroundCol += 60;
    if(col == 0) backgroundCol = -1; // reset
}


void updateStyle() {
    memset(infix, 0, sizeof infix); // clear infix buffer
    char *ANSI_codes[6] = {"1;", "2;", "3;", "4;", "8;", "9;"}; // bold, dim, italic, underline, hidden, strikethrough
    for(int i = 0; i < 6; i++) {
        if((styleFlag & (1 << (7 -i))) > 0) strcat(infix, ANSI_codes[i]); // apply specific ANSI code (bold/italic/...)        
    }    
    char textANSI[4];    
    char backgroundANSI[4];
    if(textCol > 0) { // foreground
        sprintf(textANSI, "%d", textCol);
        strcat(infix, textANSI);
        if(backgroundCol > 0) {
            strcat(infix, ";"); // add ';' for following ANSI code
        }
    }
    if(backgroundCol > 0) {
        sprintf(backgroundANSI, "%d", backgroundCol);
        strcat(infix, backgroundANSI);
    }
}


void print(char *str, int textCol_, int backgroundCol_) {
    setTextColor(textCol_, 0);
    setBackgroundColor(backgroundCol_, 0);
    updateStyle();
    printf("%s%s%s%s%s%s", reset, prefix, infix, suffix, str, reset);
}


/* print detailed info-message : "functionSignature(parameters...): msg" */
void printFunctionInfo(char *msg, const char *functionSignature, ...) {
    setStyle(128); // bold
    updateStyle();
    print(" I ", 0, PRINT_COLOR_INFO);
    setTextColor(PRINT_COLOR_INFO, 0);
    setBackgroundColor(0, 0);
    updateStyle();
    printf("%s %s%s%s", reset, prefix, infix, suffix);
    va_list args;
    va_start(args, functionSignature);
    vprintf(functionSignature, args);
    va_end(args);   
    printf("\n\t=> %s%s\n", msg, reset);
    resetStyle();
    updateStyle();
}

/* print detailed warning-message : "functionSignature(parameters...): msg" */
void printFunctionWarning(char *msg, const char *functionSignature, ...) {
    setStyle(128); // bold
    updateStyle();
    print(" W ", 0, PRINT_COLOR_WARNING);
    setTextColor(PRINT_COLOR_WARNING, 0);
    setBackgroundColor(0, 0);
    updateStyle();
    printf("%s %s%s%s", reset, prefix, infix, suffix);
    va_list args;
    va_start(args, functionSignature);
    vprintf(functionSignature, args);
    va_end(args);   
    printf("\n\t=> %s.%s\n", msg, reset);
    resetStyle();
    updateStyle();
}

/* print detailed error-message : "functionSignature(parameters...): msg" */
void printFunctionError(char *msg, const char *functionSignature, ...) {
    setStyle(128); // bold
    updateStyle();
    print(" E ", 0, PRINT_COLOR_ERROR);
    setTextColor(PRINT_COLOR_ERROR, 0);
    setBackgroundColor(0, 0);
    updateStyle();
    printf("%s %s%s%s", reset, prefix, infix, suffix);
    va_list args;
    va_start(args, functionSignature);
    vprintf(functionSignature, args);
    va_end(args);    
    printf("\n\t=> %s!%s\n", msg, reset);
    resetStyle();
    updateStyle();
}


/* print detailed message : " TAG  msg" */
void printTAGCustom(char *TAG, int tagFgCol, int tagBgCol, int msgFgCol, int msgBgCol, const char *msg, ...) {
    setStyle(128); // bold
    updateStyle();
    char tagBuf[16];
    sprintf(tagBuf, " %s ", TAG);
    print(tagBuf, tagFgCol, tagBgCol);
    setTextColor(msgFgCol, 0);
    setBackgroundColor(msgBgCol, 0);
    resetStyle();
    updateStyle();
    printf("%s %s%s%s", reset, prefix, infix, suffix);
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);   
    printf("%s\n", reset);
}

/* print detailed message : " TAG  msg" */
void printTAG(char *TAG, int col, const char *msg, ...) {
    setStyle(128); // bold
    updateStyle();
    char tagBuf[16];
    sprintf(tagBuf, " %s ", TAG);
    print(tagBuf, 1, col);
    setTextColor(col, 0);
    setBackgroundColor(0, 0);
    resetStyle();
    updateStyle();
    printf("%s %s%s%s", reset, prefix, infix, suffix);
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);   
    printf("%s\n", reset);
}

void printSystem(const char *format, ...) {
    setStyle(128); // bold
    updateStyle();
    print(" S ", 0, PRINT_COLOR_SYSTEM);
    resetStyle();
    setTextColor(PRINT_COLOR_SYSTEM, 0);
    setBackgroundColor(0, 0);
    updateStyle();
    printf(" %s%s%s%s", reset, prefix, infix, suffix);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("%s\n", reset);
}

void printInfo(const char *format, ...) {
    setStyle(128); // bold
    updateStyle();
    print(" I ", 0, PRINT_COLOR_INFO);
    resetStyle();
    setTextColor(PRINT_COLOR_INFO, 0);
    setBackgroundColor(0, 0);
    updateStyle();
    printf(" %s%s%s%s", reset, prefix, infix, suffix);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("%s\n", reset);
}

void printWarning(const char *format, ...) {
    setStyle(128); // bold
    updateStyle();
    print(" W ", 0, PRINT_COLOR_WARNING);
    setTextColor(PRINT_COLOR_WARNING, 0);
    setBackgroundColor(0, 0);
    updateStyle();
    printf(" %s%s%s%s", reset, prefix, infix, suffix);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("!%s\n", reset);
}

void printError(const char *format, ...) {
    setStyle(128); // bold
    updateStyle();
    print(" E ", 0, PRINT_COLOR_ERROR);
    resetStyle();
    setTextColor(PRINT_COLOR_ERROR, 0);
    setBackgroundColor(0, 0);
    updateStyle();
    printf(" %s%s%s%s", reset, prefix, infix, suffix);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("!%s\n", reset);
}


void printBright(char *str, int textCol_, int backgroundCol_) {
    setTextColor(textCol_, 1);
    setBackgroundColor(backgroundCol_, 1);
    updateStyle();
    printf("%s%s%s%s%s%s", reset, prefix, infix, suffix, str, reset);
}

void printTrueColor(char *str, int textCol_, int backgroundCol_) {
    textCol = -1;
    backgroundCol = -1; // internal ANSI codes for color won't be used
    updateStyle();
    printf("%s%s%s%s\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm%s%s", reset, prefix, infix, suffix, red(textCol_), green(textCol_), blue(textCol_), red(backgroundCol_), green(backgroundCol_), blue(backgroundCol_), str, reset);    
}

void printColInfo(int col) {
    printf("%s(\x1b[48;2;%d;%d;%dm%3d,%3d,%3d%s)\n", reset, red(col), green(col), blue(col), red(col), green(col), blue(col), reset);
}

void printCol(char *str, int col) {
    printf("%s\x1b[48;2;%d;%d;%dm%s%s", reset, red(col), green(col), blue(col), str, reset);
}


void printTest() {
    char *str = "@ HI @";
    for(int j = 0; j < 8; j++) {
        int flag = 0;
		for(int i = 0; i < 7; i++) {
            print("|", RGB(255, 255, 255), 0);	
		    setStyle(flag);
			print(str, (0xff << 24) + palette[j], 0);
            flag = (1 << (7 -i));
            resetStyle();
		}
        print(" ", 0, 0);	
        flag = 0;
		for(int i = 0; i < 7; i++) {
            print("|", RGB(255, 255, 255), 0);	
            setStyle(flag);
			print(str, RGB(255, 255, 255), (0xff << 24) + palette[j]);	
            flag = (1 << (7 -i));
            resetStyle();
		}
        print(" ", 0, 0);	
        flag = 0;
		for(int i = 0; i < 7; i++) {
            setStyle(flag);
			printBright(str, RGB(255, 255, 255), (0xff << 24) + palette[j]);	
            resetStyle();
            print("|", RGB(255, 255, 255), 0);	
            flag = (1 << (7 -i));
		}
		printf("\n");
	}
}

#endif