#include <stdio.h>
#include <time.h>
#include "E101.h"
//#include <math.h>
#include <stdlib.h>
//#include <ctype.h>

#define CAMERA_WIDTH 320 //Control Resolution from Camera
#define CAMERA_HEIGHT 240 //Control Resolution from Camera
#define DEBUG 0 //testing mode = 1 displays debug messages

int main()
{
	init();
	//initialise Vars
	int v_go = 50;
	int err = 0;
	int prev_err = 0;
	float kp = 0.003;
	float kd = 0.001;
	//int leftpix = 0;
	//int rightpix = 0;

	//while(1)
	//{
	//char fileName[5] = "pic1";

	while(1){

		take_picture();
	//save_picture(fileName);

	//scanf("%s", fileName);
		int scan_row = 120;
	//for (int i = 0; i < 320;i++)
	//{
		//int pix = get_pixel(scan_row,i,3);
		//printf("i=%d col=%d\n",i,pix);
	//}

    	int max = 0;
    	int min =255;
   		for (int i = 0; i <320;i++){
			int pix = get_pixel(scan_row,i,3);
        	if ( pix > max) {
				max = pix;
			}
			if (pix < min){
				min =pix;
			}
    	}
    	int thr = (max+min)/2;
		if(DEBUG){
	    	printf(" min=%d max=%d threshold=%d\n", min, max,thr);
		}
    	int whi[320];  // white pixels
     	int np = 0;
    	for (int i = 0; i <320;i++){
			whi[i]= 0 ;
			int pix = get_pixel(scan_row,i,3);
			if ( pix > thr){
				whi[i] = 1;
				np = np +1;
			//leftpix = leftpix+whi[i];

			}
			//whi[i]= 0 ;
			//int pix = get_pixel(scan_row,i,3);
			//if ( pix > thr)
			///{
			//	whi[i] = 1;
			//	rightpix = rightpix+whi[i];
			//}

		//err = leftpix - rightpix;
		}
    	err = 0;

    	for (int i = 0; i <320;i++){
		//printf("%d ",whi[i]);
			err = err + whi[i]*(i-160);
     	}
    //double e = (double) (((double) err)/320.0);
		if(DEBUG){
	    	printf("err=%d np = %d e=%f\n",err,np,(float) err/(float) np);
			//printf("%d \n",whi);
			display_picture(1,0);
		}
		int derivative = (int) (float)(err-prev_err) * kd;
		prev_err = err;
    	int dv = (int)((float)err * kp);
    	int v_left = v_go + dv + derivative;
    	int v_right = v_go - dv - derivative;
		if(DEBUG){
	    	printf("v_left = %d v_right=%d\n dv = %d",v_left,v_right,dv);
		}
    	set_motor(2, v_left);
    	set_motor(1, v_right);

    }
	return 0;
}
