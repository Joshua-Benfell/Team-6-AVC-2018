//Import Libraries
#include "E101.h"
#include <stdio.h>

//Default Constant Values
const int SCAN_ROW = 120;
const int SCAN_COL = 80;
const int DEBUG = 1; //Enable Debug messages
const int LEFT_MOTOR = 1;
const int RIGHT_MOTOR = 2;
const int PIC_WIDTH = 320;
const int PIC_HEIGHT = 240;
const int ALL_BLACK_THRESHOLD = 100; //Lower bound when it's considered all black
const int ALL_WHITE_THRESHOLD = 100; //Upper bound when it's considered all white
const int BLUE_THRESHOLD = 40;
const int RIGHT_ANGLE_VAL = 8000;
const int BOUND = 20;


//PID CONSTANTS
const float kp = 0.0019;
const float ki = 0.0000000012;
const float kd = 0.00001;

//Global Vars
int V_INIT = 50; //Default motor speed
int FOLLOWING_LINE = 1; //Boolean value for if it should be following
int FOLLOWING_MAZE = 0; //Boolean value for Quadrant3 init
int LINE_THRESHOLD = 0; //Declare the threshold which is calculated in the followLine() method
int PIC_MID = PIC_WIDTH / 2; //Middle of image
float TOTAL_ERROR = 0; //Declare total error for integral signal
float PREV_ERROR = 0; //Declare previous error for derivative signal
int quadrant = 1; //Keep track of which quadrant we are in
int min = 255;
int max = 0;


FILE* f = fopen("logFile.txt", "w+");

//Function Declarations
//Line following Quadrant 2
void followLine();
void calcMinMax();
void convertToBW(int list[PIC_WIDTH]);
int calcPropError(int list[PIC_WIDTH]);
float calcSignal(int prop_err);
void moveMotors(int speed);
void doFollowLine();
//Networking function for quadrant 1
void openGate();
void followMaze();
int lineOnSide();
void doWallMaze();
int detectQuadFour();

int main(){
	init();
	openGate();
	followLine();
	followMaze();
	//printf("Yeet");
	doWallMaze();
	fclose(f);
	return 0;
}

/** openGate
 *  Function that opens the gate for quadrant 1.
 *	Connects to server, sends "Please" and receives a password via reference
 *	Sends password back to server and gate opens
 *  Increments the quadrant as we should have left that zone after the gate opens.
 */
void openGate(){
    //Variables moved as they didn't need to be public
    //Networking Variables
    //Connection Address
    char serverAddr[15] = {"130.195.6.196"};

    //'password' for asking for password
    char pleaseArray[24] = {"Please"};
    char passwordArray[24] = {};

    if (connect_to_server(serverAddr, 1024) == 0){
        fprintf(f,"Connection Established\n");
		send_to_server(pleaseArray);
		receive_from_server(passwordArray);
		send_to_server(passwordArray);
		fprintf(f,"Password: %s\n", passwordArray);
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

	while (FOLLOWING_LINE) {
		//Reset these variables
		min = 255;
		max = 0;

		take_picture();

		calcMinMax();

        printf("Max: %d, Min: %d\n", max, min);


		//determine if it's "all black", "all white", or still on the line
		if (max < ALL_BLACK_THRESHOLD) {
            //output
            fprintf(f,"------------------------\n");
            fprintf(f,"No line detected!\n"
                   "Moving back!\n");
            fprintf(f,"------------------------\n");

		    moveMotors(-(V_INIT * 0.75)); //Move back
			//sleep1(1,0);    // allows time to find the

            int i = 0;
            while (max < ALL_BLACK_THRESHOLD){
                take_picture();
                calcMinMax();
                i++;
            }
            fprintf(f,"Black threshold loop ran for %d frames", i);
        sleep1(0,500000);

		} else if (min > ALL_WHITE_THRESHOLD) {

			printf("-----------------------------------\n");
		    printf("T Intersection Detected!\n "
                   " Now Moving to follow maze function\n");
            printf("-----------------------------------\n");

            //set this so vehicle stops for 1 second at T intersection to make quadrant transition clear
            moveMotors(0);
			sleep1(1 ,0);
            moveMotors(V_INIT);
			//Break should probably be used here
			FOLLOWING_LINE = 0; //Terminate Loop
			FOLLOWING_MAZE = 1; //Initiate Quadrant3 loop
			quadrant++;
            //Changed this to call the follow maze function
            //or here I guess
            break;

		} else {

			doFollowLine();

		}
	}

}

void doFollowLine(){
	int pixels[PIC_WIDTH];

	LINE_THRESHOLD = (max + min)/2;

	convertToBW(pixels);

	int prop_err = calcPropError(pixels);

	TOTAL_ERROR += prop_err;

	float final_sig = calcSignal(prop_err);

	PREV_ERROR = prop_err;

	//Output to the motors respectively
	set_motor(LEFT_MOTOR, V_INIT - final_sig);
	set_motor(RIGHT_MOTOR, V_INIT + final_sig);
}

void followMaze(){
	PREV_ERROR = 0;
	TOTAL_ERROR = 0;
    while (FOLLOWING_MAZE) {
        //Reset these variables
        min = 255;
        max = 0;

	V_INIT =  (V_INIT * 0.75);

        take_picture();
        calcMinMax();

        printf("Max: %d, Min: %d\n", max, min);


        //determine if it's "all black", "all white", or still on the line
        if (max < ALL_BLACK_THRESHOLD) {
            //output
            printf("------------------------\n");
            printf("No line detected!\n"
                   "Moving back!\n");
            printf("------------------------\n");

            //change back if -40 was used for a specific reason
		    //moveMotors(-40);
            //changing this to a "turn left" rather than "go back"
            set_motor(LEFT_MOTOR, -V_INIT);
            set_motor(RIGHT_MOTOR, V_INIT);

	        sleep1(1,0);

            //setting motors to turn on the spot
			if(lineOnSide() == 1){
				set_motor(LEFT_MOTOR, V_INIT);
	            set_motor(RIGHT_MOTOR, -V_INIT);
	            sleep1(0,500000);
			} else if (lineOnSide() == 2) {
				set_motor(LEFT_MOTOR, -V_INIT);
	            set_motor(RIGHT_MOTOR, V_INIT);
	            sleep1(0,500000);
			} else{
				set_motor(LEFT_MOTOR, V_INIT);
	            set_motor(RIGHT_MOTOR, -V_INIT);
				printf("Entered Black Threshold loop Q3\n");
				int i = 0;
	            while (max < ALL_BLACK_THRESHOLD){
	                take_picture();
	                calcMinMax();
	            }
	            printf("Black threshold loop ran for %d frames", i);
			}
        } else if (min > ALL_WHITE_THRESHOLD){
            //output
            printf("------------------------\n");
            printf("T intersection detected!\n"
                   "Turning left! \n");
            printf("------------------------\n");

            //Turning left on the spot (90 degree turn?)
            set_motor(LEFT_MOTOR, V_INIT);
            set_motor(RIGHT_MOTOR, -V_INIT);
            sleep1(0,500000);
        } else {
			if (detectQuadFour()){
			    printf("detected quadrant 4");
			    quadrant++;
				FOLLOWING_MAZE = 0;
				break;
			}
		 	doFollowLine();

        }
    }
}

int lineOnSide(){
	int sideMin = 255;
	int sideMax = 0;
	for (int i = 0; i < PIC_HEIGHT; i++) {

		int pix = get_pixel(i, SCAN_COL, 3); //For every pixel in the middle row

		if (pix < sideMin) { //Compare with min and max values and update min and max
			sideMax = pix;
		}
		if (pix > sideMax) {
			sideMax = pix;
		}
	}
	if (sideMax > ALL_BLACK_THRESHOLD){
		return 1;
	} else {
		sideMin = 255;
		sideMax = 0;
		for (int i = 0; i < PIC_HEIGHT; i++) {

			int pix = get_pixel(i, PIC_WIDTH-SCAN_COL, 3); //For every pixel in the middle row

			if (pix < sideMin) { //Compare with min and max values and update min and max
				sideMax = pix;
			}
			if (pix > sideMax) {
				sideMax = pix;
			}
		}
		if (sideMax > ALL_BLACK_THRESHOLD) {
			return 2;
		} else{
			return 0;
		}
	}
}

int detectQuadFour(){

	int blueMin = 255;
	int blueMax = 0;
	int redMin = 255;
	int redMax = 0;

	take_picture();


	for (int i = 140; i < 180; i++) {

		int bluePix = get_pixel(SCAN_ROW, i, 2); //For every pixel in the middle row
		int redPix = get_pixel(SCAN_ROW, i, 0); //For every pixel in the middle row


		if (redPix < redMin) { //Compare with min and max values and update min and max
			redMin = redPix;
		}
		if (redPix> redMax) {
			redMax = redPix;
		}
		if (bluePix < blueMin) { //Compare with min and max values and update min and max
			blueMin = bluePix;
		}
		if (bluePix> blueMax) {
			blueMax = bluePix;
		}
	}
	if (redMax > 225 &&
		blueMax < 140){
        printf("detect Q4 function returned 1\n");
		return 1;
	}
	else {
        //printf("detect Q4 function returned 0\n");
		return 0;
	}
}


//removed an earlier version of doWallMaze from here, check previous revisions for it

void doWallMaze() {
    int leftIR;
    int rightIR;
    int centerIR;
    int stage = 0;
    //int centerIR;

    float irCoef = 0.08;
    float rightCoef = 0.088;
    int rHandDistance = 400;

    V_INIT = 40;


    while (true) {
        //A0 = Left IR
        //A1 = Right IR
        //A2 = Center IR

        //"Frame"
        leftIR = read_analog(0);
        centerIR = read_analog(1);
        rightIR = read_analog(2);


        if (true){

            int lDiff = leftIR - rightIR;
            int rDiff = rightIR - leftIR;

            if (leftIR >= rightIR) {
                set_motor(RIGHT_MOTOR, V_INIT + lDiff * rightCoef);
                set_motor(LEFT_MOTOR, V_INIT);
                float speed_increase = lDiff * irCoef;
                printf("L Dist: %d | C Val: %d | R Dist: %d | Change: %.0f \n", leftIR, centerIR, rightIR,
                       speed_increase);
            } else if (leftIR <= rightIR) {
                set_motor(RIGHT_MOTOR, V_INIT);
                set_motor(LEFT_MOTOR, V_INIT + rDiff * irCoef);
                float speed_increase = rDiff * irCoef;
                printf("L Dist: %d | C Val: %d | R Dist: %d | Change: %.0f \n", leftIR, centerIR, rightIR,
                       speed_increase);

            }
			if(centerIR > 450){
				set_motor(LEFT_MOTOR, -V_INIT);
				set_motor(RIGHT_MOTOR, -V_INIT);

				sleep1(0,500000);

                set_motor(LEFT_MOTOR, V_INIT);
                set_motor(RIGHT_MOTOR, -V_INIT);

                sleep1(0,500000);

				stage = 1;

				printf("Yeet MACHIEN");
			}
            if (detectQuadFour()){
                stage = 1;
            }


        } else if (stage == 1){
                if (rightIR > rHandDistance){
                    printf("L Dist: %d | C Val: %d | R Dist: %d \n", leftIR, centerIR, rightIR);
                    set_motor(LEFT_MOTOR, V_INIT * rightIR/rHandDistance);
                    set_motor(RIGHT_MOTOR, V_INIT );
                } else if (rightIR < rHandDistance){
                    printf("L Dist: %d | C Val: %d | R Dist: %d \n", leftIR, centerIR, rightIR);
                    set_motor(LEFT_MOTOR, V_INIT * rightIR/rHandDistance);
                    set_motor(RIGHT_MOTOR, V_INIT );
                }
                if(centerIR > 500){
                    set_motor(LEFT_MOTOR, -V_INIT);
                    set_motor(RIGHT_MOTOR, V_INIT);
                }
        }
    }
}



/** calcMinMax
 *  Function that finds the min and max of a list.
 *  Assigns these values to variables to the global vars
 */
void calcMinMax(){
	for (int i = 0; i < PIC_WIDTH; i++){
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
	int current_error = 0;

	for (int i = 0; i < PIC_WIDTH; i++) { //For every pixel in the pixels array
		current_error += (i-PIC_MID) * list[i];
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
	float final_sig = prop_sig + integ_sig - deriv_sig; //Calculate the total signal by adding all the values to it

    //output
	printf("Line detected! \n");
	fprintf(f,"P: %f, I: %f, D: %f, F: %f\n", prop_sig, integ_sig, deriv_sig, final_sig);

	return final_sig;
}
