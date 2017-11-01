/** canny_edge.h

 Vivado HLS testbench. Loads bitmap image and executes synthesized function.
 The results are then stored with the help of bitmap.cpp in a 24 bitmap image.
 Author: Christos-Alexandros Nikolakakis
 University of Patras - Department of Electrical & Computer Engineering
 Email: nikolakakis.christos@gmail.com
 CREATED: 01-03-2017
 UPDATED: 03-04-2017

 */

#ifndef CANNY_H
#define CANNY_H

#include "stdint.h"
#include <iostream>
#include <cmath>
#include <cfloat>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "hls_stream.h"
#include "ap_axi_sdata.h"

// Parameters and settings definitions
#define IMAGE_WIDTH		150								// Image columns
#define IMAGE_HEIGHT	150							// Image rows
#define KERNEL_SIZE 	3								// Gaussian kernel size
#define PI				3.14159							// Pi
#define SUPPRESSED		0								// Suppressed pixel's value = 0
#define EDGE			255								// Edge pixel's value = 255 (1 - normalized)
#define ABS(x)         	((x>0)? x : -x)					// Absolute value of a number (used instead of built-in abs because of variable type limitations)

static float kernel[KERNEL_SIZE * KERNEL_SIZE] = { 	0.077847, 0.123317, 0.077847,
													0.123317, 0.195346, 0.123317,
													0.077847, 0.123317, 0.077847};

static uint8_t 	input[IMAGE_WIDTH*IMAGE_HEIGHT];
static uint8_t 	gray[IMAGE_WIDTH*IMAGE_HEIGHT];
static int8_t	DX[IMAGE_WIDTH*IMAGE_HEIGHT];
static int8_t 	DY[IMAGE_WIDTH*IMAGE_HEIGHT];
static uint8_t 	hai[200];

void gaussian(uint8_t *, uint8_t *);
void grad(uint8_t *, int8_t *, int8_t *);
void edgeID(uint8_t *, uint8_t *, int8_t *, int8_t *, int threshold);
void gradMag(int8_t *, int8_t *, uint8_t *);
#endif
