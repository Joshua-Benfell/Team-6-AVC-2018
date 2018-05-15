//Import Libraries
#include "E101.h"
#include <stdio.h>

//Not sure what these libraries are for, ask Michael
#include <iostream>
#include <ctime>

//Default Constant Values
const int SCAN_ROW = 120;
const int DEBUG = 1;
const int LEFT_MOTOR = 1;
const int RIGHT_MOTOR = 2;
const int PIC_WIDTH = 320;

//Networking Variables
char SERVER[15] = {"130.195.6.196"};
int PORT = 1024;
char PLEASE_ARRAY[24] = {"Please"};
char passwordArray[24] = {};

//PID CONSTANTS
const float kp = 0.0019;
const float ki = 0.0000000005;
const float kd = 0.0002;

//Global Vars
int V_INIT = 45;
int FOLLOWING_LINE = 1;
int LINE_THRESHOLD = 0;
int PIC_MID = PIC_WIDTH / 2;
float TOTAL_ERROR = 0;
float PREV_ERROR = 0;

//Function Declarations
void followLine();
void calcMinMax(int min, int max);
void convertToBW(int list[PIC_WIDTH]);
int calcPropError(int list[PIC_WIDTH]);
float calcSignal(int prop_err);

void openGate();

int main(){
	init();
	openGate();
	followLine();
	return 0;
}

void openGate(){
	if(connect_to_server(SERVER, PORT) == 0){
		printf("Connection Established\n");
		send_to_server(PLEASE_ARRAY);
		receive_from_server(passwordArray);
		send_to_server(passwordArray);
		printf("Password: %s\n", passwordArray);
	}
}

void followLine(){
	//Initialise Signal Variables
	int prop_err = 0;
	float final_sig = 0;

	while (FOLLOWING_LINE) {
		int min = 255;
		int max = 0;
		int pixels[PIC_WIDTH];
		float current_error = 0;

		take_picture();

		calcMinMax(min, max);

		LINE_THRESHOLD = (max + min)/2;

		convertToBW(pixels);

		prop_err = calcPropError(pixels);

		TOTAL_ERROR += prop_err;

		final_sig = calcSignal(prop_err);

		PREV_ERROR = prop_err;

		set_motor(LEFT_MOTOR, V_INIT - final_sig); //Output to the motors respectively
		set_motor(RIGHT_MOTOR, V_INIT + final_sig);
	}
}

void calcMinMax(int min, int max){
	for (int i = 0; i < PIC_WIDTH; i++) {
		int pix = get_pixel(SCAN_ROW, i, 3); //For every pixel in the middle row
		if (pix < min) { //Compare with min and max values and update min and max
			min = pix;
		}
		if (pix > max) {
			max = pix;
		}
	}
}

void convertToBW(int list[PIC_WIDTH]){
	for (int i = 0; i < PIC_WIDTH; i++) {
		int pix = get_pixel(SCAN_ROW, i, 3); //For every pixel in the middle row
		if (pix > LINE_THRESHOLD) { //If the pixel is greater than the threshold then add white to the pixels array
			list[i] = 1;
		}
		else { //else add 0
			list[i] = 0;
		}
	}
}

int calcPropError(int list[PIC_WIDTH]){
	int error;
	int current_error = 0;

	for (int i = 0; i < PIC_WIDTH; i++) { //For every pixel in the pixels array
		error = (i-PIC_MID) * list[i]; //calculate a proportional error
		current_error += error;
	}

	return current_error;
}

float calcSignal(int prop_err){
	float prop_sig = (float) prop_err * kp; //Calculate the proportional signal
	float integ_sig = TOTAL_ERROR * ki; //Calculate the integral signal
	float deriv_sig = ((float) prop_sig - PREV_ERROR) * kd; //Calculate the derivative signal
	float final_sig = prop_sig + integ_sig + deriv_sig; //Calculate the total signal by adding all the values to it.
	//final_sig = (final_sig/(160*1*kp))*255; //Might not need this line
	if (DEBUG) {
		printf("Proportional: %f, Integral: %f, Derivative: %f, Final: %f\n", prop_sig, integ_sig, deriv_sig, final_sig);
	}
	return final_sig;
}
