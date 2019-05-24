/*
 * huffman_decode_image.h
 *
 *  Created on: Mar 1, 2018
 *      Author: jeann
 */

#ifndef HUFFMAN_DECODE_IMAGE_H_
#define HUFFMAN_DECODE_IMAGE_H_
#include "read_huffman_encoded_data.h"

struct huffman_tree{
	int value;
	struct huffman_tree *left;
	struct huffman_tree *right;
};
typedef struct huffman_tree Tree;

int find(Tree *huffman, int code);

void insert(Tree *huffman, struct node node_insert);

struct PGM_Image *huffman_decode_image( int image_width, int image_height, int max_gray_value, int
number_of_nodes, struct node *huffman_node, long int length_of_encoded_image, unsigned char *encoded_image);

#endif /* HUFFMAN_DECODE_IMAGE_H_ */
