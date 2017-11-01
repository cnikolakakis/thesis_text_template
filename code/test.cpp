/** testCanny.cpp

 Canny Edge Detection - Vivado HLS testbench.
 Loads bitmap image and executes the edge detection algorithm in hardware.
 It then compares the two processed images checking for incosistencies.
 Bitmap loading and writing is being done with the help of easyBMP - a Cross-Platform Windows Bitmap Library
 URL -> http://easybmp.sourceforge.net/
 Author: Christos-Alexandros Nikolakakis
 University of Patras - Department of Electrical & Computer Engineering
 Email: nikolakakis.christos@gmail.com
 CREATED: 01-03-2017
 UPDATED: 06-10-2017

*/

#include "canny.h"
#include "EasyBMP.h"

using std::cout;
using std::endl;

typedef ap_axis<32,1,1,1> AXI_VAL;

void canny(hls::stream<AXI_VAL> &inStream, hls::stream<AXI_VAL> &outStream, int threshold);

int main(int argc, char **argv){

	uint8_t inputTB[IMAGE_WIDTH*IMAGE_HEIGHT];

	// AXI4Stream Instance

	hls::stream<AXI_VAL> outputStream;
	hls::stream<AXI_VAL> inputStream;
	AXI_VAL output;
	AXI_VAL input;

	// BMP Instance
	BMP image;
	SetEasyBMPwarningsOff();

	image.ReadFromFile("input.bmp");

	for (int i = 0 ; i < IMAGE_HEIGHT ; i ++ ) {
		for (int j = 0; j < IMAGE_WIDTH; j++) {
		int t = i*IMAGE_WIDTH+j;
			input.data = image(j, i)->Red;
			inputStream <<  input;
		}
	}

	// Calling top function - canny()
	canny(inputStream, outputStream, 45);

	image.SetSize(IMAGE_WIDTH, IMAGE_HEIGHT);
	image.SetBitDepth(24);

	for (int i = 0 ; i < IMAGE_HEIGHT ; i ++ ) {
		for (int j = 0; j < IMAGE_WIDTH; j++) {
				outputStream.read(output);
				image(j, i)->Red = output.data;
				image(j, i)->Green = output.data;
				image(j, i)->Blue = output.data;
			}
		}
	image.WriteToFile("output.bmp"); // Writing 24Bit BMP Image using easyBMP lib

	int check_results = system("diff output.bmp gold.bmp");
		if (check_results != 0) {
			printf("Output image has mismatches with reference output image!\n");
			check_results = 1;
		}
		else {
			printf("Output image matches the reference output image\n");
		}
		return check_results;
	return 0;
}