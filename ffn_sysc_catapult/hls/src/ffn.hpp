// Copyright (c) 2011-2024 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#ifndef __TOP_HPP__
#define __TOP_HPP__

#pragma once

#include <systemc.h>
#include <nvhls_int.h>
#include <nvhls_connections.h>
#include <string>
#include <mc_scverify.h>

#include "../inc/ffn_data_types.hpp"
#include "../inc/ffn_specs.hpp"
#include "../inc/ffn_conf_info.hpp"

#include "../src/ffn_cfg.hpp"
#include "../src/ffn_tiling.hpp"
#include "../src/ffn_ctrl.hpp"
#include "../src/ffn_bias.hpp"
#include "../src/ffn_vec.hpp"

SC_MODULE(ffn_sysc_catapult)
{
    public:
    sc_in<bool>     CCS_INIT_S1(clk);
    sc_in<bool>     CCS_INIT_S1(rst);
    sc_out<bool>    CCS_INIT_S1(acc_done);

    Connections::In<conf_info_t >           CCS_INIT_S1(conf_info);
    Connections::In< ac_int<DMA_WIDTH> >    CCS_INIT_S1(dma_read_chnl);
    Connections::Out< ac_int<DMA_WIDTH> >   CCS_INIT_S1(dma_write_chnl);
    Connections::Out<dma_info_t>            CCS_INIT_S1(dma_read_ctrl);
    Connections::Out<dma_info_t>            CCS_INIT_S1(dma_write_ctrl);

    // cfg module IO
	Connections::Combinational <conf_info_t> CCS_INIT_S1(conf_info_tiling);
	Connections::Combinational <conf_info_t> CCS_INIT_S1(conf_info_dma2acc);
	Connections::Combinational <conf_info_t> CCS_INIT_S1(conf_info_plm2vec);
	Connections::Combinational <conf_info_t> CCS_INIT_S1(conf_info_acc2dma);
    
    // tiling module IO
	Connections::Combinational <int32_t> CCS_INIT_S1(seq_iter_itcn);
	Connections::Combinational <int32_t> CCS_INIT_S1(seq_cache_itcn);
	Connections::Combinational <int32_t> CCS_INIT_S1(w_iter_itcn);
    
	Connections::Combinational <array_t<FPDATA, VEC_LEN>> CCS_INIT_S1(vec_in);
	Connections::Combinational <array_t<FPDATA, VEC_LEN>> CCS_INIT_S1(vec_weight);
	Connections::Combinational <array_t<FPDATA, VEC_LEN>> CCS_INIT_S1(vec_psum);
	Connections::Combinational <array_t<FPDATA, VEC_LEN>> CCS_INIT_S1(vec_out);
	Connections::Combinational <array_t<FPDATA, VEC_LEN>> CCS_INIT_S1(data_in);
	Connections::Combinational <array_t<FPDATA, VEC_LEN>> CCS_INIT_S1(data_bias_in);
    Connections::Combinational <array_t<FPDATA, VEC_LEN>> CCS_INIT_S1(data_out);

    AccConfig           cfg_inst;
    AccTiling           tiling_inst;
    AccController       ctrl_inst;
    VectorEngine        vec_inst;
    BiasEngine          bias_inst;

    SC_HAS_PROCESS(ffn_sysc_catapult);
    ffn_sysc_catapult(const sc_module_name& name):
        sc_module(name),
        conf_info("conf_info"),
        dma_read_chnl("dma_read_chnl"),
        dma_write_chnl("dma_write_chnl"),
        dma_read_ctrl("dma_read_ctrl"),
        dma_write_ctrl("dma_write_ctrl"),
        cfg_inst("cfg_inst"),
        tiling_inst("tiling_inst"),
        ctrl_inst("ctrl_inst"),
        vec_inst("vec_inst"),
        bias_inst("bias_inst")
    {
        cfg_inst.clk(clk);
        cfg_inst.rst(rst);
        cfg_inst.conf_info(conf_info);
        cfg_inst.conf_info_tiling(conf_info_tiling);
        cfg_inst.conf_info_dma2acc(conf_info_dma2acc);
        cfg_inst.conf_info_plm2vec(conf_info_plm2vec);
        cfg_inst.conf_info_acc2dma(conf_info_acc2dma);

        tiling_inst.clk(clk);
        tiling_inst.rst(rst);
        tiling_inst.conf_info_tiling(conf_info_tiling);
        tiling_inst.seq_iter_itcn(seq_iter_itcn);
        tiling_inst.seq_cache_itcn(seq_cache_itcn);
        tiling_inst.w_iter_itcn(w_iter_itcn);


        ctrl_inst.clk(clk);
        ctrl_inst.rst(rst);
        ctrl_inst.acc_done(acc_done);
        ctrl_inst.dma_read_chnl(dma_read_chnl);
        ctrl_inst.dma_write_chnl(dma_write_chnl);
        ctrl_inst.dma_read_ctrl(dma_read_ctrl);
        ctrl_inst.dma_write_ctrl(dma_write_ctrl);
        ctrl_inst.conf_info_dma2acc(conf_info_dma2acc);
        ctrl_inst.conf_info_plm2vec(conf_info_plm2vec);
        ctrl_inst.conf_info_acc2dma(conf_info_acc2dma);
        ctrl_inst.seq_iter_itcn(seq_iter_itcn);
        ctrl_inst.seq_cache_itcn(seq_cache_itcn);
        ctrl_inst.w_iter_itcn(w_iter_itcn);
        ctrl_inst.vec_in_itcn(vec_in);
        ctrl_inst.vec_weight_itcn(vec_weight);
        ctrl_inst.vec_psum_itcn(vec_psum);
        ctrl_inst.vec_out_itcn(vec_out);
        ctrl_inst.data_in_itcn(data_in);
        ctrl_inst.data_out_itcn(data_out);
        ctrl_inst.data_bias_itcn(data_bias_in);

        vec_inst.clk(clk);
        vec_inst.rst(rst);
        vec_inst.vec_in(vec_in);
        vec_inst.vec_weight(vec_weight);
        vec_inst.vec_psum(vec_psum);
        vec_inst.vec_out(vec_out);
        
        bias_inst.clk(clk);
        bias_inst.rst(rst);
        bias_inst.data_in_itcn(data_in);
        bias_inst.data_out_itcn(data_out);
        bias_inst.data_bias_itcn(data_bias_in); 
    }
};

#endif
