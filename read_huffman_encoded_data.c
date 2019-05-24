/*
 * read_huffman_encoded_data.c
 *
 *  Created on: Feb 28, 2018
 *      Author: jeann
 */

#include "libpnm.h"
#include<stdio.h>
#include "read_huffman_encoded_data.h"

unsigned char *read_huffman_encoded_data(char *compressed_file_name_ptr, int *image_width, int *image_height, int
*max_gray_value, int *number_of_nodes, struct node **huffman_node, long int *length_of_encoded_image){
	//state a file pointer
	FILE *fptr;
	//open the file ready to read
	fptr = fopen(compressed_file_name_ptr, "r+");
	//read image width, height, max_gray_value
	fscanf(fptr, "%d %d %d", image_width, image_height, max_gray_value);
	//read number_of_nodes
	fscanf(fptr, "%d", number_of_nodes);
	//read huffman_node
	fread(*huffman_node, sizeof(**huffman_node), *number_of_nodes, fptr);
	//read length_of_encoded_image
	fscanf(fptr, "%ld", length_of_encoded_image);

	unsigned char encoded[*length_of_encoded_image];
	unsigned char *encoded_image = encoded;
	//read encoded_image from file
	fread(encoded, sizeof(encoded), *length_of_encoded_image, fptr);

	return encoded_image;
}
