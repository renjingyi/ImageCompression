/*
 * pgm_huffman_encode.c
 *
 *  Created on: Mar 2, 2018
 *      Author: jeann
 */

#include "libpnm.h"
#include "generate_pixel_frequency.h"
#include "generate_huffman_nodes.h"
#include "huffman_encode_image.h"
#include "store_huffman_encoded_data.h"
#include<stdio.h>

int main(int argc, char* argv[]){
	struct PGM_Image pgm;
	//get file name from command line to load original PGM image
	load_PGM_Image(&pgm, argv[1]);

	//get the image pixel frequency array by using generate_pixel_frequency.c
	long int *pixelFrequency;
	int symbolNums = -1;
	pixelFrequency = generate_pixel_frequency(&pgm, &symbolNums);

	//get huffman nodes array by using generate_huffman_nodes.c
	struct node *huffmanNodes;
	huffmanNodes = generate_huffman_nodes(pixelFrequency, pgm.maxGrayValue, symbolNums);

	//make encoded array with one element representing one byte including several encoded pixels
	unsigned char *encodedImage;
	unsigned long int lengthEncodedArray = 100;
	encodedImage = huffman_encode_image(&pgm, huffmanNodes, symbolNums - 1, &lengthEncodedArray);

	//store encoded array in output file
	store_huffman_encoded_data(argv[2], pgm.width, pgm.height, pgm.maxGrayValue, symbolNums - 1,
			huffmanNodes, lengthEncodedArray, encodedImage);

	return 0;
}

