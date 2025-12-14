// Implementation of AccController with double buffering and decoupled execution.
// Optimization strategy references the DECADES architecture for high throughput.

#ifndef __FFN_CTRL_HPP__
#define __FFN_CTRL_HPP__

#pragma once

#include <systemc.h>
#include <nvhls_int.h>
#include <nvhls_connections.h>
#include <string>
#include <mc_scverify.h>

#include "ac_shared_bank_array.h"

#include "../inc/ffn_data_types.hpp"
#include "../inc/ffn_specs.hpp"
#include "../inc/ffn_conf_info.hpp"

#ifndef round_up
#define round_up(x, y) ((((x)-1) | __round_mask(x, y))+1)
#endif

#ifndef __round_mask
#define __round_mask(x, y) ((y)-1)
#endif

#if (DMA_WORD_PER_BEAT == 0)
#error "DMA_WORD_PER_BEAT == 0 configuration is not supported by AccController"
#endif

SC_MODULE(AccController)
{
    public:
    sc_in<bool>     CCS_INIT_S1(clk);
    sc_in<bool>     CCS_INIT_S1(rst);

    sc_out<bool>    CCS_INIT_S1(acc_done);
    Connections::In< ac_int<DMA_WIDTH> >    CCS_INIT_S1(dma_read_chnl);
    Connections::Out< ac_int<DMA_WIDTH> >   CCS_INIT_S1(dma_write_chnl);
    Connections::Out<dma_info_t>            CCS_INIT_S1(dma_read_ctrl);
    Connections::Out<dma_info_t>            CCS_INIT_S1(dma_write_ctrl);

    Connections::In<conf_info_t >           CCS_INIT_S1(conf_info_dma2acc);
    Connections::In<conf_info_t >           CCS_INIT_S1(conf_info_plm2vec);
    Connections::In<conf_info_t >           CCS_INIT_S1(conf_info_acc2dma);
    
    Connections::In <int32_t>               CCS_INIT_S1(seq_iter_itcn);
    Connections::In <int32_t>               CCS_INIT_S1(seq_cache_itcn);
    Connections::In <int32_t>               CCS_INIT_S1(w_iter_itcn);

    Connections::Out <array_t<FPDATA, VEC_LEN>> CCS_INIT_S1(vec_in_itcn);
    Connections::Out <array_t<FPDATA, VEC_LEN>> CCS_INIT_S1(vec_weight_itcn);
    Connections::Out <array_t<FPDATA, VEC_LEN>> CCS_INIT_S1(vec_psum_itcn);
    Connections::In <array_t<FPDATA, VEC_LEN>>  CCS_INIT_S1(vec_out_itcn);
    Connections::Out <array_t<FPDATA, VEC_LEN>> CCS_INIT_S1(data_in_itcn);
    Connections::In <array_t<FPDATA, VEC_LEN>>  CCS_INIT_S1(data_out_itcn);
    Connections::Out <array_t<FPDATA, VEC_LEN>> CCS_INIT_S1(data_bias_itcn);

    Connections::Combinational <int32_t>    CCS_INIT_S1(seq_iter_itcn_dma2acc);
    Connections::Combinational <int32_t>    CCS_INIT_S1(seq_cache_itcn_dma2acc);
    Connections::Combinational <int32_t>    CCS_INIT_S1(w_iter_itcn_dma2acc);

    Connections::Combinational <int32_t>    CCS_INIT_S1(seq_iter_itcn_plm2vec);
    Connections::Combinational <int32_t>    CCS_INIT_S1(seq_cache_itcn_plm2vec);
    Connections::Combinational <int32_t>    CCS_INIT_S1(w_iter_itcn_plm2vec);
    
    Connections::Combinational <int32_t>    CCS_INIT_S1(seq_iter_itcn_acc2dma);
    Connections::Combinational <int32_t>    CCS_INIT_S1(seq_cache_itcn_acc2dma);
    Connections::Combinational <int32_t>    CCS_INIT_S1(w_iter_itcn_acc2dma);

    // Sync channels for process handshaking
    Connections::Combinational<int32_t> ld2com_sync;
    Connections::Combinational<int32_t> com2st_sync;
    Connections::Combinational<int32_t> st2ld_sync;

#if defined(ARC_FAST)
    // Fast architecture: Ping-pong buffers
    ac_shared_bank_array_2D<DATA_TYPE, bks, ebks> plm_in_ping;
    ac_shared_bank_array_2D<DATA_TYPE, bks, ebks> plm_weight_ping;
    ac_shared_bank_array_2D<DATA_TYPE, bks, ebks> plm_bias_ping;
    ac_shared_bank_array_2D<DATA_TYPE, bks, ebks> plm_out_ping;

    ac_shared_bank_array_2D<DATA_TYPE, bks, ebks> plm_in_pong;
    ac_shared_bank_array_2D<DATA_TYPE, bks, ebks> plm_weight_pong;
    ac_shared_bank_array_2D<DATA_TYPE, bks, ebks> plm_bias_pong;
    ac_shared_bank_array_2D<DATA_TYPE, bks, ebks> plm_out_pong;
#else
    // Small architecture: Single buffer to minimize area
    ac_shared_bank_array_2D<DATA_TYPE, bks, ebks> plm_in;
    ac_shared_bank_array_2D<DATA_TYPE, bks, ebks> plm_weight;
    ac_shared_bank_array_2D<DATA_TYPE, bks, ebks> plm_bias;
    ac_shared_bank_array_2D<DATA_TYPE, bks, ebks> plm_out;
#endif

    SC_HAS_PROCESS(AccController);
    AccController(const sc_module_name& name) : 
        sc_module(name),
        acc_done("acc_done"),
        dma_read_chnl("dma_read_chnl"),
        dma_write_chnl("dma_write_chnl"),
        dma_read_ctrl("dma_read_ctrl"),
        dma_write_ctrl("dma_write_ctrl"),
        conf_info_dma2acc("conf_info_dma2acc"),
        conf_info_plm2vec("conf_info_plm2vec"),
        conf_info_acc2dma("conf_info_acc2dma"),
        seq_iter_itcn("seq_iter_itcn"),
        seq_cache_itcn("seq_cache_itcn"),
        w_iter_itcn("w_iter_itcn"),
        vec_in_itcn("vec_in_itcn"),
        vec_weight_itcn("vec_weight_itcn"),
        vec_out_itcn("vec_out_itcn")
    {
        SC_THREAD(cfg);
        sensitive << clk.pos();
        async_reset_signal_is(rst, false);
     
        SC_THREAD(dma2acc);
        sensitive << clk.pos();
        async_reset_signal_is(rst, false);

        SC_THREAD(plm2vec);
        sensitive << clk.pos();
        async_reset_signal_is(rst, false);
        
        SC_THREAD(acc2dma);
        sensitive << clk.pos();
        async_reset_signal_is(rst, false);
    }

    void cfg(){
        seq_iter_itcn.Reset();
        seq_cache_itcn.Reset();
        w_iter_itcn.Reset();
        seq_iter_itcn_dma2acc.ResetWrite();
        seq_cache_itcn_dma2acc.ResetWrite();
        w_iter_itcn_dma2acc.ResetWrite();
        seq_iter_itcn_plm2vec.ResetWrite();
        seq_cache_itcn_plm2vec.ResetWrite();
        w_iter_itcn_plm2vec.ResetWrite();
        seq_iter_itcn_acc2dma.ResetWrite();
        seq_cache_itcn_acc2dma.ResetWrite();
        w_iter_itcn_acc2dma.ResetWrite();

        wait();
        while(1)
        {
            int32_t seq_iter = seq_iter_itcn.Pop();
            int32_t seq_cache = seq_cache_itcn.Pop();
            int32_t w_iter = w_iter_itcn.Pop();

            seq_iter_itcn_dma2acc.Push(seq_iter);
            seq_cache_itcn_dma2acc.Push(seq_cache);
            w_iter_itcn_dma2acc.Push(w_iter);

            seq_iter_itcn_plm2vec.Push(seq_iter);
            seq_cache_itcn_plm2vec.Push(seq_cache);
            w_iter_itcn_plm2vec.Push(w_iter);

            seq_iter_itcn_acc2dma.Push(seq_iter);
            seq_cache_itcn_acc2dma.Push(seq_cache);
            w_iter_itcn_acc2dma.Push(w_iter);
        }
    }

    void data_convert_NVint(DATA_TYPE in, FPDATA_WORD &out) {
        for (int i=0; i<FPDATA_WL; i++) {
            out[i] = in[i];
        }
    }

#if defined(ARC_FAST)
    // =========================================================================
    // FAST Architecture: Pipelined with Ping-Pong
    // =========================================================================

    void dma2acc() {
        dma_read_chnl.Reset(); dma_read_ctrl.Reset(); conf_info_dma2acc.Reset();
        seq_iter_itcn_dma2acc.ResetRead(); seq_cache_itcn_dma2acc.ResetRead(); w_iter_itcn_dma2acc.ResetRead();
        ld2com_sync.ResetWrite(); st2ld_sync.ResetRead();
        wait();
        while(1) {
            conf_info_t conf_info = conf_info_dma2acc.Pop();
            int32_t indim = conf_info.indim, outdim = conf_info.outdim;
            int32_t addrI = conf_info.addrI, addrW = conf_info.addrW, addrB = conf_info.addrB;
            int32_t seq_iter = seq_iter_itcn_dma2acc.Pop(), seq_cache = seq_cache_itcn_dma2acc.Pop(), w_iter = w_iter_itcn_dma2acc.Pop();            

            // Optimized ping-pong with non-blocking credit consumption
            int iteration_count = 0;
            int credits_in_flight = 0;
            const int MAX_CREDITS = 2;

            for (int32_t seq_i=0; seq_i<seq_iter; seq_i++) {
                int32_t in_ld_offset = addrI + indim*seq_cache*seq_i;
                int32_t in_ld_size = indim*seq_cache;
                bool use_ping = (seq_i % 2 == 0);
                
                // Load input only once per seq_i (reuse for all w_i)
                if (use_ping) load(in_ld_offset, in_ld_size, 0, plm_in_ping);
                else          load(in_ld_offset, in_ld_size, 0, plm_in_pong);

                for (int32_t w_i=0; w_i<w_iter; w_i++){
                    // Non-blocking: only pop credit if we have 2 in flight
                    if (credits_in_flight >= MAX_CREDITS) {
                        st2ld_sync.Pop();
                        credits_in_flight--;
                    }
                    
                    int32_t bias_ld_offset = addrB + VEC_LEN * w_i;
                    int32_t bias_ld_size = VEC_LEN;
                    if (use_ping) load(bias_ld_offset, bias_ld_size, 0, plm_bias_ping);
                    else          load(bias_ld_offset, bias_ld_size, 0, plm_bias_pong);

                    // Unroll weight loop for better throughput
                    #pragma HLS UNROLL FACTOR=2
                    for (int32_t w_ii=0; w_ii<indim; w_ii++){
                        int32_t w_ld_offset = addrW + VEC_LEN * w_i + outdim * w_ii;
                        int32_t w_ld_size = VEC_LEN;
                        if (use_ping) load(w_ld_offset, w_ld_size, w_ii, plm_weight_ping);           
                        else          load(w_ld_offset, w_ld_size, w_ii, plm_weight_pong);           
                    }
                    ld2com_sync.Push(1);  
                    credits_in_flight++;
                    iteration_count++;
                }
            }
            // Drain remaining credits at end
            while (credits_in_flight > 0) {
                st2ld_sync.Pop();
                credits_in_flight--;
            }
        }   
    }

    void plm2vec() {
        conf_info_plm2vec.Reset(); seq_iter_itcn_plm2vec.ResetRead(); seq_cache_itcn_plm2vec.ResetRead(); w_iter_itcn_plm2vec.ResetRead();
        ld2com_sync.ResetRead(); com2st_sync.ResetWrite();
        vec_in_itcn.Reset(); vec_weight_itcn.Reset(); vec_psum_itcn.Reset(); vec_out_itcn.Reset();
        data_in_itcn.Reset(); data_out_itcn.Reset(); data_bias_itcn.Reset();
        wait();
        while(1) {
            conf_info_t conf_info = conf_info_plm2vec.Pop();
            int32_t indim = conf_info.indim;
            int32_t seq_iter = seq_iter_itcn_plm2vec.Pop(), seq_cache = seq_cache_itcn_plm2vec.Pop(), w_iter = w_iter_itcn_plm2vec.Pop();         

            for (int32_t seq_i=0; seq_i<seq_iter; seq_i++) {
                for (int32_t w_i=0; w_i<w_iter; w_i++) {
                    ld2com_sync.Pop(); 
                    bool use_ping = (seq_i % 2 == 0);
                    
                    for (int32_t seq_ii=0; seq_ii<seq_cache; seq_ii++) {
                        FPDATA_WORD in_word_cache, weight_word_cache[VEC_LEN], bias_word_cache[VEC_LEN];
                        FPDATA in_scalar_cache, weight_cache[VEC_LEN], bias_cache[VEC_LEN];
                        array_t<FPDATA, VEC_LEN> in_buffer, weight_buffer, psum_buffer, res_buffer;

                        // Pre-compute bias for this seq_ii once
                        for (int vec=0; vec<VEC_LEN; vec++){
                            if (use_ping) data_convert_NVint(plm_bias_ping[vec][0], bias_word_cache[vec]);
                            else          data_convert_NVint(plm_bias_pong[vec][0], bias_word_cache[vec]);
                            #ifdef FL_POINT
                                int2f(bias_word_cache[vec], bias_cache[vec]);
                            #else
                                int2fx(bias_word_cache[vec], bias_cache[vec]);
                            #endif
                        }

                        // Pipelined weight computation loop
                        #pragma HLS PIPELINE II=1
                        #pragma HLS LOOP_FLATTEN
                        for (int32_t w_ii=0; w_ii<indim; w_ii++) {
                            // Manage partial sums
                            for (int vec=0; vec<VEC_LEN; vec++) {
                                psum_buffer.data[vec] = (w_ii > 0) ? res_buffer.data[vec] : FPDATA(0);
                            }
                            
                            // Load and convert input (scalar broadcasted)
                            int in_addr = (indim*seq_ii + w_ii) / VEC_LEN;
                            int in_bank = (w_ii % VEC_LEN);

                            if (use_ping) data_convert_NVint(plm_in_ping[in_bank][in_addr], in_word_cache);
                            else          data_convert_NVint(plm_in_pong[in_bank][in_addr], in_word_cache);

                            #ifdef FL_POINT
                                int2f(in_word_cache, in_scalar_cache);
                            #else
                                int2fx(in_word_cache, in_scalar_cache);
                            #endif

                            // Fully unroll weight vector loop for II=1
                            #pragma HLS UNROLL
                            for (int vec=0; vec<VEC_LEN; vec++) {
                                in_buffer.data[vec] = in_scalar_cache;
                                int w_addr = w_ii;
                                int w_bank = vec;
                                if (use_ping) data_convert_NVint(plm_weight_ping[w_bank][w_addr], weight_word_cache[vec]);
                                else          data_convert_NVint(plm_weight_pong[w_bank][w_addr], weight_word_cache[vec]);
                                #ifdef FL_POINT
                                    int2f(weight_word_cache[vec], weight_cache[vec]);
                                #else
                                    int2fx(weight_word_cache[vec], weight_cache[vec]);
                                #endif
                                weight_buffer.data[vec] = weight_cache[vec];
                            }
                            vec_in_itcn.Push(in_buffer); vec_weight_itcn.Push(weight_buffer); vec_psum_itcn.Push(psum_buffer);
                            res_buffer = vec_out_itcn.Pop();
                        }
                        
                        // Write output with pre-computed bias
                        array_t<FPDATA, VEC_LEN> data_in_buffer = res_buffer, data_bias_in_buffer;
                        for (int vec=0; vec<VEC_LEN; vec++) {
                            data_bias_in_buffer.data[vec] = bias_cache[vec];
                        }
                        data_in_itcn.Push(data_in_buffer); data_bias_itcn.Push(data_bias_in_buffer);
                        array_t<FPDATA, VEC_LEN> data_out_buffer = data_out_itcn.Pop();

                        // Store output fully unrolled
                        #pragma HLS UNROLL
                        FPDATA_WORD out_word[VEC_LEN]; FPDATA out[VEC_LEN];
                        for (int vec=0; vec<VEC_LEN; vec++) {
                            out[vec] = data_out_buffer.data[vec];
                            #ifdef FL_POINT
                            f2int(out[vec], out_word[vec]);
                            #else
                            fx2int(out[vec], out_word[vec]);
                            #endif
                            int out_addr = seq_ii, out_bank = vec;
                            if (use_ping) plm_out_ping[out_bank][out_addr] = out_word[vec];
                            else          plm_out_pong[out_bank][out_addr] = out_word[vec];
                        }
                    }
                    com2st_sync.Push(1);
                }
            }
        }
    }

    void acc2dma() {
        dma_write_chnl.Reset(); dma_write_ctrl.Reset(); conf_info_acc2dma.Reset();
        seq_iter_itcn_acc2dma.ResetRead(); seq_cache_itcn_acc2dma.ResetRead(); w_iter_itcn_acc2dma.ResetRead();
        com2st_sync.ResetRead(); st2ld_sync.ResetWrite();
        acc_done.write(false);
        wait();
        while (1) {
            conf_info_t conf_info = conf_info_acc2dma.Pop();
            int32_t outdim = conf_info.outdim, addrO = conf_info.addrO;
            int32_t seq_iter = seq_iter_itcn_acc2dma.Pop(), seq_cache = seq_cache_itcn_acc2dma.Pop(), w_iter = w_iter_itcn_acc2dma.Pop();        

            for (int32_t seq_i=0; seq_i<seq_iter; seq_i++) {
                for (int32_t w_i=0; w_i<w_iter; w_i++) {
                    com2st_sync.Pop();
                    bool use_ping = (seq_i % 2 == 0);
                    
                    #pragma HLS PIPELINE II=1
                    for (int32_t seq_ii=0; seq_ii<seq_cache; seq_ii++) {
                        int32_t st_offset = addrO + outdim*seq_cache*seq_i + VEC_LEN*w_i + outdim*seq_ii;
                        int32_t st_sz = VEC_LEN;
                        if (use_ping) store(st_offset, st_sz, seq_ii, plm_out_ping);
                        else          store(st_offset, st_sz, seq_ii, plm_out_pong);
                    }
                    st2ld_sync.Push(1); 
                }
            }
            acc_done.write(true); wait(); acc_done.write(false);
        }
    }

#else
    // =========================================================================
    // SMALL Architecture: Single buffer, minimal resource usage
    // =========================================================================

    void dma2acc() {
        dma_read_chnl.Reset(); dma_read_ctrl.Reset(); conf_info_dma2acc.Reset();
        seq_iter_itcn_dma2acc.ResetRead(); seq_cache_itcn_dma2acc.ResetRead(); w_iter_itcn_dma2acc.ResetRead();
        ld2com_sync.ResetWrite(); st2ld_sync.ResetRead();
        wait();
        while(1) {
            conf_info_t conf_info = conf_info_dma2acc.Pop();
            int32_t indim = conf_info.indim, outdim = conf_info.outdim;
            int32_t addrI = conf_info.addrI, addrW = conf_info.addrW, addrB = conf_info.addrB;
            int32_t seq_iter = seq_iter_itcn_dma2acc.Pop(), seq_cache = seq_cache_itcn_dma2acc.Pop(), w_iter = w_iter_itcn_dma2acc.Pop();            

            for (int32_t seq_i=0; seq_i<seq_iter; seq_i++) {
                int32_t in_ld_offset = addrI + indim*seq_cache*seq_i;
                int32_t in_ld_size = indim*seq_cache;
                load(in_ld_offset, in_ld_size, 0, plm_in);

                for (int32_t w_i=0; w_i<w_iter; w_i++){
                    int32_t bias_ld_offset = addrB + VEC_LEN * w_i;
                    int32_t bias_ld_size = VEC_LEN;
                    load(bias_ld_offset, bias_ld_size, 0, plm_bias);

                    for (int32_t w_ii=0; w_ii<indim; w_ii++){
                        int32_t w_ld_offset = addrW + VEC_LEN * w_i + outdim * w_ii;
                        int32_t w_ld_size = VEC_LEN;
                        load(w_ld_offset, w_ld_size, w_ii, plm_weight);           
                    }
                    ld2com_sync.Push(1);  
                    st2ld_sync.Pop(); 
                }
            }
        }   
    }

    void plm2vec() {
        conf_info_plm2vec.Reset(); seq_iter_itcn_plm2vec.ResetRead(); seq_cache_itcn_plm2vec.ResetRead(); w_iter_itcn_plm2vec.ResetRead();
        ld2com_sync.ResetRead(); com2st_sync.ResetWrite();
        vec_in_itcn.Reset(); vec_weight_itcn.Reset(); vec_psum_itcn.Reset(); vec_out_itcn.Reset();
        data_in_itcn.Reset(); data_out_itcn.Reset(); data_bias_itcn.Reset();
        wait();
        while(1) {
            conf_info_t conf_info = conf_info_plm2vec.Pop();
            int32_t indim = conf_info.indim;
            int32_t seq_iter = seq_iter_itcn_plm2vec.Pop(), seq_cache = seq_cache_itcn_plm2vec.Pop(), w_iter = w_iter_itcn_plm2vec.Pop();         

            for (int32_t seq_i=0; seq_i<seq_iter; seq_i++) {
                for (int32_t w_i=0; w_i<w_iter; w_i++) {
                    ld2com_sync.Pop();
                    for (int32_t seq_ii=0; seq_ii<seq_cache; seq_ii++) {
                        FPDATA_WORD in_word[VEC_LEN], weight_word[VEC_LEN], bias_word[VEC_LEN];
                        FPDATA in[VEC_LEN], weight[VEC_LEN], bias[VEC_LEN];
                        array_t<FPDATA, VEC_LEN> in_buffer, weight_buffer, psum_buffer, res_buffer;

                        for (int32_t w_ii=0; w_ii<indim; w_ii++) {
                            if (w_ii > 0) {
                                for (int vec=0; vec<VEC_LEN; vec++) psum_buffer.data[vec] = res_buffer.data[vec];
                            } else {
                                for (int vec=0; vec<VEC_LEN; vec++) psum_buffer.data[vec] = FPDATA(0);
                            }
                            int in_addr = (indim*seq_ii + w_ii) / VEC_LEN;
                            int in_bank = (w_ii % VEC_LEN);
                            data_convert_NVint(plm_in[in_bank][in_addr], in_word[0]);
                            #ifdef FL_POINT
                                int2f(in_word[0], in[0]);
                            #else
                                int2fx(in_word[0], in[0]);
                            #endif
                            FPDATA in_scalar = in[0];

                            for (int vec=0; vec<VEC_LEN; vec++) {
                                in_buffer.data[vec] = in_scalar;
                                int w_addr = w_ii;
                                int w_bank = vec;
                                data_convert_NVint(plm_weight[w_bank][w_addr], weight_word[vec]);
                                #ifdef FL_POINT
                                    int2f(weight_word[vec], weight[vec]);
                                #else
                                    int2fx(weight_word[vec], weight[vec]);
                                #endif
                                weight_buffer.data[vec] = weight[vec];
                            }
                            vec_in_itcn.Push(in_buffer); vec_weight_itcn.Push(weight_buffer); vec_psum_itcn.Push(psum_buffer);
                            res_buffer = vec_out_itcn.Pop();
                        }
                        
                        array_t<FPDATA, VEC_LEN> data_in_buffer = res_buffer, data_bias_in_buffer;
                        for (int vec=0; vec<VEC_LEN; vec++){
                            data_convert_NVint(plm_bias[vec][0], bias_word[vec]);
                            #ifdef FL_POINT
                                int2f(bias_word[vec], bias[vec]);
                            #else
                                int2fx(bias_word[vec], bias[vec]);
                            #endif
                            data_bias_in_buffer.data[vec] = bias[vec];
                        }
                        data_in_itcn.Push(data_in_buffer); data_bias_itcn.Push(data_bias_in_buffer);
                        array_t<FPDATA, VEC_LEN> data_out_buffer = data_out_itcn.Pop();

                        FPDATA_WORD out_word[VEC_LEN]; FPDATA out[VEC_LEN];
                        for (int vec=0; vec<VEC_LEN; vec++) {
                            out[vec] = data_out_buffer.data[vec];
                            #ifdef FL_POINT
                            f2int(out[vec], out_word[vec]);
                            #else
                            fx2int(out[vec], out_word[vec]);
                            #endif
                            int out_addr = seq_ii, out_bank = vec;
                            plm_out[out_bank][out_addr] = out_word[vec];
                        }
                    }
                    com2st_sync.Push(1);
                }
            }
        }
    }

    void acc2dma() {
        dma_write_chnl.Reset(); dma_write_ctrl.Reset(); conf_info_acc2dma.Reset();
        seq_iter_itcn_acc2dma.ResetRead(); seq_cache_itcn_acc2dma.ResetRead(); w_iter_itcn_acc2dma.ResetRead();
        com2st_sync.ResetRead(); st2ld_sync.ResetWrite();
        acc_done.write(false);
        wait();
        while (1) {
            conf_info_t conf_info = conf_info_acc2dma.Pop();
            int32_t outdim = conf_info.outdim, addrO = conf_info.addrO;
            int32_t seq_iter = seq_iter_itcn_acc2dma.Pop(), seq_cache = seq_cache_itcn_acc2dma.Pop(), w_iter = w_iter_itcn_acc2dma.Pop();        

            for (int32_t seq_i=0; seq_i<seq_iter; seq_i++) {
                for (int32_t w_i=0; w_i<w_iter; w_i++) {
                    com2st_sync.Pop();
                    for (int32_t seq_ii=0; seq_ii<seq_cache; seq_ii++) {
                        int32_t st_offset = addrO + outdim*seq_cache*seq_i + VEC_LEN*w_i + outdim*seq_ii;
                        int32_t st_sz = VEC_LEN;
                        store(st_offset, st_sz, seq_ii, plm_out);
                    }
                    st2ld_sync.Push(1); 
                }
            }
            acc_done.write(true); wait(); acc_done.write(false);
        }
    }
#endif

    template <typename SharedArray2D>
    void load(uint32_t dma_ld_addr, uint32_t dma_ld_size, uint32_t plm_addr, SharedArray2D& plm)
    {
        uint32_t len = round_up(dma_ld_size, DMA_WORD_PER_BEAT);

        uint32_t dma_addr = dma_ld_addr;
        uint32_t plm_row  = plm_addr;

        for (uint32_t rem = len; rem > 0; ) {
            uint32_t beats = (rem < PLM_WORD) ? rem : PLM_WORD;

            dma_info_t dma_info(dma_addr / DMA_WORD_PER_BEAT, beats / DMA_WORD_PER_BEAT, DMA_SIZE, 0);
            dma_read_ctrl.Push(dma_info);

            for (uint32_t i = 0; i < beats; i += VEC_LEN) {
                for (uint32_t j = 0; j < VEC_DMA_RATIO; j++) {
                    ac_int<DMA_WIDTH> data_dma = dma_read_chnl.Pop();
                    for (uint32_t k = 0; k < DMA_WORD_PER_BEAT; k++) {
                        uint32_t bank = DMA_WORD_PER_BEAT * j + k;
                        uint32_t addr = plm_row + i / VEC_LEN;
                        plm[bank][addr] = data_dma.slc<DATA_WIDTH>(k * DATA_WIDTH);
                    }
                }
            }

            rem      -= beats;
            dma_addr += beats;
            plm_row  += beats / VEC_LEN;
        }
        wait();
    }

    template <typename SharedArray2D>
    void store(uint32_t dma_st_addr, uint32_t dma_st_size, uint32_t plm_addr, SharedArray2D& plm)
    {
        uint32_t len = round_up(dma_st_size, DMA_WORD_PER_BEAT);

        uint32_t dma_addr = dma_st_addr;
        uint32_t plm_row  = plm_addr;

        for (uint32_t rem = len; rem > 0; )
        {
            uint32_t beats = (rem < PLM_WORD) ? rem : PLM_WORD;

            dma_info_t dma_info(dma_addr / DMA_WORD_PER_BEAT, beats / DMA_WORD_PER_BEAT, DMA_SIZE, 0);
            dma_write_ctrl.Push(dma_info);

            for (uint32_t i = 0; i < beats; i += VEC_LEN)
            {
                for (uint32_t j = 0; j < VEC_DMA_RATIO; j++)
                {
                    ac_int<DMA_WIDTH> dataBV;
                    for (uint32_t k = 0; k < DMA_WORD_PER_BEAT; k++)
                    {
                        uint32_t bank = DMA_WORD_PER_BEAT * j + k;
                        uint32_t addr = plm_row + i / VEC_LEN;
                        dataBV.set_slc(k * DATA_WIDTH, plm[bank][addr]);
                    }
                    dma_write_chnl.Push(dataBV);
                }
            }

            rem      -= beats;
            dma_addr += beats;
            plm_row  += beats / VEC_LEN;
        }
        wait();
    }
};

#endif