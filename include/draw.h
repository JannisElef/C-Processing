#ifndef __DRAW_H
#define __DRAW_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wconversion"

#include "util.h"
#include "print.h"
#include "image.h"


typedef struct {
    int styleID;
    int translate_x, translate_y;
    int fillCol;
    int strokeCol;

    int strokeWeight;
    void *prev;
} Style;


static Style defaultStyle = {0,
                    0, 0,
                    (int)0xFFFFFFFF, (int)0xFFFF0000,
                    1, 
                    NULL};

static Style *currentStyle = &defaultStyle;

static int numStyles = 1;


void pushStyle() {
    // Style *newStyle = (Style *)malloc(sizeof(Style));
    // if (newStyle == NULL) {
    //     printFunctionError("couldn't allocate new memory", "pushStyle(no.: %d)", currentStyle->styleID +1);
    // }
    // *newStyle = (Style){currentStyle->styleID +1,
    //                     0, 0,
    //                     0xFFFFFFFF, 0xFFFF0000,
    //                     1, 
    //                     currentStyle};
    // currentStyle = newStyle;
    // numStyles++;
}

void popStyle() {
    // if (numStyles > 1) {
    //     Style *prev = currentStyle->prev;
    //     free(currentStyle);
    //     currentStyle = prev;
    //     numStyles--;
    // } else {
    //     printFunctionWarning("no further popStyle() possible", "popStyle()");
    // }
}

void translate(int x, int y) {
    currentStyle->translate_x = x;
    currentStyle->translate_y = y;
}

void strokeWeight(int px) {
    currentStyle->strokeWeight = px;
}

void fill(int col) {
    col = col;
}

void stroke(int col) {
    currentStyle->strokeCol = col;
}


void pixel(Image *img, int x, int y, int col) {
    img->pixel[getPixelIndex(img, x +currentStyle->translate_x, y +currentStyle->translate_y)] = col;
}

/* draw horizontal line */
void hline(Image *img, int x1, int x2, int y, int col) {
    int x1_ = x1;
    int x2_ = x2;
    if(x1_ > x2_) swap(&x1_, &x2_);
    for(int i = 0; i < x2_ -x1_; i++) {
        pixel(img, x1_ +i, y, col);

    }
}

/* draw vertical line */
void vline(Image *img, int x, int y1, int y2, int col) {
    int y1_ = y1;
    int y2_ = y2;
    if(y1_ > y2_) swap(&y1_, &y2_);
    for(int i = 0; i < y2_ -y1_; i++) {
        pixel(img, x, y1_ +i, col);
        
    }
}
/* draw universal line */
void line(Image *img, int x1, int y1, int x2, int y2, int col) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        pixel(img, x1, y1, col);
        
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}

/* draw gradient colored line */
void lineGradient(Image *img, int x1, int y1, int x2, int y2, int numSteps, int *gradientCols, size_t size) {
    pushStyle();
    float stepX = (x2 - x1) *1.0f / numSteps *1.0f;
    float stepY = (y2 - y1) *1.0f / numSteps *1.0f;
    
    for (int i = 0; i < numSteps; i++) {
        float startX = x1 + (i * stepX);
        float startY = y1 + (i * stepY);
        float endX = x1 + ((i + 1) * stepX);
        float endY = y1 + ((i + 1) * stepY);
        
        float t = i / (numSteps *1.0f);
        int col = lerpColors(gradientCols, size, t);
        
        printf("(x%fy%f) (x%fy%f)\n", startX, startY, endX, endY);
        
        line(img, startX, startY, endX, endY, col);
    }
    popStyle();
}

/* draw rectangle border */
void rectBorder(Image *img, int x, int y, int w, int h, int col) {
    hline(img, x, x +w, y, col);
    hline(img, x, x +w, y +h, col);

    vline(img, x, y, y +h, col);
    vline(img, x +w, y, y +h, col);
}
/* draw filled rectangle */
void rect(Image *img, int x, int y, int w, int h, int col) {
    for(int i = 0; i < h; i++) {
        hline(img, x, x +w, y +i, col);
    }
}

/* set background to color */
void background(Image *img, int col) {
    int tmpCol = col;
    col = col;
    rect(img, 0, 0, img->width, img->height, col);
    col = tmpCol;
}

/* draw cirlce border */
void circleBorder(Image* img, int x, int y, int r, int col) {
    int xi = 0;
    int yi = r;
    int d = 3 - 2 * r;

    while (yi >= xi) {        
        pixel(img, x + xi, y + yi, col);
        pixel(img, x - xi, y + yi, col);
        pixel(img, x + xi, y - yi, col);
        pixel(img, x - xi, y - yi, col);
        pixel(img, x + yi, y + xi, col);
        pixel(img, x - yi, y + xi, col);
        pixel(img, x + yi, y - xi, col);
        pixel(img, x - yi, y - xi, col);

        if (d < 0) {
            d = d + 4 * xi + 6;
        } else {
            d = d + 4 * (xi - yi) + 10;
            yi--;
        }
        xi++;
    }
}

/* draw filled cricle */
void circle(Image* img, int x, int y, int r, int col) {
    for (int yi = -r; yi <= r; yi++) {
        for (int xi = -r; xi <= r; xi++) {
            if (xi * xi + yi * yi <= r * r) {
                pixel(img, x + xi, y + yi, col);

            }
        }
    }
    circleBorder(img, x, y, r, col);
}

#pragma GCC diagnostic pop

#endif
