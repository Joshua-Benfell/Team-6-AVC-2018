//Import Libraries
#include "E101.h"
#include <stdio.h>

//Default Constant Values
const int SCAN_ROW = 120;
const int DEBUG =  0;
const int LEFT_MOTOR = 1;
const int RIGHT_MOTOR = 2;

int main(){

	init();

	int v_init = 50; //Speed to go when error is 0
	int following_line = 1; //Is following the line, allows termination

	//ERROR VALUES
	float current_error = 0;
	float prev_error = 0;
	float total_error = 0;
	int threshold = 0;

	//PID CONSTANTS
	float kp = 0.003;
	float ki = 0.00001;
	float kd = 0.001;

	//Initialise Signal Variables
	float prop_sig = 0;
	float integ_sig = 0;
	float deriv_sig = 0;
	float final_sig = 0;

	//Initialise Misc Variables
	int i, pix;
	float error;

	while(following_line) { //While we haven't terminated the code

		//reset some variables
		int min = 255;
		int max = 0;
		int pixels[320]; //Array of black and white pixels
		current_error = 0;

		take_picture(); // Take a photo

		for (i = 0; i < 320; i++) {
			pix = get_pixel(i, 120, 3); //For every pixel in the middle row
			if (pix < min) { //Compare with min and max values and update min and max
				min = pix;
			}
			if (pix > max) {
				max = pix;
			}
		}

		threshold = (max + min)/2; //Calculate the threshold between white and black pixels

		for (i = 0; i < 320; i++) {
			pix = get_pixel(i, 120, 3); //For every pixel in the middle row
			if (pix > threshold) { //If the pixel is greater than the threshold then add white to the pixels array
				pixels[i] = 1;
			}
			else { //else add 0
				pixels[i] = 0;
			}
		}

		for (i = 0; i < 320; i++) { //For every pixel in the pixels array
			error = (i-160) * pixels[i]; //calculate a proportional error
			current_error += error;
		}

		total_error += current_error; //add the current error to the total error

		prop_sig = current_error * kp; //Calculate the proportional signal
		integ_sig = total_error * ki; //Calculate the integral signal
		deriv_sig = (current_error - prev_error) * kd; //Calculate the derivative signal
		final_sig = prop_sig + integ_sig + deriv_sig; //Calculate the total signal buy adding all the values to it.
		final_sig = (final_sig/(160*1*kp))*255; //Might not need this line


		prev_error = current_error; //Set the previous error to the current error

		if (DEBUG) {
			printf("Proportional: %f, Integral: %f, Derivative: %f, Final: %f\n", prop_sig, integ_sig, deriv_sig, final_sig);
		}

		set_motor(RIGHT_MOTOR, v_init - final_sig); //Output to the motors respectively
		set_motor(LEFT_MOTOR, v_init + final_sig);
	}
}
