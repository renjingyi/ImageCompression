/*
 * DPCM_decoding_function.c
 *
 *  Created on: Mar 30, 2018
 *      Author: jeann
 */

#include "libpnm.h"
#include "DPCM_decoding_function.h"
#include<stdio.h>
#include<string.h>
#include<malloc.h>

void Decode_Using_DPCM(char *in_filename_Ptr){
	/*file includes:
	  * 1. PGM image filename
	  * 1. PGM image sizes and maxGrayValue
	  * 2. prediction rule number
	  * 2. prediction value used for first pixel at the upper left corner
	  * 3. prediction rule for the initialized first two rows and two columns and last one column
	  * 4. error array
	  * 5. T values array if necessary
	*/
	//assume i always used for rows and j always used for columns
	int i, j;

	/*************************************************************************************************************
	 *************************************************************************************************************
	 * get information from input file
	 *************************************************************************************************************
	 *************************************************************************************************************/
	int height, width, maxGrayValue;
	int prediction_rule;
	int prediction_first;
	char pgm_filename[50] = "";
	//read file
	FILE *fp;
	fp = fopen(in_filename_Ptr, "r");
	//get PGM image filename
	fscanf(fp, "%s ", pgm_filename);
	//get PGM image sizes
	fscanf(fp, "%d %d %d ", &height, &width, &maxGrayValue);
	//create PGM image
	struct PGM_Image pgm_new;
	create_PGM_Image(&pgm_new, width, height, maxGrayValue);
	//get image prediction rule number and first pixel prediction value
	fscanf(fp, "%d %d ", &prediction_rule, &prediction_first);
	//get prediction rule numbers for initialization
	//first row, first column, second row, second column, last column
	int imgInit[5];
	for(i = 0; i < 5; i++){
		fscanf(fp, "%d ", &imgInit[i]);
	}
	//get error array
	int **err;
	err = (int **)malloc(height * sizeof(int *));
	for(i = 0; i < height; i++){
		err[i] = (int *)malloc(width * sizeof(int));
	}
	for(i = 0; i < height; i++){
		for(j = 0; j < width; j++){
			fscanf(fp, "%d ", &err[i][j]);
		}
	}
	//get T values array
	//only if prediction_rule is 4, using CALIC method, there will be T values array
	int **Tval;
	Tval = (int **)malloc((height - 2) * sizeof(int *));
	for(i = 0; i < height - 2; i++){
		Tval[i] = (int *)malloc((width - 3) * sizeof(int));
	}
	if(prediction_rule == 4){
		for(i = 0; i < height - 2; i++){
			for(j = 0; j < width - 3; j++){
				fscanf(fp, "%d ", &Tval[i][j]);
			}
		}
	}

	/*************************************************************************************************************
	 *************************************************************************************************************
	 * generate new PGM image
	 *    formula: current pixel value = prediction + error
	 *************************************************************************************************************
	 *************************************************************************************************************/
	//prepare image
	//according to prediction_first, we could get the upper left corner pixel value and store in image
	pgm_new.image[0][0] = prediction_first + err[0][0];
	//according to imgInit[], we could get first two rows and columns and the last column in image
	//first row and first column will use W method and N method respectively
	//                           as a common sense between encoder and decoder
	for(j = 1; j < width; j++){
		pgm_new.image[0][j] = pgm_new.image[0][j - 1] + err[0][j];
	}
	for(i = 1; i < height; i++){
		pgm_new.image[i][0] = pgm_new.image[i - 1][0] + err[i][0];
	}
	//second row and column and last column we need information obtained from encoder
	//second row
	switch (imgInit[2]){
	case 1:
		//look up W
		for(j = 1; j < width; j++){
			pgm_new.image[1][j] = pgm_new.image[1][j - 1] + err[1][j];
		}
		break;
	case 2:
		//look up N
		for(j = 1; j < width; j++){
			pgm_new.image[1][j] = pgm_new.image[0][j] + err[1][j];
		}
		break;
	case 3:
		//look up W/2+N/2
		for(j = 1; j < width; j++){
			pgm_new.image[1][j] = pgm_new.image[1][j - 1] / 2 + pgm_new.image[0][j] /2 + err[1][j];
		}
		break;
	default:
		printf("Invalid image initialization prediction rule values");
	}
	//second column and last column
	int col = 1;
	for(j = 3; j < 5; j++){
	    switch (imgInit[j]){
		case 1:
			//look up W
			for(i = 2; i < height; i++){
				pgm_new.image[i][col] = pgm_new.image[i][col - 1] + err[i][col];
			}
			break;
		case 2:
			//look up N
			for(i = 2; i < height; i++){
				pgm_new.image[i][col] = pgm_new.image[i - 1][col] + err[i][col];
			}
			break;
		case 3:
			//look up W/2+N/2
			for(i = 2; i < height; i++){
				pgm_new.image[i][col] = pgm_new.image[i][col - 1] / 2 + pgm_new.image[i - 1][col] /2 + err[i][col];
			}
			break;
		default:
			printf("Invalid image initialization prediction rule values");
		}
	    col = width - 1;
	}

	//the left part of image is directed to use specific decoding method to predict prediction value
	switch (prediction_rule){
	case 1:
		look_W(&pgm_new, err);
		break;
	case 2:
		look_N(&pgm_new, err);
		break;
	case 3:
		look_WNaverage(&pgm_new, err);
		break;
	case 4:
		look_CALIC(&pgm_new, err, Tval);
		break;
	default:
		printf("Invalid prediction rule number");
	}

	/**************************************************************************************************************
	 **************************************************************************************************************
	 * output PGM image
	 **************************************************************************************************************
	 **************************************************************************************************************/
	strcat(pgm_filename, ".pgm");
	save_PGM_Image(&pgm_new, pgm_filename, true);
	free_PGM_Image(&pgm_new);

	/************************************************************************************************************
     ************************************************************************************************************
	 * free dynamic memory allocation
	 ************************************************************************************************************
	 ************************************************************************************************************/
	free(err);
	free(Tval);
}

//use W to predict decoded pixels
void look_W(struct PGM_Image *img, int **err){
	int i,j;
	for(i = 2; i < img->height; i++){
		for(j = 2; j < img->width - 1; j++){
			img->image[i][j] = img->image[i][j - 1] + err[i][j];
		}
	}
}

//use N to predict decoded pixels
void look_N(struct PGM_Image *img, int **err){
	int i,j;
	for(i = 2; i < img->height; i++){
		for(j = 2; j < img->width - 1; j++){
			img->image[i][j] = img->image[i - 1][j] + err[i][j];
		}
	}
}

// use W/2 + N/2 to predict decoded pixels
void look_WNaverage(struct PGM_Image *img, int **err){
	int i,j;
	for(i = 2; i < img->height; i++){
		for(j = 2; j < img->width - 1; j++){
			img->image[i][j] = img->image[i][j - 1] / 2 + img->image[i - 1][j] / 2 + err[i][j];
		}
	}
}

//use binary mode and continuous-tone mode using GAP only
void look_CALIC(struct PGM_Image *img, int **err, int **Tval){
	int i,j,k;
	int Tcheck = 0;
	//create an array contains W,N,WW,NN,NW,NE,NNE
	int calicArr[7];
	//horizontal gradient and vertical gradient used in GAP if necessary
	int Hgradient, Vgradient;
	/*scan the image:
	 * 1. for each pixel create a calicArr[] to store surrounding seven pixel values
	 * 2. check Tval[][] array to know which prediction method is used for this pixel
	 * 3. according to T value, compute the prediction
	 * 4. prediction plus error to get pixel value
	 */
	for(i = 2; i < img->height; i++){
		for(j = 2; j < img->width - 1; j++){
			//for each pixel build up one corresponding calicArr[]
			calicArr[0] = img->image[i][j - 1];
			calicArr[1] = img->image[i - 1][j];
			calicArr[2] = img->image[i][j - 2];
			calicArr[3] = img->image[i - 2][j];
			calicArr[4] = img->image[i - 1][j - 1];
			calicArr[5] = img->image[i - 1][j + 1];
			calicArr[6] = img->image[i - 2][j + 1];
			//check T value for current pixel
			Tcheck = Tval[i - 2][j - 2];
			switch (Tcheck){
			case 0:
				//current pixel's prediction value is equal to that of W
				img->image[i][j] = calicArr[0];
				break;
			case 1:
				//current pixel's prediction value is not equal to that of W but the other value in calicArr[]
				for(k = 1; k < 7; k++){
					if(calicArr[k] != calicArr[0]){
						img->image[i][j] = calicArr[k] + err[i][j];
						break;
					}
				}
				break;
			case 2:
				//jump to continuous-tone mode using GAP
				Hgradient = abs(calicArr[0] - calicArr[2]) + abs(calicArr[1] - calicArr[4])
										             + abs(calicArr[5] - calicArr[1]);
				Vgradient = abs(calicArr[0] - calicArr[4]) + abs(calicArr[1] - calicArr[3])
								    		         + abs(calicArr[5] - calicArr[6]);
				int prediction;
				if(Vgradient - Hgradient > 80)
					prediction = calicArr[0];
				else{
					if(Hgradient - Vgradient > 80)
						prediction = calicArr[1];
					else{
						prediction = (calicArr[0] + calicArr[1]) / 2 + (calicArr[5] - calicArr[4]) / 4;
						if(Vgradient - Hgradient > 32)
							prediction = prediction / 2 + calicArr[0] / 2;
						else{
							if(Hgradient - Vgradient > 32)
								prediction = prediction / 2 + calicArr[1] / 2;
							else{
								if(Vgradient - Hgradient > 8)
									prediction = (prediction * 3) / 4 + calicArr[0] / 4;
								else{
									if(Hgradient - Vgradient > 8)
										prediction = (prediction * 3) / 4 + calicArr[1] / 4;
								}
							}
						}
					}
				}
				img->image[i][j] = prediction + err[i][j];
				break;
			default:
				printf("Unknown T value for current pixel");
			}
		}
	}
}


