#ifndef __SKETCH_H
#define __SKETCH_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wconversion"

#include <stdio.h>
#include "util.h"
#include "color.h"
#include "image.h"
#include "process.h"


Process* root;


void settings() {
	// displayWidth = GetSystemMetrics(SM_CXMAXIMIZED);
    // displayHeight = GetSystemMetrics(SM_CYMAXIMIZED);
	
}


static void frameRate(float fps) {
	if (fps <= 0 || fps >= 1000000) {
		printf("Invalid frameRate: %.2ffps", fps);
	} else {
		root->internal_fps = fps;
		root->internal_frameTime = (unsigned long)(1000000.0f / fps);
	}
}

static void noLoop() {
	root->loop = 0;
}

static void size(int w, int h) {
	if (w <= 0 || h <= 0) {
		printf("Invalid size: (w: %d, h: %d)", w, h);
		root->quit = 1;
	} else {
		root->width = w;
		root->height = h;
	}
}

static void fullScreen() {
	root->width = displayWidth;
	root->height = displayHeight;
}

static void stop() {
	root->quit = 1;
	root->ret = 1;
}

#pragma GCC diagnostic pop

#if 0 // color print ⬎

#include <stdio.h>
#include "colorPrint.h"


int draw() {
	printTest();	

	return 0;
}

#endif // color print ⬏

#if 0 // image test .bmp ⬎

#include <math.h>
#include "util.h"
#include "imageUtil.h"
#include "drawUtil.h"


int setup() {
	size(1920, 1080);
    noLoop();
    return 0;
}

int draw() {	
	startTime();	
	
	Image img;
	createImage(&img, 1080, 720, RGB8);

	int scale = 8;

	fill(RGBA(0, 255, 255, 127));
	circle(&img, 540, 360, 100);
	fill(RGB(255, 0, 0));
	circleBorder(&img, 540, 360, 100);
	
	fill(RGB(255, 255, 0));
	rectBorder(&img, 440, 310, 200, 100);

	fill(0xFF00FF);
	for(int i = 0; i < img.width/scale; i++) {
		circle(&img, i *scale, 360+ floor(180* sin((map(i, 0, img.width/scale, 0, 1) *6.283))), scale);
	}
	
	saveBMP(&img, "C:/Users/eleftherakis/Downloads/bild.bmp");		
	
	unsigned long t = stopTime();
	printInfo("took: %lu us (=> %d ms)\n", t, (int)floor(t/1000.0));

    return 0;
}

#endif // image test .bmp ⬏

#if 0 // image test load & resize .bmp ⬎

#include <math.h>
#include "include/util.h"
#include "include/image.h"
#include "include/imageFile.h"
#include "include/draw.h"


int setup() {
	size(1920, 1080);
    noLoop();
    return 0;
}

int draw() {	
	startTime();	
	
	Image img;
	loadBMP(&img, "C:/Users/eleftherakis/Entwicklung/C/c processing/image archive/12bild.bmp");
	printImage(&img, 32, 16);	
	printImageInfo(&img);	

	Image out;
	createImage(&out, 1620, 1080);

	Image img2;
	Image img4;
	getImage(&img2, &img, 200, 200, 200, 200);
	resizeImage(&img4, &img2, 400, 100);
	putImage(&img4, &out, 1000, 100);
	putImage(&img2, &out, 700, 100);

	Image img3;
	scaleImage(&img3, &img, 0.5);

	circle(&out, 500, 800, 400);
		
	putImage(&img3, &out, 810, 540);
	putImage(&img3, &out, -405, -270);
	printImage(&out, 32, 16);
	printImageInfo(&out);		

	saveBMP(&out, "C:/Users/eleftherakis/Downloads/12bild2.bmp", RGB8);	
	
	unsigned long t = stopTime();
	printInfo("took: %lu us (=> %d ms)\n", t, (int)floor(t/1000.0));

    return 0;
}

#endif // image test load & resize .bmp ⬏

#if 0 // image test perlin noise ⬎

#include <math.h>
#include "util.h"
#include "imageUtil.h"
#include "perlinNoise.h"
#include "drawUtil.h"

int setup() {
	size(1920, 1080);
    noLoop();
    return 0;
}

int draw() {	
	startTime();	
	
	Image img;
	createImage(&img, 1080, 720, RGB8);

	int scale = 8;

	for(int i = 0; i < img.height/scale; i++) {
		for(int j = 0; j < img.width/scale; j++) {
			float val = perlin2d(j, i, 0.1, 2);
			int col = GREY((unsigned char)(val* 255));	
			fill(col);
			rect(&img, j *scale, i *scale, scale, scale);
		}
	}

	fill(0xFF0000);
	translate(100, 100);
	rect(&img, 100, 100, 200, 200);

	pushStyle();
	fill(0x00FF00);
	line(&img, 1, 0, 201, 200);
	popStyle();	

	fill(0xFFFF00);
	line(&img, 0, 0, 200, 200);


	saveBMP(&img, "C:/Users/eleftherakis/Downloads/bild.bmp");		
	
	unsigned long t = stopTime();
	printInfo("took: %lu us (=> %d ms)\n", t, (int)floor(t/1000.0));

    return 0;
}

#endif // image test perlin noise ⬏

#if 0 // flowfield ⬎

#include <math.h>
#include "include/util.h"
#include "include/color.h"
#include "include/image.h"
#include "include/imageFile.h"
#include "include/noise.h"
#include "include/draw.h"


float noiseScale = 500.0;


typedef struct {    
    vec2 pos;
    float speed;
    int col;
} Particle;


int setup() {
	size(1920, 1080);
    noLoop();
    return 0;
}

int draw() {	
	startTime();	
	
	Image *img = createImage(width, height);

	background(img, RGB(21, 8, 50));

	makeWorley(20);

	int numParticles = 400;

	Particle* particels;
	particels = (Particle*)malloc(numParticles * sizeof(Particle));

	if (particels == NULL) {
        printFunctionError("memory allocation failed", "draw()");
		quit = 1;        
    }

	for(int i = 0; i < numParticles; i++) {		
		// vec2 pos = {i* floor(width *1.0f / numParticles), height * (i *1.0f / numParticles)};	// y = x
		// vec2 pos = {i* floor(width *1.0f / numParticles), height/2 +(height/2* sin((i *1.0f / numParticles) *2.0 *TWO_PI))};	// y = sin(x *2)	
		vec2 pos = {floor(randomInt(width)), floor(randomInt(height))};		
		float speed = 1.2;

		size_t size = sizeof(color_gradient_cerulean) / sizeof(color_gradient_cerulean[0]);
		int col = getRandomCol(color_gradient_cerulean, size);		

		Particle p = {pos, speed, col};
		particels[i] = p;
	}

	for(int cnt = 0; cnt < 800; cnt++) {
		for(int i = 0; i < numParticles; i++) {			
			// double angle = map(perlin((particels[i].pos.x1 -2200) /noiseScale, (particels[i].pos.x2 +300) /noiseScale, 0.5, 4), -1, 1, 0, 1) *TWO_PI *noiseScale;	
			double angle = worley((particels[i].pos.x1), (particels[i].pos.x2), width, height) *TWO_PI *noiseScale;	
			particels[i].pos.x1 += (float)(cos(angle) *particels[i].speed);
			particels[i].pos.x2 += (float)(sin(angle) *particels[i].speed);
			if (isPIPf(particels[i].pos.x1, particels[i].pos.x2, 0, 0, width, height) < 0) {
				particels[i].pos.x1 = width/2;
				particels[i].pos.x2 = height/2;
				// particels[i].pos.x1 = floor(randomInt(width));
				// particels[i].pos.x2 = floor(randomInt(height));
			}
			fill(particels[i].col);
			circle(img, particels[i].pos.x1, particels[i].pos.x2, 2);
		}
	}	

	saveBMP(img, "C:/Users/eleftherakis/Downloads/bild.bmp", RGB8);		
	
	unsigned long t = stopTime();
	printInfo("took: %lu us (=> %d ms)\n", t, (int)floor(t/1000.0));

    return 0;
}

#endif // flowfield ⬏

#if 0 // subpixelart ⬎

#include <math.h>
#include "include/util.h"
#include "include/color.h"
#include "include/image.h"
#include "include/imageFile.h"
#include "include/draw.h"


int setup() {
	size(120, 60);
	noLoop();
	return 0;
}

int draw() {
	startTime();

	Image *img = createImage(width, height);
	circle(img, 64, 32, 24);
	line(img, 0, 0, 128, 64);
	line(img, 128, 0, 0, 64);
	grayscaleImage(img);	

	Image *out = createImage(img->width/3, img->height/3);	

	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x+=3) {
			int r = brightness(getPixel(img, x, y));
			int g = brightness(getPixel(img, x +1, y));
			int b = brightness(getPixel(img, x +2, y));

			setPixel(out, x, y, RGB(r, g, b));
		}
	}

	saveBMP(out, "C:/Users/eleftherakis/Downloads/subpixelart.bmp", RGB8);

	unsigned long t = stopTime();
	printInfo("took: %lu us (=> %d ms)\n", t, (int)floor(t / 1000.0));

	return 0;
}

#endif // subpixelart ⬏

#if 0 // colorfilter ⬎

#include <math.h>
#include "include/util.h"
#include "include/color.h"
#include "include/image.h"
#include "include/imageFile.h"
#include "include/draw.h"


int setup() {
	// size(480, 320);
	noLoop();
	return 0;
}

int draw() {
	startTime();

	// Image *img = createImage(width, height);
	Image *img = loadBMP("C:/Users/eleftherakis/Downloads/sample1.bmp");
	size(img->width, img->height);
	for(int y = 0; y < height; y++) {
		fill(lerpColors(color_gradient_rainbow, 6, map(y, 0, height, 0, 1)));
		hline(img, 0, width, y);
	}
	fill(0xFF0000);
	circle(img, 200, 100, 24);	

	Image *out = createImage(img->width, img->height);	

	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			int col = getPixel(img, x, y);
			int r = red(col);
			int g = green(col);
			int b = blue(col);

			int newCol = RGB(r, b, g);

			setPixel(out, x, y, newCol);
		}
	}

	saveBMP(img, "C:/Users/eleftherakis/Downloads/color.bmp", RGB8);
	saveBMP(out, "C:/Users/eleftherakis/Downloads/color_filter.bmp", RGB8);

	unsigned long t = stopTime();
	printInfo("took: %lu us (=> %d ms)\n", t, (int)floor(t / 1000.0));

	return 0;
}

#endif // colorfilter ⬏

#if 0 // draw calls test⬎

#include <math.h>
#include "include/util.h"
#include "include/image.h"
#include "include/imageFile.h"
#include "include/draw.h"
#include "include/noise.h"

int setup() {
	size(1920, 1080);
    noLoop();
    return 0;
}

int draw() {	
	printInfo("start draw()");
	startTime();		
		

	Image *out = createImage(width, height);
	
	pushStyle();
	for(int x = 0; x < out->width; x++) {
		fill(lerpColors(color_gradient_virdis, 9, map(x, 0, out->width, 0, 1)));
		translate(x, 0);
		vline(out, 0, 0, out->height);
	}
	popStyle();

	fill(0x00FFFF);
	rect(out, width/4, height/4, width/2, height/2);

	fill(0xFF00FF);
	circle(out, width/2, height/2, height/4);

	fill(0xFF0000);
	circleBorder(out, width/4, height/4, height/4);

	// lineGradient(out, 0, height, width, 0, 6, color_gradient_rainbow, 6);

	printImage(out, 32, 16);
	printImageInfo(out);		
	
	saveBMP(out, "C:/Users/eleftherakis/Downloads/12bild2.bmp", RGB8);	

	
	unsigned long t = stopTime();
	printInfo("draw() took: %lu us (=> %d ms)\n", t, (int)floor(t/1000.0));

    return 0;
}

#endif // draw calls test⬏

#if 0 // image test window ⬎

#include "util.h"
#include "imageUtil.h"
#include "windowUtil.h"

int setup() {
	size(1920, 1080);
    noLoop();
    return 0;
}

int draw() {

	Image img;
	createImage(&img, 480, 320);

	Window win = createWindow("test Window", img.width, img.height);
	win.canvas = &img;
	for(int i = 0; i < img.height; i++) {
		for(int j = 0; j < img.width; j++) {
			if((i +j) % 2 == 0) {
				setPixel(&img, j, i, col(255, 255, 255, 255));
				setPixel(win.canvas, j, i, col(255, 255, 255, 255));
			} else {
				setPixel(&img, j, i, col(0, 0, 0, 255));
				setPixel(win.canvas, j, i, col(0, 0, 0, 255));
			}
		}
	}

	saveBMP(&img, "C:/Users/eleftherakis/Downloads/bild.bmp");

	drawWindow(&win);
	handleWindow(&win);

	Image out;
	loadBMP(&out, "C:/Users/eleftherakis/Downloads/bild.bmp");

	int col = getPixel(&img, 0, 0);
	printf("%d,%d,%d", red(col), green(col), blue(col));
	
	col = getPixel(&img, 0, 1);
	printf(" %d,%d,%d", red(col), green(col), blue(col));

    return 0;
}

#endif // image test window ⬏

#if 1 // GDI ⬎

#include <stdio.h>
#include <windows.h>

#include "imageFile.h"
#include "draw.h"


Image *img;

int setup() {

	img = createImage(1280, 480);
	// img = loadBMP("C:\\Users\\eleftherakis\\Entwicklung\\C\\c processing\\image archive\\12bild.bmp");
	// img = loadBMP("C:\\Users\\eleftherakis\\Downloads\\1resized.bmp");
	
	// img = scaleImage(img, 0.5);
	size(img->width, img->height);

	// printImage(img, 32, 16);

	printTest();

	// saveBMP(img, "C:\\Users\\eleftherakis\\Downloads\\sampafale1.bmp", 3);

    //noLoop();

	background(img, 0xFF00FFFF);
    return 0;
}

Image* draw() {

	setPixel(img, mouseX, mouseY, RGBA(255, 0, 0, 255));

	if(root->win->mouseEvent & 1) {
		printf("quit\n");
		stop();
	}
	
	return img;
}


#endif // GDI ⬏


#if 0 // template ⬎

#include <stdio.h>


int setup() {
	size(1920, 1080);
    noLoop();
    return 0;
}

int draw() {
	int zahl;
	printf("Bitte eine schoene Ganzzahl eingeben:\n");
	fflush(stdin);
	scanf("%d", &zahl);
	printf("\nZahl: %d\n", zahl);	
}

#endif // template ⬏

#endif