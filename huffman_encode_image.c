/*
 * huffman_encode_image.c
 *
 *  Created on: Feb 24, 2018
 *      Author: jeann
 */

#include "libpnm.h"
#include<stdio.h>
#include<string.h>
#include "huffman_encode_image.h"

unsigned char *huffman_encode_image(struct PGM_Image *input_pgm_image, struct node *huffman_node,
		int number_of_nodes, unsigned long int *length_of_encoded_array){
	int i, j, k, m, p;
	//there should be (number_of_nodes - 1) bits for one huffman code at most
	char *temp;
	temp = (char *)malloc((number_of_nodes - 1) * sizeof(char));
	for(i = 0; i < number_of_nodes - 1; i++){
		temp[i] = '\0';
	}
	unsigned char temp_byte = '\0';
	unsigned char temp_bit = '\0';
	int code_length = 0, supplementary_move = 0;
	int steps;
	//use check_byte to check whether this byte is finished. -1 means not finished while 0 means finished.
	int check_byte = -1;
	//find how many encoded pixel value when using huffman could fit in one byte
	int num = 8 / (number_of_nodes - 1);
	//then the number of encoded array length should be the whole number of pixels in image
	//      divide by the least number of pixels encoded in one byte
	*length_of_encoded_array = (input_pgm_image->height * input_pgm_image->width) / num;
	//state and initial encoded image array
	unsigned char *encoded_image;
	encoded_image = (unsigned char *)malloc((*length_of_encoded_array) * sizeof(unsigned char));
	for(i = 0; i < *length_of_encoded_array; i++){
		encoded_image[i] = '\0';
	}
	//state and initial huffman_code array
	//each element in array has its gray value and its generated huffman code
	struct code huffman_code[number_of_nodes + 1];
	for(i = 0; i < number_of_nodes + 1; i++){
		huffman_code[i].symbol = -1;
		strcpy(huffman_code[i].huffCode, "");
	}

	//generate a huffman code array where gives each symbol a binary huffman code
	//according to huffman_node generate huffman_code
	/*according to huffman encoding algorithm, I find two rules that
	 *  1: next node's first value must in the current codes' symbol list;
	 *  2: next node's second value must be a new symbol value in the codes' symbol list */
	/*Which means that:
	 *     once assign the first huffman code using the last node values in the array,
	 *     when dealing with the next node i's values
	 * ~1: i.fisrt_value must be found in the code array;
	 * ~2: i.second_value must be added in the code array */
	huffman_code[0].symbol = huffman_node[number_of_nodes - 1].first_value;
	strcpy(huffman_code[0].huffCode, "0");
	huffman_code[1].symbol = huffman_node[number_of_nodes - 1].second_value;
	strcpy(huffman_code[1].huffCode, "1");
	for(i = number_of_nodes - 2; i >= 0; i--){
		for(j = 0; j < number_of_nodes + 1; j++){
			if(huffman_node[i].first_value == huffman_code[j].symbol){
				strcpy(temp, huffman_code[j].huffCode);
				strcat(huffman_code[j].huffCode, "0");
				for(k = 0; k < number_of_nodes + 1; k++){
					if(huffman_code[k].symbol == -1){
						huffman_code[k].symbol = huffman_node[i].second_value;
						strcat(huffman_code[k].huffCode, strcat(temp, "1"));
						break;
					}
				}
				break;
			}
		}
	}

	//search all the pixel in image, change pixel's gray value to huffman code
	m = 0;
	for(i = 0; i < input_pgm_image->height; i++){
		for(j = 0; j < input_pgm_image->width; j++){
			while(m < *length_of_encoded_array){
				//using check_byte to check whether temp_byte is full filled and ready to store in encoded_array
				if(check_byte == -1){
					for(k = 0; k < number_of_nodes + 1; k++){
						//find current pixel gray value's huffman code in huffman_code array
						if(input_pgm_image->image[i][j] == huffman_code[k].symbol){
							//get current bit length in one byte, if after added the next pixel's huffman code
							//    code_length is larger than 8, we should give up encoding the next pixel and store the previous
							code_length += strlen(huffman_code[k].huffCode);
							if(code_length <= 8){
								for(p = 0; p < strlen(huffman_code[k].huffCode); p++){
									temp_bit = huffman_code[k].huffCode[p] - 48;
									steps = 8 - code_length + strlen(huffman_code[k].huffCode) - p - 1;
									temp_bit <<= steps;
									temp_byte ^= temp_bit;
								}
							}
							else{
								//if it is larger than 8, then find the steps we should move and left shift the byte
								supplementary_move = strlen(huffman_code[k].huffCode) - (code_length - 8);
								temp_byte <<= supplementary_move;
								check_byte = 0;
							}
							break;
						}
					}
				}
				else{
					//if check_byte = 0 means we finish editing the current byte, then we move to next byte in array
					encoded_image[m] = temp_byte;
					m++;
					temp_byte = 0;
					code_length = 0;
					check_byte = -1;
					j -= 2;
				}
				break;
			}
		}
	}

	*length_of_encoded_array = m + 1;

	return encoded_image;
}


