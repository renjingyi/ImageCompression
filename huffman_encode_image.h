/*
 * huffman_encode_image.h
 *
 *  Created on: Feb 27, 2018
 *      Author: jeann
 */

#ifndef HUFFMAN_ENCODE_IMAGE_H_
#define HUFFMAN_ENCODE_IMAGE_H_
#include "generate_huffman_nodes.h"

struct code{
	int symbol;
	char huffCode[8];
};
unsigned char *huffman_encode_image(struct PGM_Image *input_pgm_image, struct node *huffman_node, int number_of_nodes, unsigned long int *length_of_encoded_array);

#endif /* HUFFMAN_ENCODE_IMAGE_H_ */
