/*
 * DPCM_encoding.c
 *
 *  Created on: Mar 30, 2018
 *      Author: jeann
 */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include "libpnm.h"
#include "DPCM_encoding_function.h"

int main(int argc, char* argv[]){
	//check the input validation
	if(argc != 3){
		printf("Invalid input: wrong input file number.\n");
		return 0;
	}
	else{
		if(access(argv[1], F_OK) == 0){
			if(*argv[2] == '1' || *argv[2] == '2' || *argv[2] == '3' || *argv[2] == '4'){
				//declare the average and the standard deviation of the absolute prediction errors for PGM image
				float avg = 0, std = 0;
				//change input prediction rule's data type from char to int
				int prediction_rule = (int)strtol(argv[2], NULL, 10);
				//call DPCM_encoding_function.c to encode image
				//we will get one output .txt file to be sent to decoder, one .csv file to do analysis
				//get the running time
				clock_t time;
				time = clock();
				Encode_Using_DPCM(argv[1], prediction_rule, &avg, &std);
				time = clock() - time;
				double compression_time = (double)time / CLOCKS_PER_SEC;
				//get the values of avg and std
				printf("avg is %.2f\n", avg);
				printf("std is %.2f\n", std);
				printf("compression time is %f\n", compression_time);
				return 0;
			}
			else{
				printf("Invalid input: wrong input prediction rule mode.\n");
				return 0;
			}
		}
		else{
			printf("Invalid input: wrong input file name.\n");
			return 0;
		}
	}
}
