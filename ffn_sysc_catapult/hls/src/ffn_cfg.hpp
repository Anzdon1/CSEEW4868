// Copyright (c) 2011-2024 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#ifndef __FFN_CFG_HPP__
#define __FFN_CFG_HPP__

#pragma once

#include <systemc.h>
#include <nvhls_int.h>
#include <nvhls_connections.h>
#include <string>
#include <mc_scverify.h>

#include "../inc/ffn_data_types.hpp"
#include "../inc/ffn_specs.hpp"
#include "../inc/ffn_conf_info.hpp"

SC_MODULE(AccConfig)
{
    public:
    sc_in<bool>     CCS_INIT_S1(clk);
    sc_in<bool>     CCS_INIT_S1(rst);

    Connections::In<conf_info_t >   CCS_INIT_S1(conf_info);
	Connections::Out <conf_info_t>  CCS_INIT_S1(conf_info_tiling);
	Connections::Out <conf_info_t>  CCS_INIT_S1(conf_info_dma2acc);
	Connections::Out <conf_info_t>  CCS_INIT_S1(conf_info_plm2vec);
	Connections::Out <conf_info_t>  CCS_INIT_S1(conf_info_acc2dma);


    SC_HAS_PROCESS(AccConfig);
    AccConfig(const sc_module_name& name): 
        sc_module(name),
        conf_info("conf_info"),
        conf_info_tiling("conf_info_tiling"),
        conf_info_dma2acc("conf_info_dma2acc"),
        conf_info_plm2vec("conf_info_plm2vec"),
        conf_info_acc2dma("conf_info_acc2dma")
    {
        SC_THREAD(ConfigRead);
        sensitive << clk.pos();
        async_reset_signal_is(rst, false);
    }

    void ConfigRead()
    {
        conf_info.Reset();

        conf_info_tiling.Reset();
        conf_info_dma2acc.Reset();
        conf_info_plm2vec.Reset();
        conf_info_acc2dma.Reset();

        wait(); // wait for reset to propagate

        while (1)
        {
            conf_info_t conf_reg = conf_info.Pop();

            conf_info_tiling.Push(conf_reg);
            conf_info_dma2acc.Push(conf_reg);
            conf_info_plm2vec.Push(conf_reg);
            conf_info_acc2dma.Push(conf_reg);
        }
    }
};

#endif
