// Copyright (c) 2011-2024 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#ifndef __FFN_COM_HPP__
#define __FFN_COM_HPP__

#pragma once

#include <systemc.h>
#include <nvhls_int.h>
#include <nvhls_connections.h>
#include <string>
#include <mc_scverify.h>

#include "../inc/ffn_data_types.hpp"
#include "../inc/ffn_specs.hpp"
#include "../inc/ffn_conf_info.hpp"


SC_MODULE(VectorEngine)
{
    public:
    sc_in<bool>     CCS_INIT_S1(clk);
    sc_in<bool>     CCS_INIT_S1(rst);
    
	Connections::In <array_t<FPDATA, VEC_LEN>> CCS_INIT_S1(vec_in);
	Connections::In <array_t<FPDATA, VEC_LEN>> CCS_INIT_S1(vec_weight);
	Connections::In <array_t<FPDATA, VEC_LEN>> CCS_INIT_S1(vec_psum);
	Connections::Out <array_t<FPDATA, VEC_LEN>> CCS_INIT_S1(vec_out);

    SC_HAS_PROCESS(VectorEngine);
    VectorEngine(const sc_module_name& name): 
        sc_module(name),
        vec_in("vec_in"),
        vec_weight("vec_weight"),
        vec_out("vec_out")
    {
        SC_THREAD(Compute);
        sensitive << clk.pos();
        async_reset_signal_is(rst, false);
    }

    void Compute()
    {
        array_t<FPDATA, VEC_LEN> in_itcn, weight_itcn, psum_itcn, out_itcn;
        vec_in.Reset();
        vec_weight.Reset();
        vec_psum.Reset();
        vec_out.Reset();

        FPDATA in[VEC_LEN], weight[VEC_LEN], psum[VEC_LEN], out[VEC_LEN];
        
        wait();
        while(1)
        {
            in_itcn = vec_in.Pop();
            weight_itcn = vec_weight.Pop();
            psum_itcn = vec_psum.Pop();

            
            for (int vec=0; vec<VEC_LEN; vec++)
            {
                in[vec] = in_itcn.data[vec];
                weight[vec] = weight_itcn.data[vec];
                psum[vec] = psum_itcn.data[vec];

                out[vec] = psum[vec] + in[vec] * weight[vec];
                
                out_itcn.data[vec] = out[vec];

                // if (vec == 0){
                //     fprintf(stderr, "  vector > vec %d | input %f * weight %f + psum %f \n", 
                //         vec, in[vec].to_ac_float().to_float(), weight[vec].to_ac_float().to_float(), psum[vec].to_ac_float().to_float());
                // }
            }

            vec_out.Push(out_itcn);
        }
    }
};

#endif
