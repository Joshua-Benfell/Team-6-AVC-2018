#include <stdio.h>
#include <ctime>
#include <iostream>
#include "E101.h"


int main() {
    init();
    //Connection Address
    char serverAddr[15] = {"130.195.6.196"};

    //'password' for asking for password
    char pleaseArray[24] = {"Please"};
	//Array for storing the password
    char passwordArray[24] = {};

	//Successful connection returns 0
    if (connect_to_server(serverAddr, 1024) == 0){
        printf("Connection Established \n");
        send_to_server(pleaseArray);
        receive_from_server(passwordArray);
        send_to_server(passwordArray);
        printf("%s \n", passwordArray);
    }

    return 0;
}
