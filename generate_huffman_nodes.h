/*
 * generate_huffman_nodes.h
 *
 *  Created on: Feb 27, 2018
 *      Author: jeann
 */

#ifndef GENERATE_HUFFMAN_NODES_H_
#define GENERATE_HUFFMAN_NODES_H_

struct node{
	int first_value;
	int second_value;
};
struct node *generate_huffman_nodes(long int *pixel_frequency, int max_gray_value, int number_of_non_zero_values_in_the_frequency_array);

#endif /* GENERATE_HUFFMAN_NODES_H_ */
