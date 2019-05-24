/*
 * read_huffman_encoded_data.h
 *
 *  Created on: Feb 28, 2018
 *      Author: jeann
 */

#ifndef READ_HUFFMAN_ENCODED_DATA_H_
#define READ_HUFFMAN_ENCODED_DATA_H_

struct node{
	int first_value;
	int second_value;
};
unsigned char *read_huffman_encoded_data(char *compressed_file_name_ptr, int *image_width, int *image_height, int
*max_gray_value, int *number_of_nodes, struct node **huffman_node, long int *length_of_encoded_image);

#endif /* READ_HUFFMAN_ENCODED_DATA_H_ */
