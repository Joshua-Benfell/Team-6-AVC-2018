#include <stdio.h>
#include <ctime>
#include <iostream>
#include "E101.h"


int main() {
    init();
    /*char serverAddr[15] = {"1","3","0",
                      ".","1","9","5",
                      ".","6",".",
                      "1","9","6"};*/
    char serverAddr[15] = {"130.195.6.196"};

    //char pleaseArray[24] = {"p","l","e","a","s","e"};
    char pleaseArray[24] = {"Please"};
    char passwordArray[24] = {};


    if (connect_to_server(serverAddr, 1024) == 0){
        printf("Connection Established \n");
        send_to_server(pleaseArray);
        receive_from_server(passwordArray);
        send_to_server(passwordArray);
        printf("%s \n", passwordArray);
    }

    return 0;
}
