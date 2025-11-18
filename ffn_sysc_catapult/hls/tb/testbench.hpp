//Copyright (c) 2011-2024 Columbia University, System Level Design Group
//SPDX-License-Identifier: Apache-2.0

#ifndef __TESTBENCH_HPP__
#define __TESTBENCH_HPP__

#pragma once

#include <systemc.h>
#include "ffn_conf_info.hpp"
#include "ffn_specs.hpp"
#include "ffn_data_types.hpp"
#include "esp_dma_info_sysc.hpp"
#include "../inc/core/systems/esp_dma_controller.hpp"

#define __round_mask(x, y) ((y)-1)
#define round_up(x, y) ((((x)-1) | __round_mask(x, y))+1)

SC_MODULE(testbench)
{
    sc_in<bool> CCS_INIT_S1(clk);
    sc_in<bool> CCS_INIT_S1(rst_bar);

    Connections::Out<conf_info_t>        CCS_INIT_S1(conf_info);
    Connections::Out<ac_int<DMA_WIDTH>>       CCS_INIT_S1(dma_read_chnl);
    Connections::In<ac_int<DMA_WIDTH>>        CCS_INIT_S1(dma_write_chnl);
    Connections::In<dma_info_t >        CCS_INIT_S1(dma_read_ctrl);
    Connections::In<dma_info_t >        CCS_INIT_S1(dma_write_ctrl);
    sc_in<bool>     CCS_INIT_S1(acc_done);

    sc_signal<bool> acc_rst;

    // Shared memory buffer model
    ac_int<DMA_WIDTH> *mem;

    // DMA controller instace
    esp_dma_controller<DMA_WIDTH, MEM_SIZE > *dmac;

    // SC_CTOR(testbench) {
    SC_HAS_PROCESS(testbench);
    testbench(const sc_module_name& name):
        sc_module(name)
        , mem(new ac_int<DMA_WIDTH>[MEM_SIZE])
        , dmac(new esp_dma_controller<DMA_WIDTH, MEM_SIZE>("dma-controller", mem))
    {

        SC_THREAD(kernel_processing);
        sensitive << clk.pos();
        async_reset_signal_is(rst_bar, false);

        dmac->clk(clk);
        dmac->rst(rst_bar);
        dmac->dma_read_ctrl(dma_read_ctrl);
        dmac->dma_read_chnl(dma_read_chnl);
        dmac->dma_write_ctrl(dma_write_ctrl);
        dmac->dma_write_chnl(dma_write_chnl);
        dmac->acc_done(acc_done);
        dmac->acc_rst(acc_rst);

        /* <<--params-default-->> */
        // BERT-tiny
        // seqlen  = 32;
        // indim   = 128;
        // outdim  = 512;

    }

    void kernel_processing(void);

    conf_info_t load_config(void);
    void compile_config(void);
    void load_memory(float *file_arr, uint32_t base_addr, uint32_t size);
    void generate_data(void);
    void generate_params(void);
    void validate_kernel(void);

    // Accelerator-specific data
    int sen; // Sentence index for PV selection
    /* <<--params-->> */
    uint32_t seqlen;
    uint32_t indim;
    uint32_t outdim;
    uint32_t addrI;
    uint32_t addrW;
    uint32_t addrB;
    uint32_t addrO;
    uint32_t sw_in_size, sw_weight_size, sw_out_size;
    uint32_t in_size, weight_size, out_size;
    uint32_t bias_size;

    float* in;
    float* weight;
    ac_int<DATA_WIDTH,false> *acc_out ;
    // float *golden_arr;

    /////// bert related params
    float* query_w;
	float* query_b;
	float* key_w;
	float* key_b;
	float* value_w;
	float* value_b;
	float* dense_w;
	float* dense_b;
	float* ffn1_w;
	float* ffn1_b;
	float* ffn2_w;
	float* ffn2_b;
	float* pooler_w;
	float* pooler_b;
	float* classi_w;
	float* classi_b;
    float* block_in;
    float* query_o;
    float* key_o;
    float* value_o;
    float* attn_o;
    float* dense_o;
    float* rsd1_o;
    float* ffn1_o;
    float* ffn2_o;
    float* rsd2_o;
    float* pooler_o;
    float* classifier_o;


    void run_pv_before_ffn();
    int run_pv_after_ffn();
    void load_params();
};

#endif

