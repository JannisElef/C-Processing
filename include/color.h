#ifndef __COLOR_H
#define __COLOR_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wconversion"

#include "util.h"


static int color_gradient_rainbow[]     = {0x0000FF, 0x00FFFF, 0x00FF00, 0xFFFF00, 0xFF0000, 0xFF00FF}; // 🟦🟩🟨🟥
static int color_gradient_plasma[]      = {0x000170, 0x0001C2, 0x8700D1, 0xD10187, 0xFF0050, 0xFF7901, 0xFFBF00, 0xFFEE00, 0xFFFFB0}; // lila, rot, gelb
static int color_gradient_virdis[]      = {0x440154, 0x472F7D, 0x39568C, 0x2A788E, 0x1F988B, 0x35B779, 0x7AD151, 0xD2E21B, 0xFDE725}; // lila, türkis, grün, gelb
static int color_gradient_magma[]       = {0x000004, 0x20114B, 0x57157E, 0x8C2981, 0xC43C75, 0xF1605D, 0xFE9F6D, 0xFDDEA0, 0xFCFDBF}; // schwarz, lila, orange, helles orange
static int color_gradient_rose[]        = {0x001066, 0x551672, 0x892282, 0xB93495, 0xE64BAC, 0xF380BC, 0xFCAFCB, 0xFFDDD8, 0xFFF4DE}; // lila, magenta, pink, helles pink
static int color_gradient_cerulean[]    = {0x180055, 0x1B296D, 0x1D4C81, 0x246E96, 0x3091AA, 0x58B4B9, 0x85D5CA, 0xC9F1E4, 0xEAFFF1}; // dunkelblau, petrol, cyan, helles türkis
static int color_gradient_forest[]      = {0x00313A, 0x0E4D41, 0x1E6A48, 0x31884F, 0x4CA658, 0x79C365, 0xA6DF73, 0xE2F5BD, 0xFFFFE0}; // dunkelgrün, grün, hellgrün, fast weiß


struct color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};


unsigned char alpha(int col) {
    return ((col >> 24) & 0xff);
}

unsigned char red(int col) {
    return ((col >> 16) & 0xff);
}

unsigned char green(int col) {
    return ((col >> 8) & 0xff);
}

unsigned char blue(int col) {
    return (col & 0xff);
}

unsigned char brightness(int col) {
    return ((red(col) + green(col) + blue(col)) /3);
}


int RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    return (a << 24) + (r << 16) + (g << 8) + b;
}

int _RGB(unsigned char r, unsigned char g, unsigned char b) {
    return RGBA(r, g, b, 255);
}

int GREY(unsigned char a) {
    return RGB(a, a, a);
}


int getColDistance(int col1, int col2) {
    int dr = abs(red(col1) -red(col2));    
    int dg = abs(green(col1) -green(col2));    
    int db = abs(blue(col1) -blue(col2));    

    return dr + dg + db;
}


int colorToCol(struct color c) {
    return (c.a << 24) + (c.r << 16) + (c.g << 8) + c.b;
}

int getColorDistance(struct color col1_, struct color col2_) {
    int col1 = colorToCol(col1_);
    int col2 = colorToCol(col2_);

    return getColDistance(col1, col2);
}

int posterize(int col, int steps) {
    int val = brightness(col);
    
    float stepSize = 255.0f / (steps - 1);
    int bright = (int)((val / stepSize) + 0.5f) * stepSize;
    
    if (bright > 255) bright = 255;
    if (bright < 0) bright = 0;

    return RGB(bright, bright, bright);
}

int getRandomCol(int* palette, size_t size) {    
    int col = palette[randomInt(size)];    
    return col;
}

int lerpColor(int col1, int col2, float t) {
    int r1 = (col1 >> 16) & 0xFF;
    int g1 = (col1 >> 8) & 0xFF;
    int b1 = col1 & 0xFF;

    int r2 = (col2 >> 16) & 0xFF;
    int g2 = (col2 >> 8) & 0xFF;
    int b2 = col2 & 0xFF;

    int r = (int)(r1 + t * (r2 - r1));
    int g = (int)(g1 + t * (g2 - g1));
    int b = (int)(b1 + t * (b2 - b1));

    return (255 << 24) | (r << 16) | (g << 8) | b;
}

int lerpColors(int* gradientCols, size_t size, float t) {
    int n = size - 1;
    float scaledX = t * n;
    int index1 = (int)scaledX;
    int index2 = (index1 + 1 < size) ? index1 + 1 : index1;
    float blend = scaledX - index1;

    int c1 = gradientCols[index1];
    int c2 = gradientCols[index2];

    return lerpColor(c1, c2, blend);
}

#pragma GCC diagnostic pop

#endif