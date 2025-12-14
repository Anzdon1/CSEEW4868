#include "bert_tiny.h"
#include "load_bert.h"
#include "attention_layer.h"
#include "fully_connected_layer.h"
#include "elementwise_layer.h"

int main(int argc, char **argv) {
    fprintf(stderr, "BERT-Tiny Programmer's View\n");
    const char* sentence = getenv("SENTENCE");
    int sen = sentence_index(sentence);

    fprintf(stderr, "Running for sentence: '%d'\n", sen);

    if (sen < 0) {
        fprintf(stderr, "ERROR: SENTENCE='%s' not found in SEN_TYPE list\n", sentence);
        fprintf(stderr, "       Valid options: ");
        for (int i = 0; i < 10; ++i) fprintf(stderr, "%s%s", SEN_TYPE[i], (i==9?"\n":", "));
        return 1;
    }

    float* query_w = (float *)calloc(D_MODEL*D_MODEL, sizeof(float));
    float* query_b = (float *)calloc(D_MODEL, sizeof(float));
    float* key_w = (float *)calloc(D_MODEL*D_MODEL, sizeof(float));
    float* key_b = (float *)calloc(D_MODEL, sizeof(float));
    float* value_w = (float *)calloc(D_MODEL*D_MODEL, sizeof(float));
    float* value_b = (float *)calloc(D_MODEL, sizeof(float));
    float* dense_w = (float *)calloc(D_MODEL*D_MODEL, sizeof(float));
    float* dense_b = (float *)calloc(D_MODEL, sizeof(float));
    float* ffn1_w = (float *)calloc(D_MODEL*D_MODEL*4, sizeof(float));
    float* ffn1_b = (float *)calloc(D_MODEL*4, sizeof(float));
    float* ffn2_w = (float *)calloc(D_MODEL*D_MODEL*4, sizeof(float));
    float* ffn2_b = (float *)calloc(D_MODEL, sizeof(float));
    float* pooler_w = (float *)calloc(D_MODEL*D_MODEL, sizeof(float));
    float* pooler_b = (float *)calloc(D_MODEL, sizeof(float));
    float* classi_w = (float *)calloc(D_MODEL*2, sizeof(float));
    float* classi_b = (float *)calloc(2, sizeof(float));
    float* block_in = (float *)calloc(MAX_SEQ*D_MODEL, sizeof(float));
    float* query_o = (float *)calloc(MAX_SEQ*D_MODEL, sizeof(float));
    float* key_o = (float *)calloc(MAX_SEQ*D_MODEL, sizeof(float));
    float* value_o = (float *)calloc(MAX_SEQ*D_MODEL, sizeof(float));
    float* attn_o = (float *)calloc(MAX_SEQ*D_MODEL, sizeof(float));
    float* dense_o = (float *)calloc(MAX_SEQ*D_MODEL, sizeof(float));
    float* rsd1_o = (float *)calloc(MAX_SEQ*D_MODEL, sizeof(float));
    float* ffn1_o = (float *)calloc(MAX_SEQ*D_MODEL*4, sizeof(float));
    float* ffn2_o = (float *)calloc(MAX_SEQ*D_MODEL, sizeof(float));
    float* rsd2_o = (float *)calloc(MAX_SEQ*D_MODEL, sizeof(float));
    float* pooler_o = (float *)calloc(D_MODEL, sizeof(float));
    float* classifier_o = (float *)calloc(2, sizeof(float));


	load_bert(query_w, query_b, key_w, key_b, value_w, value_b,
                dense_w, dense_b, ffn1_w, ffn1_b, ffn2_w, ffn2_b, 
                pooler_w, pooler_b, classi_w, classi_b);
    load_block_in(sentence, block_in);

// run query
    BertMatrixVector(MAX_SEQ, D_MODEL, D_MODEL, query_w, block_in, query_b, query_o);
// run key
    BertMatrixVector(MAX_SEQ, D_MODEL, D_MODEL, key_w, block_in, key_b, key_o);
// run value
    BertMatrixVector(MAX_SEQ, D_MODEL, D_MODEL, value_w, block_in, value_b, value_o);
// run attention
    BertAttention(MAX_SEQ, D_MODEL/N_HEAD, N_HEAD, SEN_MASK[sen], query_o, key_o, value_o, attn_o);
// run dense
    BertMatrixVector(MAX_SEQ, D_MODEL, D_MODEL, dense_w, attn_o, dense_b, dense_o);
// run residual
    BertResidualAddition(MAX_SEQ, D_MODEL, block_in, dense_o, rsd1_o);
// run ffn1
    BertMatrixVector(MAX_SEQ, D_MODEL, D_MODEL*4, ffn1_w, rsd1_o, ffn1_b, ffn1_o);
    BertGelu(MAX_SEQ, D_MODEL*4, ffn1_o);
// run ffn2
    BertMatrixVector(MAX_SEQ, D_MODEL*4, D_MODEL, ffn2_w, ffn1_o, ffn2_b, ffn2_o);
// run residual
    BertResidualAddition(MAX_SEQ, D_MODEL, rsd1_o, ffn2_o, rsd2_o);
// run pooler
    BertMatrixVector(1, D_MODEL, D_MODEL, pooler_w, rsd2_o, pooler_b, pooler_o);
    BertTanh(1, D_MODEL, pooler_o);
// run classifier
    BertMatrixVector(1, D_MODEL, 2, classi_w, pooler_o, classi_b, classifier_o);
// print the answer
    int idx = argmax(classifier_o, 2);
    print_label(idx, SEN[sen]);
    return 0;
}
