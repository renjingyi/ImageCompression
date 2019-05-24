/*
 * mean_absolute_error.c
 *
 *  Created on: Feb 28, 2018
 *      Author: jeann
 */

#include "libpnm.h"
#include "mean_absolute_error.h"
#include<stdio.h>

float mean_absolute_error(char *file_name_1_ptr, char *file_name_2_ptr){
	int i,j;
	float absolute_error = 0;
	float mean_absolute_error = 0;
	struct PGM_Image image1, image2;
	//load these two PGM image
	load_PGM_Image(&image1, file_name_1_ptr);
	load_PGM_Image(&image2, file_name_2_ptr);

	//check two images' sizes
	if(image1.height != image2.height || image1.width != image2.width)
		return -1;
	else{
		//check two images' max_gray_value
		//if they are not equal, then make them equal
		if(image1.maxGrayValue != image2.maxGrayValue){
			double scale;
			if(image1.maxGrayValue > image2.maxGrayValue){
				scale = image1.maxGrayValue / image2.maxGrayValue;
				//scale the small one to get the same gray value range
				for(i = 0; i < image2.height; i++){
					for(j = 0; j < image2.width; j++){
						image2.image[i][j] *= scale;
					}
				}
			}
			else{
				scale = image2.maxGrayValue / image1.maxGrayValue;
				//scale the small one to get the same gray value range
				for(i = 0; i < image1.height; i++){
					for(j = 0; j < image1.width; j++){
						image1.image[i][j] *= scale;
					}
				}
			}
		}
		//compute mean absolute error
		for(i = 0; i < image1.height; i++){
			for(j = 0; j < image1.width; j++){
				absolute_error += abs(image1.image[i][j] - image2.image[i][j]);
			}
		}
		mean_absolute_error = absolute_error / (image1.height * image1.width);

		return mean_absolute_error;
	}

}
