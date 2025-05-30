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

void freeImage(Image **img) {
    if (img && *img) {
        free((*img)->pixel);
        free(*img);
        *img = NULL;
    }
}

Image *copyImage(Image *src) {

    if(checkValid(src, "copyImage") < 0) return NULL;

    Image *copy = createImage(src->width, src->height);

    for(size_t i = 0; i < src->size; i++) {
        copy->pixel[i] = src->pixel[i];
    }

    return copy;
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

/* rotates image 90 degrees counterclockwise*/
Image *rotate90(Image *img) {
    if(checkValid(img, "rotate90") < 0) return NULL;

    Image *rotated = createImage(img->height, img->width);

    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            rotated->pixel[j * rotated->width + (rotated->width - 1 - i)] = img->pixel[i * img->width + j];
        }
    }

    return rotated;
}

/* blends top and bottom image with opacity x */
Image *blendImages(Image *top, Image *bottom, float x) {
    if(checkValid(top, "blendImage") < 0) return NULL;
    if(checkValid(bottom, "blendImage") < 0) return NULL;

    if(top->width != bottom->width || top->height != bottom->height) {
        printFunctionError("<top> image and <bottom> images differ in size", "blendImage()");
        return NULL;
    }
    
    Image *blended = createImage(top->height, top->width);

    int c1, c2;
    for(int i = 0; i < top->size; i++) {
        c1 = top->pixel[i];
        c2 = bottom->pixel[i];
        blended->pixel[i] = rgb((unsigned char)((red(c1)   *1.0f * x) + (red(c2)   *1.0f * (1 - x))),
                                (unsigned char)((green(c1) *1.0f * x) + (green(c2) *1.0f * (1 - x))),
                                (unsigned char)((blue(c1)  *1.0f * x) + (blue(c2)  *1.0f * (1 - x))));
    }
    return blended;
}
        

int grayscaleImage(Image* img) {

    if(checkValid(img, "grayscaleImage") < 0) return IMAGE_ERROR_NULLPTR;

    for(size_t i = 0; i < img->size; i++) {
        int col = img->pixel[i];
        img->pixel[i] = gray(brightness(col));
    }

    return 0;
}

int thresholdImage(Image *img, int thresholdValue) {
    if(checkValid(img, "thresholdImage") < 0) return IMAGE_ERROR_NULLPTR;

    for(unsigned int i = 0; i < img->size; i++) {
        int val = brightness(img->pixel[i]);
        if(val < thresholdValue) img->pixel[i] = 0xFF000000; // black
        else img->pixel[i] = 0xFFFFFFFF; // white
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

int invertImage(Image *img) {
    if(checkValid(img, "invertImage") < 0) return IMAGE_ERROR_NULLPTR;

    for(unsigned int i = 0; i < img->size; i++) {
        img->pixel[i] = invertCol(img->pixel[i]);
    }

    return 0;
}


int getAvgImageCol(Image *img) {
    int r, g, b;

    int col = 0;
    for(int i = 0; i < img->size; i++) {
        col = img->pixel[i];
        r += red(col);
        g += green(col);
        b += blue(col);
    }

    return rgb((int)(r *1.0f /img->size), (int)(g *1.0f /img->size), (int)(b *1.0f /img->size));
}
        

float kernel[3][3] = {{ -1, -1, -1},
                      { -1,  8, -1},
                      { -1, -1, -1}};

Image *edgeDetection(Image *img, int bgCol) { // https://processing.org/examples/edgedetection.html
    if(checkValid(img, "edgeDetection") < 0) return NULL;
    
    Image *grayImg = copyImage(img);
    grayscaleImage(grayImg);


    Image *edgeImg = createImage(img->width, img->height);

    for (int y = 1; y < grayImg->height-1; y++) {   // Skip top and bottom edges
        for (int x = 1; x < grayImg->width-1; x++) {  // Skip left and right edges
          // Output of this filter is shown as offset from 50% gray.
          // This preserves transitions from low (dark) to high (light) value.
          // Starting from zero will show only high edges on black instead.
          float sum = bgCol;
          for (int ky = -1; ky <= 1; ky++) {
            for (int kx = -1; kx <= 1; kx++) {
              // Calculate the adjacent pixel for this kernel point
              int pos = (y + ky)*grayImg->width + (x + kx);
    
              // Image is grayscale, red/green/blue are identical
              int val = blue(grayImg->pixel[pos]);
              // Multiply adjacent pixels based on the kernel values
              sum += kernel[ky+1][kx+1] * val;
            }
          }
          // For this pixel in the new image, set the output value
          // based on the sum from the kernel
          edgeImg->pixel[y*edgeImg->width + x] = gray((int)sum);
        }
    }
    return edgeImg;
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
