// Copyright (c) 2011-2024 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#ifndef __FFN_BIAS_HPP__
#define __FFN_BIAS_HPP__

#pragma once

#include <systemc.h>
#include <nvhls_int.h>
#include <nvhls_connections.h>
#include <string>
#include <mc_scverify.h>

#include "../inc/ffn_data_types.hpp"
#include "../inc/ffn_specs.hpp"
#include "../inc/ffn_conf_info.hpp"

using namespace ac_math;

SC_MODULE(BiasEngine)
{
    public:
    sc_in<bool>     CCS_INIT_S1(clk);
    sc_in<bool>     CCS_INIT_S1(rst);
    
	Connections::In <array_t<FPDATA, VEC_LEN>> CCS_INIT_S1(data_in_itcn);
    Connections::In <array_t<FPDATA, VEC_LEN>> CCS_INIT_S1(data_bias_itcn);
	Connections::Out <array_t<FPDATA, VEC_LEN>> CCS_INIT_S1(data_out_itcn);

    SC_HAS_PROCESS(BiasEngine);
    BiasEngine(const sc_module_name& name): 
        sc_module(name),
        data_in_itcn("data_in_itcn"),
        data_out_itcn("data_out_itcn")
    {
        SC_THREAD(Activation);
        sensitive << clk.pos();
        async_reset_signal_is(rst, false);
    }

    void Activation()
    {
        array_t<FPDATA, VEC_LEN> in_buffer, out_buffer, bias_in_buffer;
        data_in_itcn.Reset();
        data_bias_itcn.Reset();
        data_out_itcn.Reset();

        FPDATA in[VEC_LEN], out[VEC_LEN], bias[VEC_LEN], in_tmp[VEC_LEN];
        FPDATA_WORD in_word[VEC_LEN], out_word[VEC_LEN], bias_word[VEC_LEN];
        ac_fixed<FPDATA_WL, FPDATA_IL> data_in[VEC_LEN], data_out[VEC_LEN], bias_data_in[VEC_LEN], total_data_in[VEC_LEN];

        wait();
        while(1)
        {
            in_buffer = data_in_itcn.Pop();
            bias_in_buffer = data_bias_itcn.Pop();

            
            for (int vec=0; vec<VEC_LEN; vec++)
            {
                in_tmp[vec] = in_buffer.data[vec];
                bias[vec] = bias_in_buffer.data[vec];
                in[vec] = in_tmp[vec] + bias[vec];

            #ifdef FL_POINT
                f2int(in[vec], in_word[vec]);
                int2fx(in_word[vec], data_in[vec]);
            #else
                data_in[vec] = ac_fixed<FPDATA_WL, FPDATA_IL>(in[vec]);
            #endif

            data_out[vec] = data_in[vec];

            #ifdef FL_POINT
                fx2int(data_out[vec], out_word[vec]);
                int2f(out_word[vec], out[vec]);
            #else
                out[vec] = data_out[vec];
            #endif
    
                out_buffer.data[vec] = out[vec];                
            }

            data_out_itcn.Push(out_buffer);
        }
    }
};

#endif
