#include "E101.h"
#include <stdio.h>

const int RIGHT_MOTOR = 2;
const int LEFT_MOTOR =1;

const float V_INIT = 80; 

void doWallMaze();

int main(){
	init();
	doWallMaze();	
	return 0;
}

void doWallMaze(){
    int leftIR;
    int rightIR;
    //int centerIR;

while(true){
        //A0 = Front left IR
        //A1 = Front right IR
        //A2 = Front center IR
	
	float irCoef = 0.15;
        //"Frame"
        leftIR = 1024 - read_analog(0);
        rightIR = 1024 -  read_analog(1);
        //centerIR = 1024 - read_analog(2);
        int lDiff = leftIR - rightIR;
	int rDiff = rightIR - leftIR;
		
        if (leftIR >= rightIR){
            set_motor(RIGHT_MOTOR, V_INIT + lDiff * irCoef);
            set_motor(LEFT_MOTOR, V_INIT);
	    float speed_increase = lDiff * irCoef;
            printf("Left Dist %d \n Speed Increase %f \n \n,", lDiff, speed_increase);
            sleep1(1,0);
        } else if (leftIR <= rightIR){
            set_motor(RIGHT_MOTOR,V_INIT);
            set_motor(LEFT_MOTOR, V_INIT + rDiff * irCoef);
            float speed_increase = rDiff * irCoef;
            printf("Right Dist %d \n Speed Increase %f \n \n,", rDiff, speed_increase);
		sleep1(1,0);
        }

    }
}
