//Import Libraries
#include "E101.h"
#include <stdio.h>

//Default Constant Values
const int SCAN_ROW = 120;
const int DEBUG = 1; //Enable Debug messages
const int LEFT_MOTOR = 1;
const int RIGHT_MOTOR = 2;
const int PIC_WIDTH = 320;
const int ALL_BLACK_THRESHOLD = 100; //Lower bound when it's considered all black
const int ALL_WHITE_THRESHOLD = 100; //Upper bound when it's considered all white

//Networking Variables
char SERVER[15] = {"130.195.6.196"};
int PORT = 1024;
char PLEASE_ARRAY[24] = {"Please"};
char passwordArray[24] = {};

//PID CONSTANTS
const float kp = 0.0019;
const float ki = 0.000000001;
const float kd = 0.0002;

//Global Vars
int V_INIT = 45; //Default motor speed
int FOLLOWING_LINE = 1; //Boolean value for if it should be following
int LINE_THRESHOLD = 0; //Declare the threshold which is calculated in the followLine() method
int PIC_MID = PIC_WIDTH / 2; //Middle of image
float TOTAL_ERROR = 0; //Declare total error for integral signal
float PREV_ERROR = 0; //Declare previous error for derivative signal
int quadrant = 1; //Keep track of which quadrant we are in
int min = 255;
int max = 0;

//Function Declarations
//Line following Quadrant 2
void followLine();
void calcMinMax();
void convertToBW(int list[PIC_WIDTH]);
int calcPropError(int list[PIC_WIDTH]);
float calcSignal(int prop_err);
void moveMotors(int speed);
//Networking function for quadrant 1
void openGate();

int main(){
	init();
	openGate();
	followLine();
	return 0;
}

/** openGate
 *  Function that opens the gate for quadrant 1.
 *	Connects to server, sends "Please" and receives a password via reference
 *	Sends password back to server and gate opens
 *  Increments the quadrant as we should have left that zone after the gate opens.
 */
void openGate(){
	if(connect_to_server(SERVER, PORT) == 0){
		printf("Connection Established\n");
		send_to_server(PLEASE_ARRAY);
		receive_from_server(passwordArray);
		send_to_server(passwordArray);
		printf("Password: %s\n", passwordArray);
	}
	quadrant++;
}

/** moveMotors
 *	Function that sets both motors to the same speedLeft
 *	@params:
 *		- speed the value to set the motors to
 */
void moveMotors(int speed){
	set_motor(LEFT_MOTOR, speed);
	set_motor(RIGHT_MOTOR, speed);
}

/** followLine
 *  Function that gets the robot to follow the Line
 *  Moves robot back when it has lost the line (all black)
 *  terminates when we reach the end of quadrant 2 (all white with the white strip)
 */
void followLine(){
	//Initialise Signal Variables
	int prop_err = 0;
	float final_sig = 0;

	while (FOLLOWING_LINE) {
		//Reset these variables
		min = 255;
		max = 0;
		int pixels[PIC_WIDTH];

		take_picture();

		calcMinMax();

		if (DEBUG) {
			printf("Max: %d, Min: %d\n", max, min);
		}

		//determine if it's "all black", "all white", or still on the line
		if (max < ALL_BLACK_THRESHOLD) {
			moveMotors(-35); //Move back
			sleep1(2,0);
		} else if (min > ALL_WHITE_THRESHOLD) {
			printf("Look it's an all white thingy\n");
			FOLLOWING_LINE = 0; //Terminate Loop
			quadrant++;
		} else {
			LINE_THRESHOLD = (max + min)/2;

			convertToBW(pixels);

			prop_err = calcPropError(pixels);

			TOTAL_ERROR += prop_err;

			final_sig = calcSignal(prop_err);

			PREV_ERROR = prop_err;

			//Output to the motors respectively
			set_motor(LEFT_MOTOR, V_INIT - final_sig);
			set_motor(RIGHT_MOTOR, V_INIT + final_sig);
		}
	}
}

/** calcMinMax
 *  Function that finds the min and max of a list.
 *  Assigns these values to variables to the global vars
 */
void calcMinMax(){
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

/** convertToBW
 *  Function that uses the threshold to determine which pixels are white-ish and black-ish
 *  Adds each value to an Array via reference to it's location
 *  @params:
 *		- list[PIC_WIDTH] an array of integers that is the length of the width of the picture from the camera
 */
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

/** calcPropError
 *	Function that calculates the proportional error of the line
 *  @params:
 *		- list[PIC_WIDTH] an array of numbers either 1 (white) or 0 (black)
 *  @returns:
 *		- current_error integer that is the proportional error from the line
 */
int calcPropError(int list[PIC_WIDTH]){
	int error;
	int current_error = 0;

	for (int i = 0; i < PIC_WIDTH; i++) { //For every pixel in the pixels array
		error = (i-PIC_MID) * list[i]; //calculate a proportional error
		current_error += error;
	}

	return current_error;
}

/** calcSignal
 *  Function that calculates the total signal to apply to the motors to keep the robot straight
 *  First calculates the proportional, integral and derivative signal then adds them together for the final one.
 *  @params:
 *		- prop_err integer that is the proportional error of how far off the line the robot is
 *  @returns:
 *		- final_sig the value to apply to the motors to straighten it up
 */
float calcSignal(int prop_err){
	float prop_sig = (float) prop_err * kp; //Calculate the proportional signal
	float integ_sig = TOTAL_ERROR * ki; //Calculate the integral signal
	float deriv_sig = ((float) prop_sig - PREV_ERROR) * kd; //Calculate the derivative signal
	float final_sig = prop_sig + integ_sig - deriv_sig; //Calculate the total signal by adding all the values to it.

	if (DEBUG) {
		printf("Proportional: %f, Integral: %f, Derivative: %f, Final: %f\n", prop_sig, integ_sig, deriv_sig, final_sig);
	}

	return final_sig;
}
