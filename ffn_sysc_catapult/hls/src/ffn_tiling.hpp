// Copyright (c) 2011-2024 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#ifndef __FFN_TILE_HPP__
#define __FFN_TILE_HPP__

#pragma once

#include <systemc.h>
#include <nvhls_int.h>
#include <nvhls_connections.h>
#include <string>
#include <mc_scverify.h>

#include "../inc/ffn_data_types.hpp"
#include "../inc/ffn_specs.hpp"
#include "../inc/ffn_conf_info.hpp"


SC_MODULE(AccTiling)
{
    public:
    sc_in<bool>     CCS_INIT_S1(clk);
    sc_in<bool>     CCS_INIT_S1(rst);
    
    Connections::In<conf_info_t >   CCS_INIT_S1(conf_info_tiling);
	Connections::Out <int32_t>     CCS_INIT_S1(seq_iter_itcn);
	Connections::Out <int32_t>     CCS_INIT_S1(seq_cache_itcn);
	Connections::Out <int32_t>     CCS_INIT_S1(w_iter_itcn);

    SC_HAS_PROCESS(AccTiling);
    AccTiling(const sc_module_name& name): 
        sc_module(name),
        conf_info_tiling("conf_info_tiling"),
        seq_iter_itcn("seq_iter_itcn"),
        seq_cache_itcn("seq_cache_itcn"),
        w_iter_itcn("w_iter_itcn")
    {
        SC_THREAD(Tiling);
        sensitive << clk.pos();
        async_reset_signal_is(rst, false);
    }

    void Tiling()
    {
        conf_info_tiling.Reset();

        seq_iter_itcn.Reset();
        seq_cache_itcn.Reset();
        w_iter_itcn.Reset();

        wait();
        while(1)
        {
            conf_info_t conf_reg = conf_info_tiling.Pop();
            uint32_t seqlen = conf_reg.seqlen;
            uint32_t indim = conf_reg.indim;
            uint32_t outdim = conf_reg.outdim;
            
            ac_int<32,false> ac_plm = PLM_WORD;
            ac_int<32,false> ac_indim = indim;
            ac_int<32,false> ac_seqlen = seqlen;
            ac_int<32,false> ac_max_seq, ac_seq_iter, ac_seq_cache;

            ac_math::ac_div(ac_plm, ac_indim, ac_max_seq);

            ac_seq_cache = (ac_max_seq < ac_seqlen) ? ac_max_seq : ac_seqlen;

            ac_math::ac_div(ac_seqlen, ac_seq_cache, ac_seq_iter);

            uint32_t max_seq = ac_max_seq.to_uint();
            uint32_t seq_cache = ac_seq_cache.to_uint();
            uint32_t seq_iter = ac_seq_iter.to_uint();
            
            uint32_t w_iter = outdim / VEC_LEN;
            uint32_t o_iter = seq_iter * w_iter;
            
            seq_iter_itcn.Push(seq_iter);
            seq_cache_itcn.Push(seq_cache);
            w_iter_itcn.Push(w_iter);
        }
    }
};

#endif
