// Copyright (c) 2011-2024 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

// FURTHER NOTES: PIPELINE - PRAGMA!
// FURTHER NOTES: UNROLL MORE LOOPS!

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

    Connections::Combinational<int32_t> ld2com_sync;
    Connections::Combinational<int32_t> com2st_sync;
    Connections::Combinational<int32_t> st2ld_sync;

    ac_shared_bank_array_2D<DATA_TYPE, bks, ebks> plm_in_ping;
    ac_shared_bank_array_2D<DATA_TYPE, bks, ebks> plm_weight_ping;
    ac_shared_bank_array_2D<DATA_TYPE, bks, ebks> plm_bias_ping;
	ac_shared_bank_array_2D<DATA_TYPE, bks, ebks> plm_out_ping;

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

    void dma2acc() {
        dma_read_chnl.Reset();
        dma_read_ctrl.Reset();
    
        conf_info_dma2acc.Reset();
        seq_iter_itcn_dma2acc.ResetRead();
        seq_cache_itcn_dma2acc.ResetRead();
        w_iter_itcn_dma2acc.ResetRead();

        ld2com_sync.ResetWrite();
        st2ld_sync.ResetRead();
        
        wait();
        while(1)
        {
            conf_info_t conf_info   = conf_info_dma2acc.Pop();
            int32_t seqlen          = conf_info.seqlen;
            int32_t indim           = conf_info.indim;
            int32_t outdim          = conf_info.outdim;
            int32_t addrI           = conf_info.addrI;
            int32_t addrW           = conf_info.addrW;
            int32_t addrB           = conf_info.addrB;
            int32_t addrO           = conf_info.addrO;
            
            int32_t seq_iter        = seq_iter_itcn_dma2acc.Pop();
            int32_t seq_cache       = seq_cache_itcn_dma2acc.Pop();
            int32_t w_iter          = w_iter_itcn_dma2acc.Pop();            

            for (int32_t seq_i=0; seq_i<seq_iter; seq_i++)
            {
                int32_t in_ld_offset = addrI + indim*seq_cache*seq_i;
                int32_t in_ld_size = indim*seq_cache;
                
                load(in_ld_offset, in_ld_size, 0, plm_in_ping);

                for (int32_t w_i=0; w_i<w_iter; w_i++){



                    int32_t bias_ld_offset = addrB + VEC_LEN * w_i;
                    int32_t bias_ld_size = VEC_LEN;
                    load(bias_ld_offset, bias_ld_size, 0, plm_bias_ping);


                    for (int32_t w_ii=0; w_ii<indim; w_ii++){
                        int32_t w_ld_offset = addrW + VEC_LEN * w_i + outdim * w_ii;
                        int32_t w_ld_size = VEC_LEN;
                        load(w_ld_offset, w_ld_size, w_ii, plm_weight_ping);           
                    }

                    ld2com_sync.Push(1);  
                    st2ld_sync.Pop();
                }
            }
        }   
    }

    void data_convert_NVint(DATA_TYPE in, FPDATA_WORD &out)
    {
        for (int i=0; i<FPDATA_WL; i++) {
            out[i] = in[i];
        }
    }

    void plm2vec()
    {
        conf_info_plm2vec.Reset();
        seq_iter_itcn_plm2vec.ResetRead();
        seq_cache_itcn_plm2vec.ResetRead();
        w_iter_itcn_plm2vec.ResetRead();

        ld2com_sync.ResetRead();
        com2st_sync.ResetWrite();
        
        vec_in_itcn.Reset();
        vec_weight_itcn.Reset();
        vec_psum_itcn.Reset();
        vec_out_itcn.Reset();
        data_in_itcn.Reset();
        data_out_itcn.Reset();
        data_bias_itcn.Reset();

        wait();
        while(1)
        {
            conf_info_t conf_info   = conf_info_plm2vec.Pop();
            int32_t seqlen          = conf_info.seqlen;
            int32_t indim           = conf_info.indim;
            int32_t outdim          = conf_info.outdim;
            int32_t addrI           = conf_info.addrI;
            int32_t addrW           = conf_info.addrW;
            int32_t addrB           = conf_info.addrB;
            int32_t addrO           = conf_info.addrO;

            int32_t seq_iter        = seq_iter_itcn_plm2vec.Pop();
            int32_t seq_cache       = seq_cache_itcn_plm2vec.Pop();
            int32_t w_iter          = w_iter_itcn_plm2vec.Pop();         

            for (int32_t seq_i=0; seq_i<seq_iter; seq_i++)
            {
                for (int32_t w_i=0; w_i<w_iter; w_i++)
                {
                    ld2com_sync.Pop();
                    
                    for (int32_t seq_ii=0; seq_ii<seq_cache; seq_ii++)
                    {
                        FPDATA_WORD in_word[VEC_LEN], weight_word[VEC_LEN], bias_word[VEC_LEN];
                        FPDATA in[VEC_LEN], weight[VEC_LEN], bias[VEC_LEN];
                        array_t<FPDATA, VEC_LEN> in_buffer, weight_buffer, psum_buffer, res_buffer;

                        int in_addr, in_bank;
                        int w_addr, w_bank;

                        for (int32_t w_ii=0; w_ii<indim; w_ii++)
                        {
                            #pragma HLS PIPELINE
                            if (w_ii > 0) {
                                for (int vec=0; vec<VEC_LEN; vec++){
                                    psum_buffer.data[vec] = res_buffer.data[vec];
                                }
                            } else {
                                for (int vec=0; vec<VEC_LEN; vec++){
                                    psum_buffer.data[vec] = FPDATA(0);
                                }
                            }
                            
                            // Compute input PLM address once for this w_ii
                            int in_addr = (indim*seq_ii + w_ii) / VEC_LEN;
                            int in_bank = (w_ii % VEC_LEN);

                            // Read and convert input ONCE
                            data_convert_NVint(plm_in_ping[in_bank][in_addr], in_word[0]);
                            #ifdef FL_POINT
                                int2f(in_word[0], in[0]);
                            #else
                                int2fx(in_word[0], in[0]);
                            #endif
                            FPDATA in_scalar = in[0];   // broadcast this

                            // VEC LOOP
                            #pragma HLS UNROLL
                            for (int vec=0; vec<VEC_LEN; vec++)
                            {
                                // Broadcast same input scalar to every lane
                                in_buffer.data[vec] = in_scalar;

                                // Per-lane weight (bank = vec)
                                w_addr = w_ii;
                                w_bank = vec;

                               data_convert_NVint(plm_weight_ping[w_bank][w_addr], weight_word[vec]);
                            #ifdef FL_POINT
                                int2f(weight_word[vec], weight[vec]);
                            #else
                                int2fx(weight_word[vec], weight[vec]);
                            #endif
                                weight_buffer.data[vec] = weight[vec];
                            }

                            
                            vec_in_itcn.Push(in_buffer);
                            vec_weight_itcn.Push(weight_buffer);
                            vec_psum_itcn.Push(psum_buffer);
                            /* vector operation*/
                            res_buffer = vec_out_itcn.Pop();
                        }

                        array_t<FPDATA, VEC_LEN> data_in_buffer, data_out_buffer, data_bias_in_buffer;

                        data_in_buffer = res_buffer;

                        
                        for (int vec=0; vec<VEC_LEN; vec++){
                            data_convert_NVint(plm_bias_ping[vec][0], bias_word[vec]);
                            #ifdef FL_POINT
                                int2f(bias_word[vec], bias[vec]);
                            #else
                                int2fx(bias_word[vec], bias[vec]);
                            #endif
                                
                            data_bias_in_buffer.data[vec] = bias[vec];
                        }




                        data_in_itcn.Push(data_in_buffer);
                        data_bias_itcn.Push(data_bias_in_buffer);
                        // /* bias operation*/
                        data_out_buffer = data_out_itcn.Pop();

                        FPDATA_WORD out_word[VEC_LEN];
                        FPDATA out[VEC_LEN];
                        
                        int out_addr, out_bank;
                        
                        for (int vec=0; vec<VEC_LEN; vec++)
                        {
                            out[vec] = data_out_buffer.data[vec];
                            #ifdef FL_POINT
                            f2int(out[vec], out_word[vec]);
                            #else
                            fx2int(out[vec], out_word[vec]);
                            #endif
                            
                            out_addr = seq_ii;
                            out_bank = vec;
                            plm_out_ping[out_bank][out_addr] = out_word[vec];
                        }
                    }
                    
                    com2st_sync.Push(1);
                }
            }
        }
    }


    void acc2dma()
    {
        int32_t batch, row, addrO;
        dma_write_chnl.Reset();
        dma_write_ctrl.Reset();
        
        conf_info_acc2dma.Reset();
        seq_iter_itcn_acc2dma.ResetRead();
        seq_cache_itcn_acc2dma.ResetRead();
        w_iter_itcn_acc2dma.ResetRead();

        com2st_sync.ResetRead();
        st2ld_sync.ResetWrite();

        acc_done.write(false);

        wait();
        while (1)
        {
            conf_info_t conf_info   = conf_info_acc2dma.Pop();
            int32_t seqlen          = conf_info.seqlen;
            int32_t indim           = conf_info.indim;
            int32_t outdim          = conf_info.outdim;
            int32_t addrI           = conf_info.addrI;
            int32_t addrW           = conf_info.addrW;
            int32_t addrO           = conf_info.addrO;

            int32_t seq_iter        = seq_iter_itcn_acc2dma.Pop();
            int32_t seq_cache       = seq_cache_itcn_acc2dma.Pop();
            int32_t w_iter          = w_iter_itcn_acc2dma.Pop();        


            for (int32_t seq_i=0; seq_i<seq_iter; seq_i++)
            {
                for (int32_t w_i=0; w_i<w_iter; w_i++)
                {
                    com2st_sync.Pop();
                    
                    for (int32_t seq_ii=0; seq_ii<seq_cache; seq_ii++)
                    {
                        int32_t st_offset = addrO + outdim*seq_cache*seq_i + VEC_LEN*w_i + outdim*seq_ii;
                        int32_t st_sz =  VEC_LEN;
                        store(st_offset, st_sz, seq_ii, plm_out_ping);
                    }
                    st2ld_sync.Push(1);
                }
            }

            acc_done.write(true); wait();
            acc_done.write(false);
        }
    }

    template <typename SharedArray2D>
    void load(uint32_t dma_ld_addr, uint32_t dma_ld_size, uint32_t plm_addr, SharedArray2D& plm)
    {
        #if (DMA_WORD_PER_BEAT == 0)
            uint32_t len = dma_ld_size;
        #else
            uint32_t len = round_up(dma_ld_size, DMA_WORD_PER_BEAT);
        #endif

        for (int rem = len; rem > 0;) {
            uint32_t beats = (rem < PLM_WORD) ? rem : PLM_WORD;
        #if (DMA_WORD_PER_BEAT == 0)
            dma_info_t dma_info(dma_ld_addr * DMA_BEAT_PER_WORD, beats * DMA_BEAT_PER_WORD, DMA_SIZE, 0);
        #else
            dma_info_t dma_info(dma_ld_addr / DMA_WORD_PER_BEAT, beats / DMA_WORD_PER_BEAT, DMA_SIZE, 0);
        #endif
            dma_read_ctrl.Push(dma_info);
    
        
            for (uint32_t i = 0; i < beats; i += VEC_LEN) {
                for (uint32_t j = 0; j < VEC_DMA_RATIO; j++) {
                    ac_int<DMA_WIDTH> data_dma = dma_read_chnl.Pop();
    
                    
                    for (uint32_t k = 0; k < DMA_WORD_PER_BEAT; k++) {
                        uint32_t bank = DMA_WORD_PER_BEAT * j + k;
                        uint32_t addr = plm_addr + i / VEC_LEN;
                        // cerr << "plm_addr: " << plm_addr << " i: " << i << " addr: " << addr << endl;
                        plm[bank][addr] = data_dma.slc<DATA_WIDTH>(k * DATA_WIDTH);
                    }
                }
            }
            rem -= beats;
        }
        wait();
    }

    template <typename SharedArray2D>
    void store(uint32_t dma_st_addr, uint32_t dma_st_size, uint32_t plm_addr, SharedArray2D& plm)
    {
        #if (DMA_WORD_PER_BEAT == 0)
            uint32_t len = dma_st_size;
        #else
            uint32_t len = round_up(dma_st_size, DMA_WORD_PER_BEAT);
        #endif

            for (int rem=len; rem>0; )
            {
                uint32_t beats = (rem < PLM_WORD) ? rem : PLM_WORD;
            #if (DMA_WORD_PER_BEAT == 0)
                dma_info_t dma_info(dma_st_addr * DMA_BEAT_PER_WORD, beats * DMA_BEAT_PER_WORD, DMA_SIZE, 0);
            #else
                dma_info_t dma_info(dma_st_addr / DMA_WORD_PER_BEAT, beats / DMA_WORD_PER_BEAT, DMA_SIZE, 0);
            #endif
                dma_write_ctrl.Push(dma_info);
        
            
                for (uint32_t i=0; i<beats; i+=VEC_LEN)
                {
                    for (uint32_t j=0; j<VEC_DMA_RATIO; j++)
                    {
                        ac_int<DMA_WIDTH> dataBV;

                    
                        for (uint32_t k=0; k<DMA_WORD_PER_BEAT; k++)
                        {
                            uint32_t bank = DMA_WORD_PER_BEAT*j+k;
                            uint32_t addr = plm_addr + i/VEC_LEN;
                            
                            dataBV.set_slc(k*DATA_WIDTH, plm[bank][addr]);

                        }
                        dma_write_chnl.Push(dataBV);
                    }
                }
                rem -= beats;
            }
        wait();
    }

};

#endif
