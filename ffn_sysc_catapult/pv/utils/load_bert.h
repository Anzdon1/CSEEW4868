#ifndef _BERT_MODEL_H_
#define _BERT_MODEL_H_

#include "bert_tiny.h"

char model_dir[] = "../models/";
char input_dir[] = "../data/";

void load_bert(float* query_w, float* query_b, float* key_w, float* key_b, float* value_w, float* value_b, 
				float* dense_w, float* dense_b,float* ffn1_w, float* ffn1_b, float* ffn2_w, float* ffn2_b, 
				float* pooler_w, float* pooler_b, float* classi_w, float* classi_b
){
// query
	char query_w_bin[1024];
	sprintf(query_w_bin, "%s%s", model_dir, "query_w_t.bin");
	FILE* inFile_query_w = fopen(query_w_bin, "rb");
	if (inFile_query_w != NULL) {
		fread(query_w, sizeof(float), D_MODEL*D_MODEL, inFile_query_w);
		fclose(inFile_query_w);
	} else { 
		printf("   Can't find query weight\n");
	}
	char query_b_bin[1024];
	sprintf(query_b_bin, "%s%s", model_dir, "query_b.bin");
	FILE* inFile_query_b = fopen(query_b_bin, "rb");
	if (inFile_query_b != NULL) {
		fread(query_b, sizeof(float), D_MODEL, inFile_query_b);
		fclose(inFile_query_b);
	} else { 
		printf("   Can't find query bias\n");
	}

// key
	char key_w_bin[1024];
	sprintf(key_w_bin, "%s%s", model_dir, "key_w_t.bin");
	FILE* inFile_key_w = fopen(key_w_bin, "rb");
	if (inFile_key_w != NULL) {
		fread(key_w, sizeof(float), D_MODEL*D_MODEL, inFile_key_w);
		fclose(inFile_key_w);
	} else { 
		printf("   Can't find key weight\n");
	}
	char key_b_bin[1024];
	sprintf(key_b_bin, "%s%s", model_dir, "key_b.bin");
	FILE* inFile_key_b = fopen(key_b_bin, "rb");
	if (inFile_key_b != NULL) {
		fread(key_b, sizeof(float), D_MODEL, inFile_key_b);
		fclose(inFile_key_b);
	} else { 
		printf("   Can't find key bias\n");
	}

// value
	char value_w_bin[1024];
	sprintf(value_w_bin, "%s%s", model_dir, "value_w_t.bin");
	FILE* inFile_value_w = fopen(value_w_bin, "rb");
	if (inFile_value_w != NULL) {
		fread(value_w, sizeof(float), D_MODEL*D_MODEL, inFile_value_w);
		fclose(inFile_value_w);
	} else { 
		printf("   Can't find value weight\n");
	}
	char value_b_bin[1024];
	sprintf(value_b_bin, "%s%s", model_dir, "value_b.bin");
	FILE* inFile_value_b = fopen(value_b_bin, "rb");
	if (inFile_value_b != NULL) {
		fread(value_b, sizeof(float), D_MODEL, inFile_value_b);
		fclose(inFile_value_b);
	} else { 
		printf("   Can't find value bias\n");
	}

// dense
	char dense_w_bin[1024];
	sprintf(dense_w_bin, "%s%s", model_dir, "dense_w_t.bin");
	FILE* inFile_dense_w = fopen(dense_w_bin, "rb");
	if (inFile_dense_w != NULL) {
		fread(dense_w, sizeof(float), D_MODEL*D_MODEL, inFile_dense_w);
		fclose(inFile_dense_w);
	} else { 
		printf("   Can't find dense weight\n");
	}

	char dense_b_bin[1024];
	sprintf(dense_b_bin, "%s%s", model_dir, "dense_b.bin");
	FILE* inFile_dense_b = fopen(dense_b_bin, "rb");
	if (inFile_dense_b != NULL) {
		fread(dense_b, sizeof(float), D_MODEL, inFile_dense_b);
		fclose(inFile_dense_b);
	} else { 
		printf("   Can't find dense bias\n");
	}

// ffn1
	char ffn1_w_bin[1024];
	sprintf(ffn1_w_bin, "%s%s", model_dir, "ffn1_w_t.bin");
	FILE* inFile_ffn1_w = fopen(ffn1_w_bin, "rb");
	if (inFile_ffn1_w != NULL) {
		fread(ffn1_w, sizeof(float), D_MODEL*D_MODEL*4, inFile_ffn1_w);
		fclose(inFile_ffn1_w);
	} else { 
		printf("   Can't find ffn1 weight\n");
	}

	char ffn1_b_bin[1024];
	sprintf(ffn1_b_bin, "%s%s", model_dir, "ffn1_b.bin");
	FILE* inFile_ffn1_b = fopen(ffn1_b_bin, "rb");
	if (inFile_ffn1_b != NULL) {
		fread(ffn1_b, sizeof(float), D_MODEL*4, inFile_ffn1_b);
		fclose(inFile_ffn1_b);
	} else { 
		printf("   Can't find ffn1 bias\n");
	}

// ffn2
	char ffn2_w_bin[1024];
	sprintf(ffn2_w_bin, "%s%s", model_dir, "ffn2_w_t.bin");
	FILE* inFile_ffn2_w = fopen(ffn2_w_bin, "rb");
	if (inFile_ffn2_w != NULL) {
		fread(ffn2_w, sizeof(float), D_MODEL*D_MODEL*4, inFile_ffn2_w);
		fclose(inFile_ffn2_w);
	} else { 
		printf("   Can't find ffn2 weight\n");
	}

	char ffn2_b_bin[1024];
	sprintf(ffn2_b_bin, "%s%s", model_dir, "ffn2_b.bin");
	FILE* inFile_ffn2_b = fopen(ffn2_b_bin, "rb");
	if (inFile_ffn2_b != NULL) {
		fread(ffn2_b, sizeof(float), D_MODEL, inFile_ffn2_b);
		fclose(inFile_ffn2_b);
	} else { 
		printf("   Can't find ffn2 bias\n");
	}

// pooler
	char pooler_w_bin[1024];
	sprintf(pooler_w_bin, "%s%s", model_dir, "pooler_w_t.bin");
	FILE* inFile_pooler_w = fopen(pooler_w_bin, "rb");
	if (inFile_pooler_w != NULL) {
		fread(pooler_w, sizeof(float), D_MODEL*D_MODEL, inFile_pooler_w);
		fclose(inFile_pooler_w);
	} else { 
		printf("   Can't find pooler weight\n");
	}

	char pooler_b_bin[1024];
	sprintf(pooler_b_bin, "%s%s", model_dir, "pooler_b.bin");
	FILE* inFile_pooler_b = fopen(pooler_b_bin, "rb");
	if (inFile_pooler_b != NULL) {
		fread(pooler_b, sizeof(float), D_MODEL, inFile_pooler_b);
		fclose(inFile_pooler_b);
	} else { 
		printf("   Can't find pooler bias\n");
	}

// classifier
	char classi_w_bin[1024];
	sprintf(classi_w_bin, "%s%s", model_dir, "classifier_w_t.bin");
	FILE* inFile_classi_w = fopen(classi_w_bin, "rb");
	if (inFile_classi_w != NULL) {
		fread(classi_w, sizeof(float), D_MODEL*2, inFile_classi_w);
		fclose(inFile_classi_w);
	} else { 
		printf("   Can't find classifier weight\n");
	}

	char classi_b_bin[1024];
	sprintf(classi_b_bin, "%s%s", model_dir, "classifier_b.bin");
	FILE* inFile_classi_b = fopen(classi_b_bin, "rb");
	if (inFile_classi_b != NULL) {
		fread(classi_b, sizeof(float), 2, inFile_classi_b);
		fclose(inFile_classi_b);
	} else { 
		printf("   Can't find classifier bias\n");
	}
}

int sentence_index(const char* s) {
    for (int i = 0; i < 10; ++i) {
        if (strcmp(SEN_TYPE[i], s) == 0) return i;
    }
    return -1;
}

void load_block_in(const char* sentence, float* block_in) {
// block in
	char block_in_bin[1024];
	sprintf(block_in_bin, "%s%s%s", input_dir, sentence, ".bin");
	FILE* inFile_block_i = fopen(block_in_bin, "rb");
	if (inFile_block_i != NULL) {
		fread(block_in, sizeof(float), D_MODEL*MAX_SEQ, inFile_block_i);
		fclose(inFile_block_i);
		// printf("   Load input for block %d len %d completed\n", lyr+1, D_MODEL*MAX_SEQ);
	} else { 
		printf("   Can't find input\n");
	}
}

#endif