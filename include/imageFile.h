#ifndef __IMAGEFILE_H
#define __IMAGEFILE_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"

#include <stdio.h>
#include <stdlib.h>
#include "color.h"
#include "util.h"
#include "print.h"
#include "image.h"


Image * loadBMP(char *filePath) {
    FILE *file = fopen(filePath, "rb");    

    if (!file) {
        printFunctionError("couldn't open file", "loadBMP(filePath: \"%s\")", filePath);
        return NULL;
    } else {

        BMP_FILEHEADER fileHeader;
        fread(&fileHeader, sizeof(BMP_FILEHEADER), 1, file);

        BMP_INFOHEADER infoHeader;
        fread(&infoHeader, sizeof(BMP_INFOHEADER), 1, file);

        int w = infoHeader.biWidth;
        int h = infoHeader.biHeight;
        size_t size = w * h;
        int bytesPerPixel = (infoHeader.biBitCount /8);

        Image * img = (Image*)malloc(sizeof(Image));

        unsigned char *data = (unsigned char *)malloc(size *bytesPerPixel);
        int *pixel = (int *)malloc(size * sizeof(int));

        if (!data || !img || !pixel) {
            printFunctionError("memory allocation failed", "loadBMP()");
            free(img);
            return NULL;
        }
        fseek(file, fileHeader.bfOffBits, SEEK_SET);
        fread(data, size *bytesPerPixel, 1, file);

        fclose(file);

        img->width = w;
        img->height = h;
        img->size = size; 
        img->pixel = pixel;       

        for(size_t i = 0; i < size; i++) {
            if(bytesPerPixel == 3) {
                img->pixel[i] = _RGB(data[(i *bytesPerPixel) +2],
                                     data[(i *bytesPerPixel) +1],
                                     data[(i *bytesPerPixel) +0]);
            } else {
                img->pixel[i] = RGBA(data[(i *bytesPerPixel) +3],
                                     data[(i *bytesPerPixel) +2],
                                     data[(i *bytesPerPixel) +1],
                                     data[(i *bytesPerPixel) +0]);
            }
        }

        free(data);
        data = NULL;

        return img;
    }    
}

int saveBMP(Image *img, const char *filePath, int colorspace) {

    if(checkValid(img, "saveBMP") < 0) return IMAGE_ERROR_NULLPTR;

    if(colorspace < 3 || colorspace > 4) {
        printFunctionError("invalid colorspace", "saveBMP(..., colorspace: %d)", colorspace);
        return IMAGE_ERROR_PARAMETER;
    }

    FILE *file = fopen(filePath, "wb");
    if (!file) {
        printFunctionError("couldn't create file", "saveBMP(filePath: \"%s\", ...)", filePath);
        return IMAGE_ERROR_INTERNAL;        
    } else { 

        int remainder_x = img->width %4;   
        int remainder_y = img->height %4;           
        if(remainder_x != 0 || remainder_y != 0) {
            Image * out = createImage(img->width +remainder_x, img->height +remainder_y);

            printFunctionWarning("dimensions must be a multiple of 4 for BMP", "saveBMP(...) for <img>(w: %d, h: %d) => (w: %d, h: %d)",
                                img->width, img->height, out->width, out->height);


            // putImage(img, out, 0, 0);

            for(int y = 0; y < img->height; y++) {
                for(int x = 0; x < img->width; x++) {
                    int col = getPixel(img, x, y);
                    setPixel(out, x, y, col);
                }
            }     
            img->width = out->width;       
            img->height = out->height;       
            img->size = out->size;       
            img->pixel = out->pixel;       
        }

        BMP_FILEHEADER fileHeader = {0};
        fileHeader.bfType = 0x4D42; // 'BM'
        fileHeader.bfSize = sizeof(BMP_FILEHEADER) + sizeof(BMP_INFOHEADER) + img->size;
        fileHeader.bfOffBits = sizeof(BMP_FILEHEADER) + sizeof(BMP_INFOHEADER);

        BMP_INFOHEADER infoHeader = {0};
        infoHeader.biSize = sizeof(BMP_INFOHEADER);
        infoHeader.biWidth = img->width;
        infoHeader.biHeight = img->height;
        infoHeader.biPlanes = 1;
        infoHeader.biBitCount = colorspace *8;
        infoHeader.biSizeImage = img->size *colorspace;        

        fwrite(&fileHeader, sizeof(BMP_FILEHEADER), 1, file);
        fwrite(&infoHeader, sizeof(BMP_INFOHEADER), 1, file);

        unsigned char *data = (unsigned char *)malloc(img->size *colorspace);

        for(size_t i = 0; i < img->size; i++) {
            if(colorspace == 3) {
                data[(i *colorspace) +2] = red(img->pixel[i]);
                data[(i *colorspace) +1] = green(img->pixel[i]);
                data[(i *colorspace) +0] = blue(img->pixel[i]);
            } else {
                data[(i *colorspace) +3] = red(img->pixel[i]);
                data[(i *colorspace) +2] = green(img->pixel[i]);
                data[(i *colorspace) +1] = blue(img->pixel[i]);
                data[(i *colorspace) +0] = alpha(img->pixel[i]);
            }
        }
        fwrite(data, img->size *colorspace, 1, file);            

        fclose(file);

        free(data);
        data = NULL;
    }
    // free(img->data);

    return 0;
}

#pragma GCC diagnostic pop

#endif