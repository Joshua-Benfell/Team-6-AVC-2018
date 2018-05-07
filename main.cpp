#include <stdio.h>
#include <time.h>
#include "E101.h"

//Motor Constants
const int motorLeft = 2;
const int motorRight = 1;

//Network Constants
const char server[15];
const int port;
const char request[6] = "Please";
char password[24];

//Function Declarations
void leftMotor(int seconds, int milliseconds);
void rightMotor(int seconds, int milliseconds);
void moveForwardTime(int seconds);
void moveForwardDistance(int centimeters);
double distanceToTime(int centimeters);
<<<<<<< HEAD
void followLine();

//Left motor is #1
//Right motor is #2
=======
int networkGate();
>>>>>>> b8981fd8b5634926dabc21a2bb5e7a36604da77a

int main(){
    init();

    followLine;

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

<<<<<<< HEAD
void moveForwardTime(int seconds){
    set_motor(1,255);
    set_motor(2,255);
    sleep1(seconds, 0);
    set_motor(1,0);
    set_motor(2,0);
}

void leftMotorTime(int seconds, int milliseconds){
    set_motor(1,255);
=======
void moveForwardTime(double time){
    int seconds = (int)floor(time);
    int milliseconds = (int)(time - seconds) * 1000000;
                                                //a million


    set_motor(motorLeft,255);
    set_motor(motorRight,255);
    sleep1(seconds, milliseconds);
    set_motor(1,0);
    set_motor(motorRight,0);
}

    void leftMotor(int seconds, int milliseconds){
    set_motor(motorLeft,255);
>>>>>>> b8981fd8b5634926dabc21a2bb5e7a36604da77a
    sleep1(seconds, milliseconds);
    set_motor(motorLeft,0);
}
<<<<<<< HEAD
void leftMotor(int amount, bool isForward){
    if( isForward == false){
        amount = amount * -1;
    }

    set_motor(1,amount);
}

void rightMotor(int amount, bool isForward){
    if( isForward == false){
        amount = amount * -1;
    }

    set_motor(2,amount);
}


void rightMotorTime(int seconds, int milliseconds) {
    set_motor(2, 255);
=======
    void rightMotor(int seconds, int milliseconds) {
    set_motor(motorRight, 255);
>>>>>>> b8981fd8b5634926dabc21a2bb5e7a36604da77a
    sleep1(seconds, milliseconds);
    set_motor(2, 0);
}

/* Function for connecting to the provided network to open the network gate
 * connects to the network, sends a string receives and string.
 * returns true if exchange was successful
 */

 int networkGate(){
    int connetion = connect_to_server(server, port);
    int send = send_to_server(request);
    int received = receive_from_server(password);
    //Deal with null termination

    int sendAgain = send_to_server(password);
 }
