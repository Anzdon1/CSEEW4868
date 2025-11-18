// Copyright (c) 2011-2024 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#ifndef __CONF_INFO_HPP__
#define __CONF_INFO_HPP__

#pragma once

#include <sstream>
#include <ac_int.h>
#include <ac_fixed.h>
#include "ffn_specs.hpp"
#include "auto_gen_port_info.h"
#include "ac_enum.h"

//
// Configuration parameters for the accelerator.
//

struct conf_info_t
{

    /* <<--params-->> */
    int32_t seqlen;
    int32_t indim;
    int32_t outdim;
    int32_t addrI;
    int32_t addrW;
    int32_t addrB;
    int32_t addrO;

    AUTO_GEN_FIELD_METHODS(conf_info_t, (\
            seqlen  \
        ,   indim   \
        ,   outdim  \
        ,   addrI   \
        ,   addrW   \
        ,   addrB   \
        ,   addrO   \
    ))

};

template<typename T,int SIZE>
struct array_t
{
  
  T data[SIZE];

  AUTO_GEN_FIELD_METHODS(array_t, ( \
     data \
  ) )
};
#endif // __MAC_CONF_INFO_HPP__
