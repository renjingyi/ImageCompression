/*
 * DPCM_encoding_function.h
 *
 *  Created on: Mar 28, 2018
 *      Author: jeann
 */

#ifndef DPCM_ENCODING_FUNCTION_H_
#define DPCM_ENCODING_FUNCTION_H_

void Encode_Using_DPCM(char *in_PGM_filename_Ptr, int prediction_rule,
		float *avg_abs_error_Ptr, float *std_abs_error_Ptr);

void lookup_W(struct PGM_Image *img, int **err);
void lookup_N(struct PGM_Image *img, int **err);
void lookup_WNaverage(struct PGM_Image *img, int **err);
void lookup_CALIC(struct PGM_Image *img, int **err, int **Tval);

int binaryCheck(int *arr, int num, int *val1, int *val2);
void generate_frequency(int **arr, int *freq, int arr_h, int arr_w);

float avg_abs_err(int *abs_err, int err_len);
float std_abs_err(int *abs_err, int err_len, float avg);
float square_root(float num);

#endif /* DPCM_ENCODING_FUNCTION_H_ */
