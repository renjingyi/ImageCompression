/*
 * store_huffman_encoded_data.c
 *
 *  Created on: Feb 28, 2018
 *      Author: jeann
 */

#include "libpnm.h"
#include<stdio.h>
#include "store_huffman_encoded_data.h"

void store_huffman_encoded_data(char *compressed_file_name_ptr, int image_width, int image_height, int
max_gray_value, int number_of_nodes, struct node *huffman_node, long int length_of_encoded_image, unsigned char
*encoded_image){
	//state a file pointer
	FILE *fp;
	//open the file with the given name
	fp = fopen(compressed_file_name_ptr, "w+");
	//write in image width height and max_gray_value
	fprintf(fp, "%d %d %d ", image_width, image_height, max_gray_value);
	//write in number_of_nodes
	fprintf(fp, "%d ", number_of_nodes);
	//write in structure array
	fwrite(huffman_node, sizeof(huffman_node), number_of_nodes, fp);
	//write in length_of_encoded_image
	fprintf(fp, "%ld ", length_of_encoded_image);
	//write in encoded_image
	fwrite(encoded_image, sizeof(encoded_image), length_of_encoded_image, fp);
	//close file
	fclose(fp);
}

