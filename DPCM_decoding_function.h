/*
 * DPCM_decoding_function.h
 *
 *  Created on: Mar 30, 2018
 *      Author: jeann
 */

#ifndef DPCM_DECODING_FUNCTION_H_
#define DPCM_DECODING_FUNCTION_H_

void Decode_Using_DPCM(char *in_filename_Ptr);

void look_W(struct PGM_Image *img, int **err);
void look_N(struct PGM_Image *img, int **err);
void look_WNaverage(struct PGM_Image *img, int **err);
void look_CALIC(struct PGM_Image *img, int **err, int **Tval);

#endif /* DPCM_DECODING_FUNCTION_H_ */
