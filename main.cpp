#include <stdio.h>
#include <time.h>
#include "E101.h"
#include <math.h>

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
int networkGate();

int main(){
    init();
    for (int x = 0; x < 5; x++){
        moveForwardDistance(10);

        /*set_motor(1,255);
        sleep1(1,0);
        set_motor(1,0);
        sleep1(1,0);*/

    }
    return 0;}

void moveForwardDistance(int centimeters){
    moveForwardTime(distanceToTime(centimeters));
}


double distanceToTime(int centimeters){
    double wheelRadius = 80;//Measured in millimeters
    double revolutionsPerSecond = 1;
    double pi = 3.1415;
    double velocity = (2 * pi * wheelRadius) * revolutionsPerSecond;
    double time = velocity/centimeters;
    return time;
}

void moveForwardTime(double time){
    int seconds = (int)floor(time);
    int milliseconds = (int)(time - seconds) * 1000000;
                                                //a million


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
    int connetion = connect_to_server(server, port);
    int send = send_to_server(request);
    int received = receive_from_server(password);
    //Deal with null termination
 }
