#Copyright (c) 2011-2024 Columbia University, System Level Design Group
#SPDX-License-Identifier: Apache-2.0

set ccs_file "Catapult.ccs"
set DMA_WIDTH 64
set ACCELERATOR ffn

if {[file exists $ccs_file]} {
    project load $ccs_file
} else {
    project new
}

set sfd [file dir [info script]]

options set /Input/CppStandard c++11
options set /Input/CompilerFlags "-DCONNECTIONS_ACCURATE_SIM -DCONNECTIONS_NAMING_ORIGINAL -DHLS_CATAPULT"
options set /Input/SearchPath {/opt/matchlib_examples_kit_fall_2024/matchlib_toolkit/include} -append
options set /Input/SearchPath {/opt/matchlib_examples_kit_fall_2024/matchlib_toolkit/examples/boost_home/} -append
options set /Input/SearchPath {/opt/matchlib_examples_kit_fall_2024/matchlib_toolkit/examples/matchlib/cmod/include} -append
options set Architectural DefaultLoopMerging false

flow package require /SCVerify

flow package require /QuestaSIM
flow package option set /QuestaSIM/ENABLE_CODE_COVERAGE true

#
# Input
#

solution options set /Input/SearchPath { \
    ../../inc/ \
    ../../src/ \
    ../../tb/ \
    ../../inc/core/systems \
    ../../inc/mem_bank } -append

solution new -state new -solution solution.v1 ffn_bias



solution file add "../../tb/testbench.cpp" -exclude true
solution file add "../../tb/testbench.hpp" -exclude true
solution file add "../../tb/sc_main.cpp" -exclude true
solution file add "../../tb/system.hpp" -exclude true
solution file add "../../inc/ffn_data_types.hpp"
solution file add "../../inc/esp_dma_info_sysc.hpp"
solution file add "../../inc/ffn_conf_info.hpp"
solution file add "../../src/ffn_cfg.hpp"
solution file add "../../src/ffn_vec.hpp"
solution file add "../../src/ffn_ctrl.hpp"
solution file add "../../src/ffn.hpp"
solution file add "../../inc/ffn_specs.hpp"
solution file add "../../src/ffn_bias.hpp"
solution file add "../../src/ffn_tiling.hpp"

solution file set ../../inc/ffn_specs.hpp -args -DDMA_WIDTH=$DMA_WIDTH

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

solution library \
    add mgc_Xilinx-$FPGA_FAMILY$FPGA_SPEED_GRADE\_beh -- \
    -rtlsyntool Vivado \
    -manufacturer Xilinx \
    -family $FPGA_FAMILY \
    -speed $FPGA_SPEED_GRADE \
    -part $FPGA_PART_NUM

directive set -CLOCKS {clk {-CLOCK_PERIOD 5.0}}

solution design set BiasEngine -top
directive set REGISTER_THRESHOLD 34

go analyze
go compile
go libraries
go assembly
go architect
go allocate
go extract

project save
