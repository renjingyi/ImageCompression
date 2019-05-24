/*
 * store_huffman_encoded_data.h
 *
 *  Created on: Feb 28, 2018
 *      Author: jeann
 */

#ifndef STORE_HUFFMAN_ENCODED_DATA_H_
#define STORE_HUFFMAN_ENCODED_DATA_H_
#include "generate_huffman_nodes.h"
#include "huffman_encode_image.h"

void store_huffman_encoded_data(char *compressed_file_name_ptr, int image_width, int image_height, int
max_gray_value, int number_of_nodes, struct node *huffman_node, long int length_of_encoded_image, unsigned char
*encoded_image);

#endif /* STORE_HUFFMAN_ENCODED_DATA_H_ */
