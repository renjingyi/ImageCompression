/*
 * huffman_decode_image.c
 *
 *  Created on: Mar 1, 2018
 *      Author: jeann
 */

#include "libpnm.h"
#include<stdio.h>
#include "huffman_decode_image.h"
#include<malloc.h>
#define ROOT -1
#define INTERNAL -2
#define LEFT 0
#define RIGHT 1

struct PGM_Image *huffman_decode_image(int image_width, int image_height, int max_gray_value,
		int number_of_nodes, struct node *huffman_node, long int length_of_encoded_image, unsigned char *encoded_image){
	int i, p = 0, q = 0;
	int byte_check = 0;
	int code_check;
	unsigned char temp_byte;
	unsigned char temp_bit;
	struct PGM_Image pgm_decode, *decoded_image;
	decoded_image = &pgm_decode;
	create_PGM_Image(&pgm_decode, image_width, image_height, max_gray_value);

	//before building the whole huffman tree,
	//we need to put the values of the huffman_node array's last index into huffman tree
	//state a huffman tree and allocate memory to it
	Tree *huffman;
	huffman = (Tree *)malloc(sizeof(Tree));
	//make ROOT the value of huffman structure
	huffman->value = ROOT;
	//use two other Tree structures to store the last huffman_node values in huffman tree
	Tree *huffman_level1_L, *huffman_level1_R;
	huffman_level1_L = (Tree *)malloc(sizeof(Tree));
	huffman_level1_R = (Tree *)malloc(sizeof(Tree));
	huffman_level1_L->value = huffman_node[number_of_nodes - 1].first_value;
	huffman_level1_R->value = huffman_node[number_of_nodes - 1].second_value;
	huffman_level1_L->left = huffman_level1_L->right = NULL;
	huffman_level1_R->left = huffman_level1_R->right = NULL;
	huffman->left = huffman_level1_L;
	huffman->right = huffman_level1_R;

	//build huffman tree
	for(i = number_of_nodes - 2; i >= 0; i--){
		//call the function
		insert(huffman, huffman_node[i]);
	}

	//decode the encoded_image
	for(i = 0; i < length_of_encoded_image; i++){
		//make sure the decoding image is not out of range
		if(p < pgm_decode.height){
			//get the current byte needed to decode
			temp_byte = encoded_image[i];
			Tree *temp;
			temp = huffman;

			//keep checking until we finish checking all 8 bits in one byte
			NEXT_CHECK: while(byte_check < 8){
				//get the MSB of current byte
				temp_bit = (temp_byte ^ 0) >> 7;
				//left shift current byte to get the new byte in order to get next temp_bit
				temp_byte <<= 1;
				//due to checking one bit, increase byte_check by 1
				byte_check++;
				//using find() function to find value in huffman tree
				//0 means the left value, 1 means the right value
				code_check = find(temp, (int)temp_bit);
				//if code_check >= 0, which means we find the leaf, the pixel gray value
				if(code_check >= 0){
					for(; q < pgm_decode.width;){
						//save this gray value into decoded image
						pgm_decode.image[p][q] = code_check;
						temp = huffman;
						//make sure q is not out of image width range
						if(q == pgm_decode.width - 1){
							q = -1;
							p++;
						}
						q++;
						goto NEXT_CHECK;
					}
				}
				//if code_check < 0, which means we are still in the internal
				else{
					//then we need to change the current tree's range
					if(temp_bit == 0)
						temp = temp->left;
					else
						temp = temp->right;
				}
			}
			//reset byte_check to 0 for the new byte
			byte_check = 0;
		}
		//if p is bigger than the image height range, then stop jumping in and jump out the loop
		else
			break;
	}

	return decoded_image;
}

int find(Tree *huffman, int code){
	Tree *temp;
	if(code == 0)
		temp = huffman->left;
	else
		temp = huffman->right;
	return temp->value;
}

void insert(Tree *huffman, struct node node_insert){
	//when huffman is NULL, stop and return to the last iteration
	while(huffman != NULL){
		//when finding the node value is what we want,
		//change the node to be the internal node and add two new nodes to store the huffman_node pair
		if(huffman->value == node_insert.first_value){
			huffman->value = INTERNAL;
			Tree *temp_l = (Tree *)malloc(sizeof(Tree));
			Tree *temp_r = (Tree *)malloc(sizeof(Tree));
			temp_l->value = node_insert.first_value;
			temp_r->value = node_insert.second_value;
			temp_l->left = temp_l->right = NULL;
			temp_r->left = temp_r->right = NULL;
			huffman->left = temp_l;
			huffman->right = temp_r;
			return;
		}
		//from left to right, using recursions to find the node value that we want
		else{
			insert(huffman->left, node_insert);
			insert(huffman->right, node_insert);
			return;
	    }
	}
}
