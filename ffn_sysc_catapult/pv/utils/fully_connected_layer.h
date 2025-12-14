#ifndef _BERT_FULLY_CONNECTED_H_
#define _BERT_FULLY_CONNECTED_H_

#include "bert_tiny.h"

void BertMatrixVector(int seq_batch, int row, int col,
					float *matrix, float *vector, float *bias, float *out){

	fprintf(stderr, "  > running BertMatrixVector of sequence %d dimension %d\n", seq_batch, row);
	for (int s=0; s<seq_batch; s++){
		for (int in_c=0; in_c<row; in_c++){
			for (int out_c=0; out_c<col; out_c++){
				out[col*s+out_c] += matrix[col*in_c+out_c] * vector[row*s+in_c]; 
			}
		}

		for (int out_c=0; out_c<col; out_c++){
			out[col*s+out_c] += bias[out_c];
		}
	}

}

#endif