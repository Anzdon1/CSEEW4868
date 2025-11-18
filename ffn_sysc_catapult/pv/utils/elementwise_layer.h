#ifndef _BERT_ELEMENT_H_
#define _BERT_ELEMENT_H_

#include "bert_tiny.h"

void BertResidualAddition(int seq_batch, int dim, float *vector1, float *vector2, float *out){
	fprintf(stderr, "  > running BertResidualAddition\n");

	for (int s=0; s<seq_batch; s++){
		for (int out_c=0; out_c<dim; out_c++){
			out[dim*s+out_c] = vector1[dim*s+out_c] + vector2[dim*s+out_c];
		}
	}
}

float Gelu(float x){
    float y = 0.5 * x * (1 + tanh(sqrt(2.0 / PI) * (x + A * x * x * x)));
    return y;
}


void BertGelu(int seq_batch, int dim, float *in){
	for (int s=0; s<seq_batch; s++){
		for (int d=0; d<dim; d++){
			float gelu_in = in[dim*s+d];
			in[dim*s+d] = Gelu(gelu_in);
		}
	}

}

void BertTanh(int seq_batch, int dim, float *in){
	for (int s=0; s<seq_batch; s++){
		for (int d=0; d<dim; d++){
			float tanh_in = in[dim*s+d];
			in[dim*s+d] = tanh(tanh_in);
		}
	}
}

int argmax(float *arr, int size) {
    int idx = 0;
    float max_val = -10000;
    for (int i=0; i<size; i++) {
		fprintf(stderr, "logits idx %d: %f\n", i, arr[i]);
        if (arr[i] > max_val) {
            max_val = arr[i];
            idx = i;
        }
    }
    return idx;
}

void print_label(int idx, const char* sentence) {
    if (idx == 1) {
        fprintf(stderr, "'%s' has Positive vibe 0_< \n", sentence);
    } else if (idx == 0) {
        fprintf(stderr, "'%s' has Negative vibe TT\n", sentence);
    } else {
        fprintf(stderr, "Unknown class: %d\n", idx);
    }
}

#endif