#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/timeb.h>

#include "lodepng.h"
#include "config.h"

#ifndef _UTIL_H
#define _UTIL_H

unsigned char color2byte(float v);

void hsv2rgb(float h, float s, float v, unsigned char* rgb);

void init_colormap(int len, unsigned char* map);

void save_image(const char* filename, const unsigned char* image, unsigned width, unsigned height);

void description(char* name, char* desc, int blocks, int threads);

void progress(char* name, int r, double time, int blocks, int threads);

void report(char* name, double* times, float avg_blocks, float avg_threads);

#endif
