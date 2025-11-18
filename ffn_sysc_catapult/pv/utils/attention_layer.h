#ifndef _BERT_ATTENTION_H_
#define _BERT_ATTENTION_H_

#include "bert_tiny.h"

void BertAttention(int seq_batch, int dim, int head, int mask,
				float *query, float *key, float *value, float *output
){
	fprintf(stderr, "  > running BertAttention \n");

	float **score = (float**)malloc(sizeof(float*)*head);
	float **norm_score = (float**)malloc(sizeof(float*)*head);
	for (int h=0; h<head; h++){
		score[h] = (float*)calloc(seq_batch*seq_batch, sizeof(float));
		norm_score[h] = (float*)calloc(seq_batch*seq_batch, sizeof(float));
	}

	// int count = 0;
	// query * key matmul
	for (int s=0; s<seq_batch; s++){
		for (int h=0; h<head; h++){
			for (int d=0; d<dim; d++){
				for (int token=0; token<seq_batch; token++){
					score[h][seq_batch*s+token] += query[head*dim*s+dim*h+d]* key[head*dim*token+dim*h+d];
				}
			}
		}
	}

	/*
		softmax with masking
		-------------- token ------------
		| <- unmasked->	| <- masked ->	|
		|				|   0   0   0	|
		seq_batch		|   0   0   0	|
		|				|   0   0   0	|
		|				|   0   0   0	|
		-------------- token ------------
	*/
	// count = 0;
	for (int h=0; h<head; h++){
		for (int s=0; s<seq_batch; s++){
			float exp_sum = 0;
			for (int token=0; token<seq_batch; token++){
				float scaled_score = exp(1 + score[h][seq_batch*s+token]/8); 
				if (token < mask){
					exp_sum += scaled_score;
					score[h][seq_batch*s+token] = scaled_score;
				} else {
					score[h][seq_batch*s+token] = 0;
				}
			}

			for (int token=0; token<seq_batch; token++){
				norm_score[h][seq_batch*s+token] = score[h][seq_batch*s+token] / exp_sum;
			}
		}
	}

	for (int h=0; h<head; h++){
		for (int s=0; s<seq_batch; s++){
			for (int token=0; token<seq_batch; token++){
				for (int d=0; d<dim; d++){
					output[head*dim*s+dim*h+d] += norm_score[h][seq_batch*s+token] * value[head*dim*token+dim*h+d];
				}
			}
		}
	}

}

#endif