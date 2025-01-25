#ifndef __IMAGE_H
#define __IMAGE_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"

#include <stdio.h>
#include <stdlib.h>
#include "color.h"
#include "util.h"
#include "print.h"


typedef enum {
    IMAGE_ERROR_INTERNAL = -1,
    IMAGE_ERROR_PARAMETER = -2,
    IMAGE_ERROR_NULLPTR = -3
} IMAGE_ERROR;

typedef enum {
    RGB8 = 3,
    RGBA8 = 4
} COLOR_SPACE;

typedef struct {    
    int width;
    int height;
    size_t size;
    int *pixel;    
} Image;


#pragma pack(push, 1)
typedef struct {
    unsigned short bfType;
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    long bfOffBits;
} BMP_FILEHEADER;

typedef struct {
    unsigned int biSize;
    int biWidth;
    int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
} BMP_INFOHEADER;
#pragma pack(pop)


/* check if Image pointer is valid */
int checkValid(Image* img, char *originFunction) {
    if(img == NULL) {
        printFunctionError("<*img> is a nullpointer", "%s()", originFunction);
        return -1;
    } else return 0;
}

int getPixelIndex(Image* img, int x, int y) {
    return x + ((img->height -1 -y) * img->width);
}

int setPixel(Image* img, int x, int y, int col) {    

    if(x >= img->width || y >= img->height || x < 0 || y < 0) { // position out of area
        //printFunctionWarning("point not in image => operation will be ignored", "setPixel(x: %d,y: %d) in <img>(w: %d, h: %d)", x, y, img->width, img->height);
        return IMAGE_ERROR_PARAMETER;
    } else {        
        img->pixel[x + ((img->height -1 -y) * img->width)] = col;
        // float a = alpha(col) /255.0;
        // img->data[index] = floor((img->data[index] * (1.0 -a)) + (blue(col) *a));
        // img->data[index +1] = floor((img->data[index +1] * (1.0 -a)) + (green(col) *a));
        // img->data[index +2] = floor((img->data[index +2] * (1.0 -a)) + (red(col) *a));                

        return 0;
    }
}

int getPixel(Image* img, int x, int y) {    

    if(x >= img->width || y >= img->height || x < 0 || y < 0) { // position out of area
        //printFunctionWarning("point not in image => operation will be ignored", "getPixel(x: %d,y: %d) in <img>(w: %d, h: %d)", x, y, img->width, img->height);
        return IMAGE_ERROR_PARAMETER;
    }        
    return img->pixel[x + ((img->height -1 -y) * img->width)];
}


Image* createImage(int w, int h) {    
    if(w <= 0 || h <= 0) {
        printFunctionError("invalid parameter(s)", "createImage(w: %d, h: %d)", w, h);
        return NULL;
    }

    size_t size = w * h;
    Image* img = (Image*)malloc(sizeof(Image));
    int *data = (int *)malloc(size * sizeof(int));    

    if (!data || !img) {
        printFunctionError("memory allocation failed", "createImage(w: %d, h: %d)", w, h);
        free(img);
        return NULL;
    }

    img->width = w;
    img->height = h;
    img->size = size;
    img->pixel = data; 

    return img; 
}

/* resize image to a specific width and height */
Image* resizeImage(Image* src, int newWidth, int newHeight) {

    if(checkValid(src, "resizeImage") < 0) return NULL;

    if ( newWidth <= 0 || newHeight <= 0) {
        printFunctionError("invalid parameter(s)", "resizeImage(w: %d, h: %d)", newWidth, newHeight);
        return NULL;
    }

    Image* dest = createImage(newWidth, newHeight);    
    
    float xScale = (float)src->width / newWidth;
    float yScale = (float)src->height / newHeight;

    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            int srcX = (int)(x * xScale);
            int srcY = (int)(y * yScale);
            
            int col = src->pixel[getPixelIndex(src, srcX, srcY)];
            dest->pixel[getPixelIndex(dest, x, y)] = col;
        }
    }

    return dest;
}

/* scale image with a scalar */
Image* scaleImage(Image* src, float scalar) {
    return resizeImage(src, (int)(src->width *1.0f *scalar), (int)(src->height *1.0f *scalar));
}

/* get selected area of Image*/
Image* getImage(Image* src, int x, int y, int w, int h) {

    if(checkValid(src, "getImage") < 0) return NULL;

    if(x >= src->width || y >= src->height || x + w >= src->width || y + h >= src->height || x < 0 || y < 0 || w < 0 || h < 0) {
        printFunctionError("exceeds <src> image boundaries", "getImage(x: %d, y: %d, w: %d, h: %d)", x, y, w, h);
        return NULL;
    }

    Image* dest = createImage(w, h);

    for(int y_ = 0; y_ < dest->height; y_++) {
        for(int x_ = 0; x_ < dest->width; x_++) {
            // int col = getPixel(src, x + x_, y +y_);
            // setPixel(dest, x_, y_, col);
            int col = src->pixel[getPixelIndex(src, x +x_, y +y_)];
            dest->pixel[getPixelIndex(dest, x_, y_)] = col;
        }
    }

    return dest;
}

/* paste image into another image at a position (x, y) */
int putImage(Image* src, Image* dest, int x, int y) {
    
    if(checkValid(src, "putImage") < 0 || checkValid(dest, "putImage") < 0) return IMAGE_ERROR_NULLPTR;

    if(x >= dest->width || y >= dest->height || x <= -src->width || y <= -src->height) {
        printFunctionError("<src> image not in <dest> image boundaries", "putImage(x: %d, y: %d) for <dest>(w: %d, h: %d)",
                            x, y, src->width, src->height);
        return IMAGE_ERROR_PARAMETER;
    }

    int startX = 0;
    int startY = 0;
    int endX = src->width;
    int endY = src->height;    

    if(x < 0) startX = -x;
    else if(x +src->width > dest->width) {
        endX = dest->width -x;
        if(endX > src->width) endX = src->width;
    }
    if(y < 0) startY = -y;
    else if(y +src->height > dest->height){
        endY = dest->height -y;
        if(endY > src->height) endY = src->height;
    }
    
    for(int y_ = startY; y_ < endY; y_++) {
        for(int x_ = startX; x_ < endX; x_++) {
            // int col = getPixel(src, x_, y_);
            // setPixel(dest, x_ +x, y_ +y, col);
            int col = src->pixel[getPixelIndex(src, x_, y_)];
            dest->pixel[getPixelIndex(dest, x_ +x, y_ +y)] = col;
        }
    }

    return 0;
}

int grayscaleImage(Image* img) {

    if(checkValid(img, "grayscaleImage") < 0) return IMAGE_ERROR_NULLPTR;

    for(unsigned int i = 0; i < img->size; i++) {
        int col = img->pixel[i];
        img->pixel[i] = GREY(brightness(col));
    }

    return 0;
}

int posterizeImage(Image* img, int steps) {
    
    if(checkValid(img, "posterizeImage") < 0) return IMAGE_ERROR_NULLPTR;

    for(unsigned int i = 0; i < img->size; i++) {
        int col = img->pixel[i];
        img->pixel[i] = posterize(col, steps);
    }

    return 0;
}


void printImageInfo(Image* img) {
    if(checkValid(img, "printImageInfo") >= 0) {
        printInfo("Image (w: %d, h: %d)\t=> %.02f MB pixel data @ 3 BPP\t(%d Bytes)",
                img->width, img->height, (img->size *3.0f /1000000.0), img->size *3);
    }
}

void printImage(Image* img, int w, int h) {

    if(checkValid(img, "printImage") >= 0) {
        float xScale = (float)img->width / w;
        float yScale = (float)img->height / h;

        for (int y = 0; y < h; y++) {
            printf("\n");
            for (int x = 0; x < w; x++) {
                int srcX = (int)(x * xScale);
                int srcY = (int)(y * yScale);

                int col = getPixel(img, srcX, srcY);

                printCol("  ", col);
            }
        }
        printf("\n");
    }
}


#pragma GCC diagnostic pop

#endif