/*
 * DPCM_encoding_function.c
 *
 *  Created on: Mar 28, 2018
 *      Author: jeann
 */

#include "libpnm.h"
#include "DPCM_encoding_function.h"
#include<stdio.h>
#include<malloc.h>
#include<string.h>

void Encode_Using_DPCM(char *in_PGM_filename_Ptr, int prediction_rule,
		float *avg_abs_error_Ptr, float *std_abs_error_Ptr){
	//assume i always used for rows and j always used for columns
	int i,j;
	//load pgm_image
	struct PGM_Image pgm_image;
	load_PGM_Image(&pgm_image, in_PGM_filename_Ptr);

	//create a 2-D error array
	int **errors;
	//create a 2-D array to store T values in CALIC and will allocate memory in CALIC method
	int **Tval;
	errors = (int **)malloc(pgm_image.height * sizeof(int *));
	for(i = 0; i < pgm_image.height; i++){
		errors[i] = (int *)malloc(pgm_image.width * sizeof(int));
	}
	//initialize error array
	for(i = 0; i < pgm_image.height; i++){
		for(j = 0; j < pgm_image.width; j++){
			errors[i][j] = 0;
		}
	}

	/************************************************************************************************************
	 ************************************************************************************************************
	 * prepare the error array:
	 * the pixel at the first row and the first column (i.e., top left corner) is always predicted as 128
	 * the rest of pixels at the first row are always predicted as W
	 * pixels at the second row are always predicted as N
	 * the rest of pixels at the first two columns are always predicted as N
	 * the rest of pixels at the last column are always predicted as N
	 ************************************************************************************************************
	 ************************************************************************************************************/
	//pre-dispose the first row
	errors[0][0] = pgm_image.image[0][0] - 128;
	for(j = 1; j < pgm_image.width; j++){
		//error = current_pixel_value - prediction(previous_pixel_value)
		errors[0][j] = pgm_image.image[0][j] - pgm_image.image[0][j - 1];
	}
	//pre-dispose the second row
	for(j = 0; j < pgm_image.width; j++){
		errors[1][j] = pgm_image.image[1][j] - pgm_image.image[0][j];
	}
	//pre-dispose the first column
	for(i = 2; i < pgm_image.height; i++){
		errors[i][0] = pgm_image.image[i][0] - pgm_image.image[i - 1][0];
	}
	//pre-dispose the second column
	for(i = 2; i < pgm_image.height; i++){
		errors[i][1] = pgm_image.image[i][1] - pgm_image.image[i - 1][1];
	}
	//pre-dispose the last column
	for(i = 2; i < pgm_image.height; i++){
		errors[i][pgm_image.width - 1] = pgm_image.image[i][pgm_image.width - 1] -
				                                   pgm_image.image[i - 1][pgm_image.width - 1];
	}

	/************************************************************************************************************
	 ************************************************************************************************************
	 * according to prediction_rule, there are several scenarios to consider
	 * if prediction_rule = 1, use W to predict encoded pixels
	 * if prediction_rule = 2, use N to predict encoded pixels
	 * if prediction_rule = 3, use W/2 +N/2 to predict encoded pixels
	 * if prediction_rule = 4, use CALIC initial prediction
	 ************************************************************************************************************
	 ************************************************************************************************************/
	switch (prediction_rule){
	case 1:
		lookup_W(&pgm_image, errors);
		break;
	case 2:
		lookup_N(&pgm_image, errors);
		break;
	case 3:
		lookup_WNaverage(&pgm_image, errors);
		break;
	case 4:
		//we don't need to do first two rows
		Tval = (int **)malloc((pgm_image.height - 2) * sizeof(int *));
		for(i = 0; i < pgm_image.height - 2; i++){
			//we don't need to do first two columns and last column
			Tval[i] = (int *)malloc((pgm_image.width - 3) * sizeof(int));
		}
		lookup_CALIC(&pgm_image, errors, Tval);
		break;
	default:
		printf("Invalid prediction rule mode");
	}

	/************************************************************************************************************
	 ************************************************************************************************************
	 * write necessary information for decoder into files
	 ************************************************************************************************************
	 ************************************************************************************************************/
	FILE *fp_toDecoder;
	char outfile[50] = "";
	sprintf(outfile, "%s.%d.DPCM.txt", in_PGM_filename_Ptr, prediction_rule);
	fp_toDecoder = fopen(outfile, "w+");
	/*file includes:
	 * 1. PGM image file name
	 * 2. PGM image sizes and maxGrayValue
	 * 3. prediction rule number
	 * 4. prediction value used for first pixel at the upper left corner
	 * 5. prediction rule for the initialized first two rows and two columns and last one column
	 * 6. error array
	 * 7. T values array if necessary
	 */
	//No.1
	sprintf(outfile, "%s.%d.DPCM", in_PGM_filename_Ptr, prediction_rule);
	fprintf(fp_toDecoder, "%s ", outfile);
	//No.2
	fprintf(fp_toDecoder, "%d %d %d ", pgm_image.height, pgm_image.width, pgm_image.maxGrayValue);
	//No.3
	fprintf(fp_toDecoder, "%d ", prediction_rule);
	//No.4
	fprintf(fp_toDecoder, "128 ");
	//No.5, first row, first column, second row, second column, last column
	fprintf(fp_toDecoder, "1 2 2 2 2 ");
	//No.6
	for(i = 0; i < pgm_image.height; i++){
		for(j = 0; j < pgm_image.width; j++){
			fprintf(fp_toDecoder, "%d ", errors[i][j]);
		}
	}
	//No.7
	if(prediction_rule == 4){
		for(i = 0; i < pgm_image.height - 2; i++){
			for(j = 0; j < pgm_image.width - 3; j++){
				fprintf(fp_toDecoder, "%d ", Tval[i][j]);
			}
		}
	}

	/************************************************************************************************************
	 ************************************************************************************************************
	 * write absolute error values and their frequency to file
	 ************************************************************************************************************
	 ************************************************************************************************************/
	FILE *fp_err;
	sprintf(outfile, "%s.%d.errors.csv", in_PGM_filename_Ptr, prediction_rule);
	fp_err = fopen(outfile, "w+");
	/*file includes:
	 * 1. absolute error values which have non-zero frequency
	 * 2. corresponding frequencies
	 */
	//first to declare an frequency array with (MAX_GRAY_VALUE + 1) elements
	//the max length of the frequency array should be the maximum absolute difference between pixel values
	int *errFrequency;
	errFrequency = (int *)malloc((pgm_image.maxGrayValue + 1) * sizeof(int));
	for(i = 0; i < pgm_image.maxGrayValue + 1; i++){
		errFrequency[i] = 0;
	}
	//call generate_frequency() method to generate error frequency array
	generate_frequency(errors, errFrequency, pgm_image.height, pgm_image.width);
	//the first row in errors.csv is non_zero error absolute values
	for(i = 0; i < pgm_image.maxGrayValue + 1; i++){
		if(errFrequency[i] != 0)
			fprintf(fp_err, "%d,", i);
	}
	fprintf(fp_err, "\n");
	//the second row in errors.csv is corresponding error frequencies
	for(i = 0; i < pgm_image.maxGrayValue + 1; i++){
		if(errFrequency[i] != 0)
			fprintf(fp_err, "%d,", errFrequency[i]);
	}

	/************************************************************************************************************
	 ************************************************************************************************************
	 * compute the average and the standard deviation of absolute error array
	 ************************************************************************************************************
	 ************************************************************************************************************/
	float avg = avg_abs_err(errFrequency, pgm_image.maxGrayValue + 1);
	*avg_abs_error_Ptr = avg;
	float std = std_abs_err(errFrequency, pgm_image.maxGrayValue + 1, avg);
	*std_abs_error_Ptr = std;

	/************************************************************************************************************
	 ************************************************************************************************************
	 * free dynamic memory allocation
	 ************************************************************************************************************
	 ************************************************************************************************************/
	free(errors);
	free(Tval);
	free(errFrequency);
}

//use W to predict encoded pixels
void lookup_W(struct PGM_Image *img, int **err){
	int i,j;
	for(i = 2; i < img->height; i++){
		for(j = 2; j < img->width - 1; j++){
			err[i][j] = img->image[i][j] - img->image[i][j - 1];
		}
	}
}

//use N to predict encoded pixels
void lookup_N(struct PGM_Image *img, int **err){
	int i,j;
	for(i = 2; i < img->height; i++){
		for(j = 2; j < img->width - 1; j++){
			err[i][j] = img->image[i][j] - img->image[i - 1][j];
		}
	}
}

// use W/2 + N/2 to predict encoded pixels
void lookup_WNaverage(struct PGM_Image *img, int **err){
	int i,j;
	for(i = 2; i < img->height; i++){
		for(j = 2; j < img->width - 1; j++){
			err[i][j] = img->image[i][j] - (img->image[i][j - 1] /2 + img->image[i - 1][j] / 2);
		}
	}
}

//use binary mode and continuous-tone mode using GAP only
void lookup_CALIC(struct PGM_Image *img, int **err, int **Tval){
	int i,j;
	int check = 0;
	//create an array contains W,N,WW,NN,NW,NE,NNE
	int calicArr[7];
	//val1 and val2 store different values in calicArr[]
	int val1 = 0, val2 = 0;
	/*scan the image:
	 * 1. for each pixel create a calicArr[] to store surrounding seven pixel values
	 * 2. use binaryCheck() method to check whether current pixel suits binary mode or not
	 * 3. according to the different binaryCheck() returns select different disposition to send
	 * 4. we need to complete error array and T values array in this function
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
			//check whether current pixel fits binary mode
			check = binaryCheck(calicArr, 7, &val1, &val2);
			if(check == 1){
				//it fits binary mode and has exactly two different values in seven pixel values
				if(img->image[i][j] == calicArr[0]){
					Tval[i - 2][j - 2] = 0;
					err[i][j] = 0;
				}
				else{
					Tval[i - 2][j - 2] = 1;
					err[i][j] = img->image[i][j] - val2;
				}
			}
			else if(check == 0){
				//it fits binary mode but only get one identical value
				if(img->image[i][j] == calicArr[0]){
					Tval[i - 2][j - 2] = 0;
					err[i][j] = 0;
				}
				//if current pixel value is not same as the identical one in seven values then goes to GAP
				else
					goto GAP;
			}
			else{
				//it is not in binary mode and should turn to continuous-tone mode
				GAP: Tval[i - 2][j - 2] = 2;
				//horizontal gradient and vertical gradient used in GAP
				int Hgradient, Vgradient;
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
				err[i][j] = img->image[i][j] - prediction;
			}
		}
	}
}

/*function to check whether corresponding seven pixels have no more than two values
 * return: 1 means they have two different values;
 *         0 means they have one same value;
 *         -1 means they have more than two values.
 */
int binaryCheck(int *arr, int num, int *val1, int *val2){
	int i, j;
	int flag = 0;
	*val1 = arr[0];

	for(i = 1; i < num; i++){
		if(arr[i] != *val1){
			*val2 = arr[i];
			for(j = i + 1; j < num; j++){
				if(arr[j] != *val1 && arr[j] != *val2){
					flag = -1;
					break;
				}
				else
					flag = 1;
			}
			break;
		}
	}
	return flag;
}

//function to complete array's elements frequencies
void generate_frequency(int **arr, int *freq, int arr_h, int arr_w){
	int i, j;
	int absVal = -1;
	for(i = 0; i < arr_h; i++){
		for(j = 0; j < arr_w; j++){
			absVal = abs(arr[i][j]);
			freq[absVal] += 1;
		}
	}
}

//function to compute mean of absolute error
float avg_abs_err(int *abs_err, int err_len){
	float mean = 0;
	unsigned long int sum = 0;
	unsigned long int num = 0;
	int i;
	for(i = 0; i < err_len; i++){
		sum += abs_err[i] * i;
		num += abs_err[i];
	}
	mean = (float)sum / num;
	return mean;
}

//function to compute standard derivation of absolute error
float std_abs_err(int *abs_err, int err_len, float avg){
	float std = 0;
	long double sum = 0;
	unsigned long int num = 0;
	int i;
	for(i = 0; i < err_len; i++){
		sum += (i - avg) * (i - avg) * abs_err[i];
		num += abs_err[i];
	}
	std = square_root(sum / num);
	return std;
}

//function to compute square root
float square_root(float num){
	float root = 1;
	while(abs(root - num / root) > 1e-6){
		root = (root + num / root) / 2;
	}
	return root;
}


