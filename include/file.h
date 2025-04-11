#ifndef __FILE_H
#define __FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


// #define DEBUG_PRINT_V
// #define DEBUG_PRINT_VV


/* FILEFORMAT

    tagName0: 1         // int
    tagName1: 0.25      // float
    tagName2: "hallo"   // string

*/


int writeInt(FILE *file, char *tag, int i) {
    if(file == NULL) return 1; // file handle error
    else {
        fprintf(file, "%s: %d\n", tag, i);
    }
    return 0;
}

int writeFloat(FILE *file, char *tag, float f) {
    if(file == NULL) return 1; // file handle error
    else {
        fprintf(file, "%s: %f\n", tag, f);
    }
    return 0;
}

int writeString(FILE *file, char *tag, char *string) {
    if(file == NULL) return 1; // file handle error
    else {
        fprintf(file, "%s: %s\n", tag, string);
    }
    return 0;
}



int strlen(char *str) {
    int cnt = 0;
    while(str[cnt] != '\0') {
        cnt++;
    }
    return cnt;
}

int getIndexOf(char c, char *str) {
    int cnt = 0;
    while(1) {
        if(str[cnt] == c) return cnt;
        else if(str[cnt] == 0) return -1;
        else cnt++;
    }
}



int getIndexOfTagValue(FILE *file, char *tag) {
    char ch;

    char target = tag[0];

    int index = -1;
    int cnt = 0;

    while ((ch = fgetc(file)) != EOF && index == -1) {
        for(int i = 0; i < strlen(tag); i++) {
            cnt++;
            if(ch != tag[i]) {
                index = -1;
                break;
            }
            if(index == -1) index = cnt;
            #ifdef DEBUG_PRINT_VV
                printf("match: %c [%d]\n", ch, cnt);
            #endif
            ch = fgetc(file);
        }
    }

    if(index >= 0) index += strlen(tag) +1; // move index to value start position
    #ifdef DEBUG_PRINT_V
        printf("found value of \"%s\" at index %d\n", tag, index);
    #endif
    return index;
}

char *getValueOfTag(FILE *file, char *tag) {
    fseek(file, 0, SEEK_SET); // move filepointer to start
    int index = getIndexOfTagValue(file, tag);

    if(index < 0) return 0; // tag not found

    fseek(file, index, SEEK_SET); // move filepointer to index

    int length = 0;
    char ch;
    while ((ch = fgetc(file)) != '\n') { // count chars of tag-value
        length++;
    }

    fseek(file, index, SEEK_SET);

    char *buf = (char *)malloc(length +1);

    for (int i = 0; i < length; i++) {
        buf[i] = fgetc(file);
    }

    buf[length] = '\0';

    #ifdef DEBUG_PRINT_V
        printf("value \"%s\": %s\n", tag, buf);
    #endif

    return buf;
}

int readInt(FILE *file, char *tag, int *i) {
    int value = 0;
    if(file == NULL) return 1; // file handle error
    else {
        char *buf = getValueOfTag(file, tag);
        if(buf == 0) return 1; // tag not found

        int decimal = 0;
        int exponent = strlen(buf);
        int sign = 1;

        for(int i = 0; i < strlen(buf); i++) {
            exponent--;
            char c = buf[i];
            decimal = c - '0';
            if(decimal > 0 && decimal < 10) value += decimal *pow(10, exponent);
            else if(c == '-') sign = -1;
        }
        *i = sign *value;
    }
    return 0;
}

int readFloat(FILE *file, char *tag, float *f) {
    float value = 0;
    if(file == NULL) return 1; // file handle error
    else {
        char *buf = getValueOfTag(file, tag);
        if(buf == 0) return 1; // tag not found

        int decimal = 0;
        int pointIndex = getIndexOf('.', buf); // get decimal point index
        int exponent = pointIndex;
        int sign = 1;

        for(int i = 0; i < strlen(buf); i++) {
            if(i == pointIndex) continue; // skip decimal point
            else exponent--;
            char c = buf[i];
            decimal = c - '0';
            if(decimal > 0 && decimal < 10) value += (float)(decimal *pow(10, exponent));
            else if(c == '-') sign = -1;
        }
        *f = sign *value;
    }
    return 0;
}

int readString(FILE *file, char *tag, char **str) {
    if(file == NULL) return 1; // file handle error
    else {
        char *buf = getValueOfTag(file, tag);
        *str = buf;
    }
    return 0;
}


#endif
