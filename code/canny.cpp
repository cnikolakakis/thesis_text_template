/**
 canny.cpp

 Canny Edge Detector Implementation in Vivado HLS.

 Processing steps are:

 1) Data input from axis
 1) Smoothing image with gaussian filter
 2) compute gradient DX, DY of smoothed image
 3) compute magnitude of DX & DY
 4) non-maxima suppression
 5) Hysteresis
 6) Thresholding
 7) Output data to axis

 Author: Christos-Alexandros Nikolakakis
 University of Patras - Department of Electrical & Computer Engineering
 Email: nikolakakis.christos@gmail.com
 CREATED: 01-03-2017
 UPDATED: 06-10-2017
 */

#include "canny.h"

// AXI Instance
typedef ap_axis<32, 1, 1, 1> AXI_VAL;

void canny(hls::stream<AXI_VAL> &inStream, hls::stream<AXI_VAL> &outStream, int threshold) {

#pragma HLS INTERFACE axis port=inStream
#pragma HLS INTERFACE axis port=outStream
#pragma	HLS INTERFACE s_axilite port=threshold bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL_BUS

	AXI_VAL valueOUT;
	AXI_VAL valueIN;

#pragma HLS array_partition variable=input cyclic factor=2

	// stream in the input image data in grayscale format
	for (int i = 0 ; i < IMAGE_HEIGHT ; i ++ ) {

		for (int j = 0; j < IMAGE_WIDTH; j++) {
#pragma HLS pipeline rewind

			valueIN = inStream.read();
			input[i * IMAGE_WIDTH + j] = valueIN.data ;
		}
	}

	gaussian(input, gray);
	grad(gray, DX, DY);
	gradMag(DX, DY, gray);
	edgeID(gray, input, DX, DY, threshold); //Reused input as output buffer

	for (int i = 0 ; i < IMAGE_HEIGHT ; i ++ ) {

		for (int j = 0; j < IMAGE_WIDTH; j++) {
#pragma HLS pipeline rewind
			int t = i * IMAGE_WIDTH + j;
			valueOUT.data = input[t];
			outStream.write(valueOUT);
		}
	}
}

void gaussian(uint8_t *in, uint8_t *out) {

	int32_t m, n, mm, nn;
	int32_t kCenterX, kCenterY; // kernel's center
	int32_t rowIndex, colIndex; // flipped kernel indexes
	int32_t sum = 0;

	kCenterX = KERNEL_SIZE >> 1;
	kCenterY = KERNEL_SIZE >> 1;

L3: for (int i = 0; i < IMAGE_HEIGHT; ++i) {
L4: for (int j = 0; j < IMAGE_WIDTH; ++j) {
#pragma HLS pipeline rewind
			sum = 0;
L5: for (m = 0; m < KERNEL_SIZE; ++m) {
				mm = KERNEL_SIZE - 1 - m;
L6: for (n = 0; n < KERNEL_SIZE; ++n) {

					nn = KERNEL_SIZE - 1 - n;
					rowIndex = i + m - kCenterY;
					colIndex = j + n - kCenterX;

					if (rowIndex
					        >= 0 && rowIndex < IMAGE_HEIGHT && colIndex >= 0 && colIndex < IMAGE_WIDTH) {
						sum = sum
						      + in[rowIndex * IMAGE_WIDTH + colIndex]
						      * kernel[mm * KERNEL_SIZE + nn];
					}
				}
			}
			out[i * IMAGE_WIDTH + j] = (uint8_t) (ABS(sum)); // Output to GRT Buffer
		}
	}
}

void grad(uint8_t *in, int8_t *DX, int8_t *DY) {

	int t, i, j;

	for (i = 0; i < IMAGE_HEIGHT; ++i) {
#pragma HLS pipeline rewind
		t = i * IMAGE_WIDTH; // current position X per line
		// gradient at the first pixel of each line
		// note: the edge,pix[t-1] doesn't exist
		DX[t] = in[t + 1] - in[t];
		// gradients where NOT edge
		for (j = 1; j < IMAGE_WIDTH - 1; ++j) {
			t++;
			DX[t] = in[t + 1] - in[t - 1];
		}
		// gradient at the last pixel of each line
		t++;
		DX[t] = in[t] - in[t - 1];
	}

	for (j = 0; j < IMAGE_WIDTH; ++j) {
#pragma HLS LOOP_TRIPCOUNT
#pragma HLS pipeline rewind
		t = j; // current Y position per column
		// gradient at the first pixel
		DY[t] = in[t + IMAGE_WIDTH] - in[t];
		// gradients for NOT edge pixels
		for (i = 1; i < IMAGE_HEIGHT - 1; ++i) {
			t += IMAGE_WIDTH;
			DY[t] = in[t + IMAGE_WIDTH] - in[t - IMAGE_WIDTH];
		}
		// gradient at the last pixel of each column
		t += IMAGE_WIDTH;
		DY[t] = in[t] - in[t - IMAGE_WIDTH];
	}
}

void gradMag(int8_t *DX, int8_t *DY, uint8_t	*out) {

}

void edgeID(uint8_t	*input, uint8_t	*output, int8_t	*DX , int8_t *DY , int threshold) {

	int8_t GX = 0, GY = 0;
	int i, j ;

	for (i = 1; i < IMAGE_HEIGHT - 1; ++i) {
		for (j = 1; j < IMAGE_WIDTH - 1; ++j) {
#pragma HLS pipeline rewind
			GX = DX[i * IMAGE_WIDTH + j];	// temp - easier to handle
			GY = DY[i * IMAGE_WIDTH + j];

			if (GX > 0) {
				GX = 1; 	// Direction of gradient is right
			} else if (GX < 0) {
				GX = -1; 	// Direction of gradient is left
			}

			if (GY > 0) {
				GY = 1; 	// Direction of gradient is  up
			} else if (GY < 0) {
				GY = -1;	// Direction of gradient is down
			}
			if ((input[i * IMAGE_WIDTH + j] < input[(i + GY) * IMAGE_WIDTH + j + GX])
			        || (input[i * IMAGE_WIDTH + j] < input[(i - GY) * IMAGE_WIDTH + j - GX])) {
				output[i * IMAGE_WIDTH + j] = SUPPRESSED;
			} else if (input[i * IMAGE_WIDTH + j] >= threshold) {
				output[i * IMAGE_WIDTH + j] = EDGE;
			} else {
				output[i * IMAGE_WIDTH + j] = SUPPRESSED;
			}
		}
	}
}

