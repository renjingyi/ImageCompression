/*
 * generate_huffman_nodes.c
 *
 *  Created on: Feb 22, 2018
 *      Author: jeann
 */

#include "libpnm.h"
#include<stdio.h>
#include<limits.h>
#include<malloc.h>
#include "generate_huffman_nodes.h"


struct node *generate_huffman_nodes(long int *pixel_frequency, int max_gray_value, int number_of_non_zero_values_in_the_frequency_array){
	int i, j;
	int min1_index, min2_index;
	long int min1, min2;
	//state a structure array and its pointer
	struct node *huffman_nodes;
	huffman_nodes = (struct node *)malloc((number_of_non_zero_values_in_the_frequency_array - 1) * sizeof(struct node));
	//initial huffman_nodes
	for(i = 0; i < number_of_non_zero_values_in_the_frequency_array - 1; i++){
		huffman_nodes[i].first_value = -1;
		huffman_nodes[i].second_value = -1;
	}
	//state and initial a 2-D array to store non zero values in pixel frequency with their frequencies and pixel values
	long int non_zero_values_pixel_frequency[number_of_non_zero_values_in_the_frequency_array][2];
	for(i = 0; i < number_of_non_zero_values_in_the_frequency_array; i++){
		non_zero_values_pixel_frequency[i][0] = 0;
		non_zero_values_pixel_frequency[i][1] = 0;
	}

	//get non zero values from pixel frequency
	for(j = 0; j < number_of_non_zero_values_in_the_frequency_array;){
		for(i = 0; i < max_gray_value + 1; i++){
			if(pixel_frequency[i] != 0){
				non_zero_values_pixel_frequency[j][0] = pixel_frequency[i];
			    non_zero_values_pixel_frequency[j][1] = i;
			    j++;
			}
		}
	}

	//Huffman algorithm
	for(i = 0; i < number_of_non_zero_values_in_the_frequency_array - 1; i++){
		//let first two values to be the smallest two in the array for now
		min1 = non_zero_values_pixel_frequency[0][0];
		min2 = non_zero_values_pixel_frequency[1][0];
		//get the current two smallest values' index
		min1_index = 0;
		min2_index = 1;
		//start comparing from the third value in frequency array
		for(j = 2; j < number_of_non_zero_values_in_the_frequency_array; j++){
			//according to the order of current two smallest values, get the new two smallest values in array
			if(min1 < min2){
				if(non_zero_values_pixel_frequency[j][0] < min2){
					min2 = non_zero_values_pixel_frequency[j][0];
					min2_index = j;
				}
			}
			else if(min1 > min2){
				if(non_zero_values_pixel_frequency[j][0] < min1){
					min1 = min2;
					min1_index = min2_index;
					min2 = non_zero_values_pixel_frequency[j][0];
					min2_index = j;
				}
			}
			else{
				if(non_zero_values_pixel_frequency[j][0] < min2){
					min2 = non_zero_values_pixel_frequency[j][0];
					min2_index = j;
				}
			}
		}
		//after finding the smallest two values and their index in the frequency array
		//get the node
		huffman_nodes[i].first_value = non_zero_values_pixel_frequency[min1_index][1];
		huffman_nodes[i].second_value = non_zero_values_pixel_frequency[min2_index][1];
		//sum up to one value and modify another in the frequency array
		non_zero_values_pixel_frequency[min1_index][0] = non_zero_values_pixel_frequency[min1_index][0] + non_zero_values_pixel_frequency[min2_index][0];
		non_zero_values_pixel_frequency[min2_index][0] = LONG_MAX;
	}

	return huffman_nodes;
}
