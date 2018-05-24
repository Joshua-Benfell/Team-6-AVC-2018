//Import Libraries
#include "E101.h"
#include <stdio.h>

const int PIC_WIDTH = 320;
const int SCAN_ROW = 120;
int min = 255;
int max = 0;

void calcMinMax(int chan);

int main(){
	init();
	while(1){
		take_picture();

		calcMinMax(0);
		printf("Rmin: %d, Rmax: %d", min, max);
		calcMinMax(1);
		printf(" Gmin: %d, Gmax: %d", min, max);
		calcMinMax(2);
		printf(" Bmin: %d, Bmax: %d", min, max);
		calcMinMax(3);
		printf(" Wmin: %d, Wmax: %d\n", min, max);
	}
}


calcMinMax(int chan){
	min = 255;
	max = 0;
	for (int i = 0; i < PIC_WIDTH; i++) {

		int pix = get_pixel(SCAN_ROW, i, chan); //For every pixel in the middle row

		if (pix < min) { //Compare with min and max values and update min and max
			min = pix;
		}
		if (pix > max) {
			max = pix;
		}
	}
}
