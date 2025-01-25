#ifndef __UTIL_H
#define __UTIL_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"

#include <stdio.h>
#include <stdlib.h>


#define QUARTER_PI  0.7853982
#define HALF_PI     1.5707964
#define PI          3.1415927
#define TWO_PI      6.2831855


/* check if point-in-polygon => 0 if yes, -1 if not */
int isPIP(int x, int y, int areaX, int areaY, int areaW, int areaH) {    
    if(x > areaW || x < areaX || y > areaH || y < areaY) {
        return 0; // is inside
    } else return -1;
}
/* check if point-in-polygon (float) => 0 if yes, -1 if not */
int isPIPf(float x, float y, float areaX, float areaY, float areaW, float areaH) {
    if(x > areaW || x < areaX || y > areaH || y < areaY) {
        return 0; // is inside
    } else return -1;
}

float map(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/* time ⬎*/

#include <sys/time.h>

unsigned long startTimeUS;


unsigned long getSysTimeUS() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

void startTime() {
    startTimeUS = getSysTimeUS();
    srand(time(NULL));
}

unsigned long stopTime() {
    return getSysTimeUS() -startTimeUS;
}

void printSysTimeUS() {
   printf("current system time: %lu us\n", getSysTimeUS());
}

/* 0...1 */
float random() {
    return (float)rand() / RAND_MAX;
}

int randomInt(int max) {
    return (int)floor(random() *max);
}

float randomDouble(float max) {
    return (float)random() *max;
}

/* time ⬏*/

/* vector ⬎*/
typedef struct {
    float x1, x2;
} vec2;

typedef struct {
    float x1, x2, x3;
} vec3;

typedef struct {
    float x1, x2, x3, x4;    
} vec4;


void printVec2(vec2* v) {
	printf("(%.02f|%.02f)\n", v->x1, v->x2);
}

void printVec3(vec3* v) {
	printf("(%.02f|%.02f|%.02f)\n", v->x1, v->x2, v->x3);
}

void printVec4(vec4* v) {
	printf("(%.02f|%.02f|%.02f|%.02f)\n", v->x1, v->x2, v->x3, v->x4);
}


/* Addition of two vec2 vectors */
vec2 addVec2(const vec2* v1, const vec2* v2) {
    vec2 result = {v1->x1 + v2->x1, v1->x2 + v2->x2};
    return result;
}

/* Subtraction of two vec2 vectors */
vec2 subVec2(const vec2* v1, const vec2* v2) {
    vec2 result = {v1->x1 - v2->x1, v1->x2 - v2->x2};
    return result;
}

/* Scalar multiplication of a vec2 vector */
vec2 scaleVec2(const vec2* v, float s) {
    vec2 result = {v->x1 * s, v->x2 * s};
    return result;
}

/* Normalization of a vec2 vector */
vec2 normalizeVec2(const vec2* v) {
    float len = sqrt(v->x1 * v->x1 + v->x2 * v->x2);
    if (len == 0) return *v;
    return scaleVec2(v, 1.0f / len);
}

/* Length of a vec2 vector */
float lengthVec2(const vec2* v) {
    return sqrt(v->x1 * v->x1 + v->x2 * v->x2);
}


/* Addition of two vec3 vectors */
vec3 addVec3(const vec3* v1, const vec3* v2) {
    vec3 result = {v1->x1 + v2->x1, v1->x2 + v2->x2, v1->x3 + v2->x3};
    return result;
}

/* Subtraction of two vec3 vectors */
vec3 subVec3(const vec3* v1, const vec3* v2) {
    vec3 result = {v1->x1 - v2->x1, v1->x2 - v2->x2, v1->x3 - v2->x3};
    return result;
}

/* Scalar multiplication of a vec3 vector */
vec3 scaleVec3(const vec3* v, float s) {
    vec3 result = {v->x1 * s, v->x2 * s, v->x3 * s};
    return result;
}

/* Normalization of a vec3 vector */
vec3 normalizeVec3(const vec3* v) {
    float len = sqrt(v->x1 * v->x1 + v->x2 * v->x2 + v->x3 * v->x3);
    if (len == 0) return *v;
    return scaleVec3(v, 1.0f / len);
}

/* Length of a vec3 vector */
float lengthVec3(const vec3* v) {
    return sqrt(v->x1 * v->x1 + v->x2 * v->x2 + v->x3 * v->x3);
}


/* Addition of two vec4 vectors */
vec4 addVec4(const vec4* v1, const vec4* v2) {
    vec4 result = {v1->x1 + v2->x1, v1->x2 + v2->x2, v1->x3 + v2->x3, v1->x4 + v2->x4};
    return result;
}

/* Subtraction of two vec4 vectors */
vec4 subVec4(const vec4* v1, const vec4* v2) {
    vec4 result = {v1->x1 - v2->x1, v1->x2 - v2->x2, v1->x3 - v2->x3, v1->x4 - v2->x4};
    return result;
}

/* Scalar multiplication of a vec4 vector */
vec4 scaleVec4(const vec4* v, float s) {
    vec4 result = {v->x1 * s, v->x2 * s, v->x3 * s, v->x4 * s};
    return result;
}

/* Normalization of a vec4 vector */
vec4 normalizeVec4(const vec4* v) {
    float len = sqrt(v->x1 * v->x1 + v->x2 * v->x2 + v->x3 * v->x3 + v->x4 * v->x4);
    if (len == 0) return *v;
    return scaleVec4(v, 1.0f / len);
}

/* Length of a vec4 vector */
float lengthVec4(const vec4* v) {
    return sqrt(v->x1 * v->x1 + v->x2 * v->x2 + v->x3 * v->x3 + v->x4 * v->x4);
}

/* Distance between two vec2 vectors */
float distVec2(const vec2* v1, const vec2* v2) {
    return sqrt(pow(v1->x1 - v2->x1, 2) + pow(v1->x2 - v2->x2, 2));
}

/* Distance between two vec3 vectors */
float distVec3(const vec3* v1, const vec3* v2) {
    return sqrt(pow(v1->x1 - v2->x1, 2) + pow(v1->x2 - v2->x2, 2) + pow(v1->x3 - v2->x3, 2));
}

/* Distance between two vec4 vectors */
float distVec4(const vec4* v1, const vec4* v2) {
    return sqrt(pow(v1->x1 - v2->x1, 2) + pow(v1->x2 - v2->x2, 2) + pow(v1->x3 - v2->x3, 2) + pow(v1->x4 - v2->x4, 2));
}

/* General distance function for 2D points */
float dist(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

/* vector ⬏*/

#pragma GCC diagnostic pop

#endif