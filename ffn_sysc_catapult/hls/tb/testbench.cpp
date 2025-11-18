//Copyright (c) 2011-2024 Columbia University, System Level Design Group
//SPDX-License-Identifier: Apache-2.0

#include "testbench.hpp"
#include "ac_math/ac_random.h"
#include <mc_connections.h>
#include <mc_scverify.h>
#include "../../pv/utils/bert_tiny.h"
#include "../../pv/utils/load_bert.h"
#include "../../pv/utils/attention_layer.h"
#include "../../pv/utils/fully_connected_layer.h"
#include "../../pv/utils/elementwise_layer.h"
#include <fstream>
#include <iomanip>

#define ERR_THRESHOLD 0.01
#define PI  3.14159265358979323846
#define A 0.044715

// #define ACCELERATED_SIM 1

#define TARGET_LAYER 2

std::ofstream ofs;

void init_tensor(float* tensor, const int size, bool random, bool print)
{
    float value;
    for (int i = 0; i < size; ++i) {
        if (random){
            value = rand() / float(RAND_MAX);
        }
        else {
            value = float(0);
        }
	    
	    tensor[i] = value - 0.5;
        if (print){
            printf("tensor[%d] = %f \n", i, tensor[i]);
        }
    }
}



conf_info_t testbench::load_config()
{
    conf_info_t config;
    config.seqlen       = seqlen;
    config.indim        = indim;
    config.outdim       = outdim;
    config.addrI        = addrI;
    config.addrW        = addrW;
    config.addrB        = addrB;
    config.addrO        = addrO;
    return config;
}



float gelu(float x){
    float y = 0.5 * x * (1 + tanh(sqrt(2.0 / PI) * (x + A * x * x * x)));
    return y;
}


void testbench::load_memory(float *file_arr, uint32_t base_addr, uint32_t size)
{
#if (DMA_WORD_PER_BEAT == 0)
    for (int i=0; i<size; i++)
    {
        ac_int<DATA_WIDTH> data_bv = file_arr[i];

        for (int j=0; j<DMA_BEAT_PER_WORD; j++)
        {
            mem[DMA_BEAT_PER_WORD * i + j] = data_bv.slc<DMA_WIDTH>(j * DMA_WIDTH);
        }
    }
#else
    for (int i=0; i <size/DMA_WORD_PER_BEAT; i++)
    {
        ac_int<DMA_WIDTH> data_bv;
        for (int j=0; j<DMA_WORD_PER_BEAT; j++)
        {
            FPDATA_WORD fpdata_word;
        #ifdef FL_POINT
            float data = file_arr[i* DMA_WORD_PER_BEAT + j];
            FPDATA fpdata = FPDATA(data);
            f2int(fpdata, fpdata_word);
        #else
            ac_ieee_float32 data = file_arr[i* DMA_WORD_PER_BEAT + j];
            FPDATA fpdata = data.convert_to_ac_fixed<FPDATA_WL,FPDATA_IL,true,AC_TRN, AC_WRAP>();
            fx2int(fpdata, fpdata_word);
        #endif
            data_bv.set_slc(j*DATA_WIDTH, fpdata_word);
        }
        mem[base_addr/DMA_WORD_PER_BEAT + i] = data_bv;
    }
#endif
}

#if TARGET_LAYER == 1

void testbench::compile_config()
{
#if ACCELERATED_SIM == 1
    seqlen  = 32;
    indim   = 128;
    outdim  = 128;
#else
    seqlen  = 32;
    indim   = 128;
    outdim  = 512;
#endif
    sw_in_size          = seqlen*indim;
    sw_weight_size      = indim*outdim;
    sw_out_size         = seqlen*outdim;
    in_size             = round_up(sw_in_size, DMA_WORD_PER_BEAT);
    weight_size         = round_up(sw_weight_size, DMA_WORD_PER_BEAT);
    out_size            = round_up(sw_out_size, DMA_WORD_PER_BEAT);
    bias_size           = round_up(outdim, DMA_WORD_PER_BEAT);
    addrI               = 0;
    addrW               = in_size;
    addrB               = in_size+weight_size;
    addrO               = in_size+weight_size+bias_size;
}

void testbench::run_pv_before_ffn()
{
  
    BertMatrixVector(MAX_SEQ, D_MODEL, D_MODEL, query_w, block_in, query_b, query_o);
    BertMatrixVector(MAX_SEQ, D_MODEL, D_MODEL, key_w, block_in, key_b, key_o);
    BertMatrixVector(MAX_SEQ, D_MODEL, D_MODEL, value_w, block_in, value_b, value_o);
    BertAttention(MAX_SEQ, D_MODEL/N_HEAD, N_HEAD, SEN_MASK[sen], query_o, key_o, value_o, attn_o);
    BertMatrixVector(MAX_SEQ, D_MODEL, D_MODEL, dense_w, attn_o, dense_b, dense_o);
    BertResidualAddition(MAX_SEQ, D_MODEL, block_in, dense_o, rsd1_o);


    load_memory(rsd1_o, addrI, in_size);

#if ACCELERATED_SIM == 1
    // rearrange the weight matrix to smaller dimension, then load into the memory
    int original_out_dim = D_MODEL*4;
    float* ffn1_w_small = new float[weight_size];
    for (int i = 0; i < indim; ++i) {
        for (int j = 0; j < outdim; ++j) {
            ffn1_w_small[i*outdim + j] = ffn1_w[i*original_out_dim + j];
        }
    }
    load_memory(ffn1_w_small, addrW, weight_size);
#else
    load_memory(ffn1_w, addrW, weight_size);
#endif



    load_memory(ffn1_b, addrB, bias_size);

}

int testbench::run_pv_after_ffn()
{
// run Gelu
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
    return idx;
}

#else

//////////////////////////
///// second group ///////
//////////////////////////

void testbench::compile_config()
{
    cerr << "this is the value of the ACCELERATED_SIM: " << ACCELERATED_SIM << endl;
    #if defined(ARC_SMALL)
        cerr << "architecture: ARC_SMALL" << endl;
    #elif defined(ARC_FAST)
        cerr << "architecture: ARC_FAST" << endl;
    #endif 
#if ACCELERATED_SIM == 1
    seqlen  = 32;
    indim   = 512;
    outdim  = 32;
#else
    seqlen  = 32;
    indim   = 512;
    outdim  = 128;
#endif
    sw_in_size          = seqlen*indim;
    sw_weight_size      = indim*outdim;
    sw_out_size         = seqlen*outdim;
    in_size             = round_up(sw_in_size, DMA_WORD_PER_BEAT);
    weight_size         = round_up(sw_weight_size, DMA_WORD_PER_BEAT);
    out_size            = round_up(sw_out_size, DMA_WORD_PER_BEAT);
    bias_size           = round_up(outdim, DMA_WORD_PER_BEAT);
    addrI               = 0;
    addrW               = in_size;
    addrB               = in_size+weight_size;
    addrO               = in_size+weight_size+bias_size;
}

void testbench::run_pv_before_ffn()
{
  
    BertMatrixVector(MAX_SEQ, D_MODEL, D_MODEL, query_w, block_in, query_b, query_o);
    BertMatrixVector(MAX_SEQ, D_MODEL, D_MODEL, key_w, block_in, key_b, key_o);
    BertMatrixVector(MAX_SEQ, D_MODEL, D_MODEL, value_w, block_in, value_b, value_o);
    BertAttention(MAX_SEQ, D_MODEL/N_HEAD, N_HEAD, SEN_MASK[sen], query_o, key_o, value_o, attn_o);
    BertMatrixVector(MAX_SEQ, D_MODEL, D_MODEL, dense_w, attn_o, dense_b, dense_o);
    BertResidualAddition(MAX_SEQ, D_MODEL, block_in, dense_o, rsd1_o);

    BertMatrixVector(MAX_SEQ, D_MODEL, D_MODEL*4, ffn1_w, rsd1_o, ffn1_b, ffn1_o);
// run Gelu
    BertGelu(MAX_SEQ, D_MODEL*4, ffn1_o);
// run ffn2
    // BertMatrixVector(MAX_SEQ, D_MODEL*4, D_MODEL, ffn2_w, ffn1_o, ffn2_b, ffn2_o);


    load_memory(ffn1_o, addrI, in_size);

#if ACCELERATED_SIM == 1
    // rearrange the weight matrix to smaller dimension, then load into the memory
    int original_out_dim = D_MODEL;
    float* ffn2_w_small = new float[weight_size];
    for (int i = 0; i < indim; ++i) {
        for (int j = 0; j < outdim; ++j) {
            ffn2_w_small[i*outdim + j] = ffn2_w[i*original_out_dim + j];
        }
    }
    load_memory(ffn2_w_small, addrW, weight_size);
#else
    load_memory(ffn2_w, addrW, weight_size);
#endif

    load_memory(ffn2_b, addrB, bias_size);

}

int testbench::run_pv_after_ffn()
{

// run residual
    BertResidualAddition(MAX_SEQ, D_MODEL, rsd1_o, ffn2_o, rsd2_o);
// run pooler
    BertMatrixVector(1, D_MODEL, D_MODEL, pooler_w, rsd2_o, pooler_b, pooler_o);
    BertTanh(1, D_MODEL, pooler_o);
// run classifier
    BertMatrixVector(1, D_MODEL, 2, classi_w, pooler_o, classi_b, classifier_o);
// print the answer
    int idx = argmax(classifier_o, 2);
    return idx;
}

#endif // end of TARGET_LAYER



void testbench::validate_kernel(void)
{
    #if ACCELERATED_SIM == 1
        // std::string acc_file_name = "accelerated_data_layer" + std::to_string(TARGET_LAYER) + ".txt";
        std::string acc_file_name = "accelerated_data.txt";
        ofs.open(acc_file_name, std::ofstream::out);
    #endif
    
    acc_out = new ac_int<DATA_WIDTH, false>[out_size];

#if (DMA_WORD_PER_BEAT == 0)
    int offset = addrO * DMA_BEAT_PER_WORD;
    for (uint32_t i=0; i<out_size; i++){
        ac_int <DATA_WIDTH> data_bv;
        for (int j=0; j<DMA_BEAT_PER_WORD; j++){
            data_bv.set_slc(j*DMA_WIDTH, mem[offset + DMA_BEAT_PER_WORD*i + j]);
        }
        acc_out[i] = data_bv.to_int64();
    }
#else
    int offset = addrO/DMA_WORD_PER_BEAT;
    for (uint32_t i=0; i<out_size/DMA_WORD_PER_BEAT; i++){
        for (uint32_t j=0; j<DMA_WORD_PER_BEAT; j++){
            acc_out[i*DMA_WORD_PER_BEAT + j] = mem[offset+i].slc<DATA_WIDTH>(j*DATA_WIDTH);
        }
    }
#endif
    CCS_LOG("testbench dump memory completed" );

    int err = 0;
    float acc_result;
    
#if TARGET_LAYER == 1
    BertMatrixVector(MAX_SEQ, D_MODEL, D_MODEL*4, ffn1_w, rsd1_o, ffn1_b, ffn1_o);

#if ACCELERATED_SIM == 1
    float *golden_arr = new float[sw_out_size];
    for (int i = 0; i < seqlen; ++i) {
        for (int j = 0; j < outdim; ++j) {
            golden_arr[i*outdim + j] = ffn1_o[i*D_MODEL*4 + j];
        }
    }
#else
    float *golden_arr = ffn1_o;
#endif

#else

    ///// this is for the second group /////
    // BertMatrixVector(MAX_SEQ, D_MODEL, D_MODEL*4, ffn1_w, rsd1_o, ffn1_b, ffn1_o);
    BertMatrixVector(MAX_SEQ, D_MODEL*4, D_MODEL, ffn2_w, ffn1_o, ffn2_b, ffn2_o);

#if ACCELERATED_SIM == 1
    float *golden_arr = new float[sw_out_size];
    for (int i = 0; i < seqlen; ++i) {
        for (int j = 0; j < outdim; ++j) {
            golden_arr[i*outdim + j] = ffn2_o[i*D_MODEL + j];
        }
    }
#else
    float *golden_arr = ffn2_o;
#endif

#endif



    for (int i=0; i<sw_out_size; i++){
        FPDATA_WORD data_bv = acc_out[i];
        FPDATA data_fp;
    
    #ifdef FL_POINT
        int2f(data_bv, data_fp);
        acc_result = data_fp.to_ac_float().to_float();
    #else
        bv2fp(data_bv, data_fp);
        double data_double = data_fp.to_double();
        acc_result = (float) data_double;
    #endif
        float golden = golden_arr[i];

        if (golden != acc_result){
            float MSE = (golden - acc_result) * (golden - acc_result) / golden;
            if (MSE > ERR_THRESHOLD && abs(golden) > 0.002){
                fprintf(stderr, "MSE: %f \n", MSE);
                printf("Vector: output[%d] = %f (pv_golden: %f) \n", i, acc_result, golden_arr[i]);
                err += 1;
                golden_arr[i] = acc_result;
            }
        }
#if ACCELERATED_SIM == 1
            ofs << i << ": " << std::fixed << std::setprecision(3) << acc_result << std::endl;
#endif
    }

    if (err == 0){
        CCS_LOG("---------------------------------------");
        printf("  Validation succeeded for total %d!\n", sw_out_size);
        CCS_LOG("---------------------------------------");
    } else {
        CCS_LOG("---------------------------------------");
        printf("  Mismatch found! (err rate: %d / %d = %f%%)\n", err, sw_out_size, err/float(sw_out_size)*100);
        CCS_LOG("---------------------------------------");
    }
    CCS_LOG("testbench accelerator validation completed" );
}


char model_dir_local[] = "../../models/";
char input_dir_local[] = "../../data/";

void load_bert_tb(float* query_w, float* query_b, float* key_w, float* key_b, float* value_w, float* value_b, 
				float* dense_w, float* dense_b,float* ffn1_w, float* ffn1_b, float* ffn2_w, float* ffn2_b, 
				float* pooler_w, float* pooler_b, float* classi_w, float* classi_b
){
// query
	char query_w_bin[1024];
	sprintf(query_w_bin, "%s%s", model_dir_local, "query_w_t.bin");
	FILE* inFile_query_w = fopen(query_w_bin, "rb");
	if (inFile_query_w != NULL) {
		fread(query_w, sizeof(float), D_MODEL*D_MODEL, inFile_query_w);
		fclose(inFile_query_w);
	} else { 
		printf("   Can't find query weight\n");
	}
	char query_b_bin[1024];
	sprintf(query_b_bin, "%s%s", model_dir_local, "query_b.bin");
	FILE* inFile_query_b = fopen(query_b_bin, "rb");
	if (inFile_query_b != NULL) {
		fread(query_b, sizeof(float), D_MODEL, inFile_query_b);
		fclose(inFile_query_b);
	} else { 
		printf("   Can't find query bias\n");
	}

// key
	char key_w_bin[1024];
	sprintf(key_w_bin, "%s%s", model_dir_local, "key_w_t.bin");
	FILE* inFile_key_w = fopen(key_w_bin, "rb");
	if (inFile_key_w != NULL) {
		fread(key_w, sizeof(float), D_MODEL*D_MODEL, inFile_key_w);
		fclose(inFile_key_w);
	} else { 
		printf("   Can't find key weight\n");
	}
	char key_b_bin[1024];
	sprintf(key_b_bin, "%s%s", model_dir_local, "key_b.bin");
	FILE* inFile_key_b = fopen(key_b_bin, "rb");
	if (inFile_key_b != NULL) {
		fread(key_b, sizeof(float), D_MODEL, inFile_key_b);
		fclose(inFile_key_b);
	} else { 
		printf("   Can't find key bias\n");
	}

// value
	char value_w_bin[1024];
	sprintf(value_w_bin, "%s%s", model_dir_local, "value_w_t.bin");
	FILE* inFile_value_w = fopen(value_w_bin, "rb");
	if (inFile_value_w != NULL) {
		fread(value_w, sizeof(float), D_MODEL*D_MODEL, inFile_value_w);
		fclose(inFile_value_w);
	} else { 
		printf("   Can't find value weight\n");
	}
	char value_b_bin[1024];
	sprintf(value_b_bin, "%s%s", model_dir_local, "value_b.bin");
	FILE* inFile_value_b = fopen(value_b_bin, "rb");
	if (inFile_value_b != NULL) {
		fread(value_b, sizeof(float), D_MODEL, inFile_value_b);
		fclose(inFile_value_b);
	} else { 
		printf("   Can't find value bias\n");
	}

// dense
	char dense_w_bin[1024];
	sprintf(dense_w_bin, "%s%s", model_dir_local, "dense_w_t.bin");
	FILE* inFile_dense_w = fopen(dense_w_bin, "rb");
	if (inFile_dense_w != NULL) {
		fread(dense_w, sizeof(float), D_MODEL*D_MODEL, inFile_dense_w);
		fclose(inFile_dense_w);
	} else { 
		printf("   Can't find dense weight\n");
	}

	char dense_b_bin[1024];
	sprintf(dense_b_bin, "%s%s", model_dir_local, "dense_b.bin");
	FILE* inFile_dense_b = fopen(dense_b_bin, "rb");
	if (inFile_dense_b != NULL) {
		fread(dense_b, sizeof(float), D_MODEL, inFile_dense_b);
		fclose(inFile_dense_b);
	} else { 
		printf("   Can't find dense bias\n");
	}

// ffn1
	char ffn1_w_bin[1024];
	sprintf(ffn1_w_bin, "%s%s", model_dir_local, "ffn1_w_t.bin");
	FILE* inFile_ffn1_w = fopen(ffn1_w_bin, "rb");
	if (inFile_ffn1_w != NULL) {
		fread(ffn1_w, sizeof(float), D_MODEL*D_MODEL*4, inFile_ffn1_w);
		fclose(inFile_ffn1_w);
	} else { 
		printf("   Can't find ffn1 weight\n");
	}

	char ffn1_b_bin[1024];
	sprintf(ffn1_b_bin, "%s%s", model_dir_local, "ffn1_b.bin");
	FILE* inFile_ffn1_b = fopen(ffn1_b_bin, "rb");
	if (inFile_ffn1_b != NULL) {
		fread(ffn1_b, sizeof(float), D_MODEL*4, inFile_ffn1_b);
		fclose(inFile_ffn1_b);
	} else { 
		printf("   Can't find ffn1 bias\n");
	}

// ffn2
	char ffn2_w_bin[1024];
	sprintf(ffn2_w_bin, "%s%s", model_dir_local, "ffn2_w_t.bin");
	FILE* inFile_ffn2_w = fopen(ffn2_w_bin, "rb");
	if (inFile_ffn2_w != NULL) {
		fread(ffn2_w, sizeof(float), D_MODEL*D_MODEL*4, inFile_ffn2_w);
		fclose(inFile_ffn2_w);
	} else { 
		printf("   Can't find ffn2 weight\n");
	}

	char ffn2_b_bin[1024];
	sprintf(ffn2_b_bin, "%s%s", model_dir_local, "ffn2_b.bin");
	FILE* inFile_ffn2_b = fopen(ffn2_b_bin, "rb");
	if (inFile_ffn2_b != NULL) {
		fread(ffn2_b, sizeof(float), D_MODEL, inFile_ffn2_b);
		fclose(inFile_ffn2_b);
	} else { 
		printf("   Can't find ffn2 bias\n");
	}

// pooler
	char pooler_w_bin[1024];
	sprintf(pooler_w_bin, "%s%s", model_dir_local, "pooler_w_t.bin");
	FILE* inFile_pooler_w = fopen(pooler_w_bin, "rb");
	if (inFile_pooler_w != NULL) {
		fread(pooler_w, sizeof(float), D_MODEL*D_MODEL, inFile_pooler_w);
		fclose(inFile_pooler_w);
	} else { 
		printf("   Can't find pooler weight\n");
	}

	char pooler_b_bin[1024];
	sprintf(pooler_b_bin, "%s%s", model_dir_local, "pooler_b.bin");
	FILE* inFile_pooler_b = fopen(pooler_b_bin, "rb");
	if (inFile_pooler_b != NULL) {
		fread(pooler_b, sizeof(float), D_MODEL, inFile_pooler_b);
		fclose(inFile_pooler_b);
	} else { 
		printf("   Can't find pooler bias\n");
	}

// classifier
	char classi_w_bin[1024];
	sprintf(classi_w_bin, "%s%s", model_dir_local, "classifier_w_t.bin");
	FILE* inFile_classi_w = fopen(classi_w_bin, "rb");
	if (inFile_classi_w != NULL) {
		fread(classi_w, sizeof(float), D_MODEL*2, inFile_classi_w);
		fclose(inFile_classi_w);
	} else { 
		printf("   Can't find classifier weight\n");
	}

	char classi_b_bin[1024];
	sprintf(classi_b_bin, "%s%s", model_dir_local, "classifier_b.bin");
	FILE* inFile_classi_b = fopen(classi_b_bin, "rb");
	if (inFile_classi_b != NULL) {
		fread(classi_b, sizeof(float), 2, inFile_classi_b);
		fclose(inFile_classi_b);
	} else { 
		printf("   Can't find classifier bias\n");
	}
}

void load_block_in_tb(const char* sentence, float* block_in) {
// block in
	char block_in_bin[1024];
	sprintf(block_in_bin, "%s%s%s", input_dir_local, sentence, ".bin");
	FILE* inFile_block_i = fopen(block_in_bin, "rb");
	if (inFile_block_i != NULL) {
		fread(block_in, sizeof(float), D_MODEL*MAX_SEQ, inFile_block_i);
		fclose(inFile_block_i);
		// printf("   Load input for block %d len %d completed\n", lyr+1, D_MODEL*MAX_SEQ);
	} else { 
		printf("   Can't find input\n");
	}
}




void testbench::load_params() {

    query_w = (float *)calloc(D_MODEL*D_MODEL, sizeof(float));
    query_b = (float *)calloc(D_MODEL, sizeof(float));
    key_w = (float *)calloc(D_MODEL*D_MODEL, sizeof(float));
    key_b = (float *)calloc(D_MODEL, sizeof(float));
    value_w = (float *)calloc(D_MODEL*D_MODEL, sizeof(float));
    value_b = (float *)calloc(D_MODEL, sizeof(float));
    dense_w = (float *)calloc(D_MODEL*D_MODEL, sizeof(float));
    dense_b = (float *)calloc(D_MODEL, sizeof(float));
    ffn1_w = (float *)calloc(D_MODEL*D_MODEL*4, sizeof(float));
    ffn1_b = (float *)calloc(D_MODEL*4, sizeof(float));
    ffn2_w = (float *)calloc(D_MODEL*D_MODEL*4, sizeof(float));
    ffn2_b = (float *)calloc(D_MODEL, sizeof(float));
    pooler_w = (float *)calloc(D_MODEL*D_MODEL, sizeof(float));
    pooler_b = (float *)calloc(D_MODEL, sizeof(float));
    classi_w = (float *)calloc(D_MODEL*2, sizeof(float));
    classi_b = (float *)calloc(2, sizeof(float));
    block_in = (float *)calloc(MAX_SEQ*D_MODEL, sizeof(float));
    query_o = (float *)calloc(MAX_SEQ*D_MODEL, sizeof(float));
    key_o = (float *)calloc(MAX_SEQ*D_MODEL, sizeof(float));
    value_o = (float *)calloc(MAX_SEQ*D_MODEL, sizeof(float));
    attn_o = (float *)calloc(MAX_SEQ*D_MODEL, sizeof(float));
    dense_o = (float *)calloc(MAX_SEQ*D_MODEL, sizeof(float));
    rsd1_o = (float *)calloc(MAX_SEQ*D_MODEL, sizeof(float));
    ffn1_o = (float *)calloc(MAX_SEQ*D_MODEL*4, sizeof(float));
    ffn2_o = (float *)calloc(MAX_SEQ*D_MODEL, sizeof(float));
    rsd2_o = (float *)calloc(MAX_SEQ*D_MODEL, sizeof(float));
    pooler_o = (float *)calloc(D_MODEL, sizeof(float));
    classifier_o = (float *)calloc(2, sizeof(float));

    load_bert_tb(query_w, query_b, key_w, key_b, value_w, value_b,
                dense_w, dense_b, ffn1_w, ffn1_b, ffn2_w, ffn2_b, 
                pooler_w, pooler_b, classi_w, classi_b);

    const char* sentence = getenv("SENTENCE");
    sen = sentence_index(sentence);
    if (sen < 0) {
        fprintf(stderr, "ERROR: SENTENCE='%s' not found in SEN_TYPE list\n", sentence);
        fprintf(stderr, "       Valid options: ");
        for (int i = 0; i < 10; ++i) fprintf(stderr, "%s%s", SEN_TYPE[i], (i==9?"\n":", "));
        sc_stop();
        return;
    }

    cerr << "Loading input for sentence: " << sentence << endl;
    cerr << "Sentence index: " << sen << endl;

    load_block_in_tb(sentence, block_in);


}

void testbench::kernel_processing()
{
    conf_info.Reset();
    wait();

    CCS_LOG("=== TEST BEGIN ===");    
    compile_config();
    CCS_LOG("testbench setup configuration completed");
    CCS_LOG("testbench setup memory completed");

    // load the bert
    load_params();

    // run pv before gelu
    run_pv_before_ffn();

    sc_time start_time = sc_time_stamp();
    conf_info_t config = load_config();
    conf_info.Push(config);
    CCS_LOG("testbench push configuration info completed");

    wait();
    do { wait(); } while (!acc_done.read());
    sc_time end_time = sc_time_stamp();


    validate_kernel();

    // run the rest of bert
    int idx = run_pv_after_ffn();


    // Validate hardware output against PV GELU output (ffn1_o)
    print_label(idx, SEN[sen]);

    sc_time elapsed_time = end_time - start_time;
    CCS_LOG("testbench operating accelerator completed");
    std::cout << "--------------------------------------- Elapsed time: " << elapsed_time << " ---------------------------------------" << std::endl;

    sc_stop();
}
