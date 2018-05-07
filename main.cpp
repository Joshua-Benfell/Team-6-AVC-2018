#include <stdio.h>
#include <time.h>
#include "E101.h"
#include <math.h>

//Motor Constants
const int motorLeft = 2;
const int motorRight = 1;

//Network Constants
char server[15] = "127.0.0.1"; //Change this once we get the IP Address
int port = 6667; //Change this once we get the port
char request[24] = "Please";
char password[24];

//Function Declarations
void leftMotor(int seconds, int milliseconds);
void rightMotor(int seconds, int milliseconds);
void moveForwardTime(int seconds);
void moveForwardDistance(int centimeters);
double distanceToTime(int centimeters);
void followLine();
//int networkGate();


int main(){
    init();

    followLine();

    return 0;}

void moveForwardDistance(int centimeters){
    moveForwardTime(distanceToTime(centimeters));
}

void followLine(){
    //Setup for algorithm
    double averageHigh;
    double averageLow;
    double overallAverage = 0;

    int ROW = 120;
    int COLUMNS = 320;
    int SPEED = 75;
    int speedLeft = 75;
    int speedRight = 75;

    //Data from row chosen stored in this array
    int pixelData[320];

    //Values of pixels determined to be "high"
    int highPixels[320];
    //Values of pixels determined to be "low"
    int lowPixels[320];

    //index of pixels determinted to be "high"
    int avgHighIndex[320];
    //index of pixels determinted to be "low"
    int avgLowIndex[320];

    int weightedSum = 0;
    int motorDifference = 0;

    leftMotor(SPEED, true);
    rightMotor(SPEED, true);

    for(int l = 0; l > 200; l++){
        take_picture();

        //Stores Data from image in array
        for(int i = 0; i < COLUMNS; i++){
            pixelData[i] = get_pixel(ROW, i, 3);

            overallAverage = overallAverage + pixelData[i];
        }
        overallAverage = overallAverage / COLUMNS;

        for (int a = 0; a < sizeof(overallAverage); a++){
            if (pixelData[a] > overallAverage) {
                weightedSum = 1 * (a - 120);
            }

        }
        motorDifference = weightedSum / 30;
        printf("MD: %d", motorDifference);
        leftMotor(SPEED - motorDifference, true);
        rightMotor(SPEED + motorDifference, true);
        sleep1(1,0);

        break;
    }

}


double distanceToTime(int centimeters){
    double wheelRadius = 80;//Measured in millimeters
    double revolutionsPerSecond = 1;
    double pi = 3.1415;
    double velocity = (2 * pi) * revolutionsPerSecond;
    double time = velocity/centimeters;
    return time;
}

void moveForwardTime(int seconds){
    set_motor(motorLeft,255);
    set_motor(motorRight,255);
    sleep1(seconds, 0);
    set_motor(motorLeft,0);
    set_motor(motorRight,0);
}

void leftMotorTime(int seconds, int milliseconds){
    set_motor(motorLeft,255);
}

void moveForwardTime(double time){
    int seconds = (int)floor(time);
    int milliseconds = (int)(time - seconds) * 1000000; //a million
    set_motor(motorLeft,255);
    set_motor(motorRight,255);
    sleep1(seconds, milliseconds);
    set_motor(motorLeft,0);
    set_motor(motorRight,0);
}

    void leftMotor(int seconds, int milliseconds){
    set_motor(motorLeft,255);

    sleep1(seconds, milliseconds);
    set_motor(motorLeft,0);
}

void leftMotor(int amount, bool isForward){
    if( isForward == false){
        amount = amount * -1;
    }

    set_motor(motorLeft,amount);
}

void rightMotor(int amount, bool isForward){
    if( isForward == false){
        amount = amount * -1;
    }

    set_motor(motorRight,amount);
}


void rightMotorTime(int seconds, int milliseconds) {
    set_motor(2, 255);
}

void rightMotor(int seconds, int milliseconds) {
    set_motor(motorRight, 255);
    sleep1(seconds, milliseconds);
    set_motor(motorRight, 0);
}

/* Function for connecting to the provided network to open the network gate
 * connects to the network, sends a string receives and string.
 * returns true if exchange was successful
 */

 int networkGate(){
    if(connect_to_server(server, port) == 0){
        send_to_server(request);
        receive_from_server(password);
        printf("%s\n", password);
        send_to_server(password);
        return -1;
    }
    return 0;
}
