/*
 * compare_pgm_images.c
 *
 *  Created on: Mar 2, 2018
 *      Author: jeann
 */

#include "mean_absolute_error.h"
#include<stdio.h>
#include<unistd.h>

int main(int argc, char* argv[]){
	//check input validation
	if(argc != 3){
		printf("Invalid input: wrong input file number.\n");
		return 0;
	}
	else{
		if(access(argv[1], F_OK) == 0 && access(argv[2], F_OK) == 0){
			//compare two images
			float error;
			error = mean_absolute_error(argv[1], argv[2]);
			if(error == -1)
				printf("Compare Fail: different image sizes.\n");
			else
				printf("error is %.2f", error);
			return 0;
		}
		else{
			printf("Invalid input: wrong input file name.\n");
			return 0;
		}
	}
}
