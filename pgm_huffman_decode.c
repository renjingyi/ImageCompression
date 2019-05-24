/*
 * pgm_huffman_decode.c
 *
 *  Created on: Mar 2, 2018
 *      Author: jeann
 */

#include "libpnm.h"
#include "read_huffman_encoded_data.h"
#include "huffman_decode_image.h"
#include<stdio.h>

int main(int argc, char* argv[]){
	int *width = 0, *height = 0, *max_gray_value = 0;
	int *number_of_nodes = 0;
	struct node *huffman_nodes;
	long int *length_of_encoded_image = 0;
	unsigned char *encoded_image;
	//get encoded values from file
	encoded_image = read_huffman_encoded_data(argv[1], width, height, max_gray_value,
			number_of_nodes, &huffman_nodes, length_of_encoded_image);

	//decode the array and output image
	struct PGM_Image *pgm_decode;
	pgm_decode = huffman_decode_image(*width, *height, *max_gray_value, *number_of_nodes, huffman_nodes,
			*length_of_encoded_image, encoded_image);
	save_PGM_Image(pgm_decode, argv[2], true);

	return 0;
}
