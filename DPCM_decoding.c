/*
 * DPCM_decoding.c
 *
 *  Created on: Mar 30, 2018
 *      Author: jeann
 */

#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<time.h>
#include "libpnm.h"
#include "DPCM_decoding_function.h"

int main(int argc, char* argv[]){
	//check input validation
	if(argc != 2){
		printf("Invalid input: wrong input file number.\n");
		return 0;
	}
	else{
		//add extension to input
		char name[50] = "";
		strcat(name, argv[1]);
		strcat(name, ".txt");
		if(access(name, F_OK) == 0){
			//get the received compressed filename from command
			//call the Decode_Using_DPCM() to generate a new PGM image
			//get the running time
			clock_t time;
			time = clock();
			Decode_Using_DPCM(name);
			time = clock() - time;
			double decompression_time = (double)time / CLOCKS_PER_SEC;
			printf("decompression time is %f\n", decompression_time);
			return 0;
		}
		else{
			printf("Invalid input: wrong input file name.\n");
			return 0;
		}
	}
}
