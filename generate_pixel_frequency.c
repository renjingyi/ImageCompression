/*
 * generate_pixel_frequency.c
 *
 *  Created on: Feb 19, 2018
 *      Author: jeann
 */

#include "libpnm.h"
#include<malloc.h>
#include "generate_pixel_frequency.h"
#define max_gray_value 255

long int *generate_pixel_frequency(struct PGM_Image *input_pgm_image,
		int *number_of_non_zero_values_in_the_frequency_array){
	int i,j;
	//state count variable to count how many symbols in frequency array
	int count = 0;
	//state value variable to temporarily save each pixel gray value
	int value = -1;
	//initial number_of_non_zero_values_in_the_frequency_array
	*number_of_non_zero_values_in_the_frequency_array = 0;
	//dynamically allocate memory to frequency array
	long int *frequency_array;
	frequency_array = (long int *)malloc((max_gray_value + 1) * sizeof(long int));
	//initial frequency_array
	for(i = 0; i < max_gray_value + 1; i++){
		frequency_array[i] = 0;
	}
	//find the gray value of each pixel and add 1 to its corresponding frequency
	//get the number_of_non_zero_values_in_the_frequency_array
	for(i = 0; i < input_pgm_image->height; i++){
		for(j = 0; j < input_pgm_image->width; j++){
			value = input_pgm_image->image[i][j];
			frequency_array[value] += 1;
		}
	}
	//get *number_of_non_zero_values_in_the_frequency_array which is the number of symbols
	for(i = 0; i < max_gray_value + 1; i++){
		if(frequency_array[i] != 0)
			count += 1;
	}

	*number_of_non_zero_values_in_the_frequency_array = count;
	return frequency_array;
}



