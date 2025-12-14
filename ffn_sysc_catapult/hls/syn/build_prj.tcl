#Copyright (c) 2011-2025 Columbia University, System Level Design Group
#SPDX-License-Identifier: Apache-2.0

set ACCELERATOR ffn
set DMA_WIDTH 64
set proj_name "${ACCELERATOR}_dma${DMA_WIDTH}"
# set proj_name "${ACCELERATOR}"
set ccs_file "${proj_name}.ccs"

if {[file exists $ccs_file]} {
    project load $ccs_file
} else {
    project new -name $proj_name
}

set CSIM_RESULTS "./tb_data/catapult_csim_results.log"
set RTL_COSIM_RESULTS "./tb_data/catapult_rtl_cosim_results.log"
set sfd [file dir [info script]]

# solution new -state initial
solution options defaults

options set /Input/CppStandard c++11
options set /Input/CompilerFlags "-DCONNECTIONS_ACCURATE_SIM -DCONNECTIONS_NAMING_ORIGINAL -DHLS_CATAPULT"
options set /Input/SearchPath {/opt/matchlib_examples_kit_fall_2024/matchlib_toolkit/include} -append
options set /Input/SearchPath {/opt/matchlib_examples_kit_fall_2024/matchlib_toolkit/examples/boost_home} -append
options set /Input/SearchPath {/opt/matchlib_examples_kit_fall_2024/matchlib_toolkit/examples/matchlib/cmod/include} -append


# options set /Input/SearchPath "$sfd/../src/mem_bank" -append
# options set /ComponentLibs/SearchPath "$sfd/../src/mem_bank" -append

# options set Input/CompilerFlags -DHLS_READY


# flow package require /DesignWrapper
flow package require /SCVerify

flow package require /QuestaSIM
flow package option set /QuestaSIM/ENABLE_CODE_COVERAGE true
solution options set /Flows/QuestaSIM/MSIM_AC_TYPES false

# flow package require ModelSim
# flow package option set /ModelSim/ENABLE_CODE_COVERAGE true
# solution options set /Flows/ModelSim/MSIM_AC_TYPES false

# options set Flows/DesignWrapper/USE_ORIGINAL_PINNAMES false

#
# Input
#

solution options set /Input/SearchPath { \
    ../inc/ \
    ../src/ \
    ../tb/ \
    ../inc/core/systems \
    ../inc/mem_bank } -append

solution new -state new -solution solution.v1 ${ACCELERATOR}

solution file add "../tb/testbench.cpp" -exclude true
solution file add "../tb/testbench.hpp" -exclude true
solution file add "../tb/sc_main.cpp" -exclude true
solution file add "../tb/system.hpp" -exclude true
solution file add "../inc/ffn_data_types.hpp"
solution file add "../inc/esp_dma_info_sysc.hpp"
solution file add "../inc/ffn_conf_info.hpp"
solution file add "../src/ffn_cfg.hpp"
solution file add "../src/ffn_vec.hpp"
solution file add "../src/ffn_ctrl.hpp"
solution file add "../src/ffn.hpp"
solution file add "../inc/ffn_specs.hpp"

solution file set ../inc/ffn_data_types.hpp -args -DDMA_WIDTH=$DMA_WIDTH
solution file set ../inc/ffn_specs.hpp -args -DDMA_WIDTH=$DMA_WIDTH

# solution file add "../tb/system.hpp" -exclude true
# solution file add "../tb/system.cpp" -exclude true
# solution file add "../tb/sc_main.cpp" -exclude true
# solution file add "../tb/driver.cpp" -exclude true
# solution file add "../tb/driver.hpp" -exclude true
# solution file add "../tb/memory.hpp" -exclude true
# solution file add "../src/utils.hpp"
# solution file add "../src/accelerated_sim.hpp"
# solution file add "../src/conv_layer_conf_info.hpp"
# solution file add "../src/conv_layer_functions.hpp"
# solution file add "../src/conv_layer.hpp"
# solution file add "../src/conv_layer.cpp"
# solution file add "../src/mem_wrap.hpp"
# solution file add "../src/mem_wrap1.hpp"
# solution file add "../src/conv_layer_specs.hpp"
# solution file add "../src/conv_layer_utils.hpp"

#
# Output
#

# Verilog only
solution option set Output/OutputVHDL false
solution option set Output/OutputVerilog true

# Package output in Solution dir
solution option set Output/PackageOutput true
solution option set Output/PackageStaticFiles true

# Add Prefix to library and generated sub-blocks
solution option set Output/PrefixStaticFiles true
solution options set Output/SubBlockNamePrefix "esp_acc_${ACCELERATOR}_"

# Do not modify names
solution option set Output/DoNotModifyNames true
options set Message/ErrorOverride ASSERT-1 -remove


solution library \
    add mgc_Xilinx-$FPGA_FAMILY$FPGA_SPEED_GRADE\_beh -- \
    -rtlsyntool Vivado \
    -manufacturer Xilinx \
    -family $FPGA_FAMILY \
    -speed $FPGA_SPEED_GRADE \
    -part $FPGA_PART_NUM

solution options set ComponentLibs/SearchPath ./ffn_bias/Catapult -append
solution options set ComponentLibs/SearchPath ./ffn_cfg/Catapult -append
solution options set ComponentLibs/SearchPath ./ffn_ctrl/Catapult -append
solution options set ComponentLibs/SearchPath ./ffn_tiling/Catapult -append
solution options set ComponentLibs/SearchPath ./ffn_vec/Catapult -append

# solution library add DUAL_PORT_RBW
solution library add Xilinx_RAMS

directive set -CLOCKS {clk {-CLOCK_PERIOD 5.0}}

solution library add {[Block] ffn_bias.v1}
solution library add {[Block] ffn_cfg.v1}
solution library add {[Block] ffn_ctrl.v1}
solution library add {[Block] ffn_tiling.v1}
solution library add {[Block] ffn_vec.v1}

solution design set $ACCELERATOR -top

go libraries

directive set /ffn/BiasEngine -MAP_TO_MODULE {[Block] ffn_bias.v1}
directive set /ffn/AccConfig -MAP_TO_MODULE {[Block] ffn_cfg.v1}
directive set /ffn/AccController -MAP_TO_MODULE {[Block] ffn_ctrl.v1}
directive set /ffn/AccTiling -MAP_TO_MODULE {[Block] ffn_tiling.v1}
directive set /ffn/VectorEngine -MAP_TO_MODULE {[Block] ffn_vec.v1}
go extract

flow run /Vivado/synthesize -shell vivado_v/ffn.v.xv
project save
