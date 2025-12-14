#Copyright (c) 2011-2025 Columbia University, System Level Design Group
#SPDX-License-Identifier: Apache-2.0

TECH=virtex7

include ./dma.mk





CXXFLAGS += -g -std=c++11 -Wall -Wno-unknown-pragmas -Wno-unused-variable -Wno-unused-label -Wno-maybe-uninitialized

ifndef SYSTEMC_HOME
$(error - Environment variable SYSTEMC_HOME must be defined)
endif
ifndef CONNECTIONS_HOME
$(error - Environment variable CONNECTIONS_HOME must be defined)
endif
ifndef MATCHLIB_HOME
$(error - Environment variable MATCHLIB_HOME must be defined)
endif
ifndef BOOST_HOME
$(error - Environment variable BOOST_HOME must be defined)
endif
ifndef RAPIDJSON_HOME
$(error - Environment variable RAPIDJSON_HOME must be defined)
endif
ifndef AC_TYPES
$(error - Environment variable AC_TYPES must be defined)
endif
ifndef AC_SIMUTILS
$(error - Environment variable AC_SIMUTILS must be defined)
endif

# Default to the compiler installed on the machine
CXX ?= g++
LD_LIBRARY_PATH := $(if $(LD_LIBRARY_PATH),$(LD_LIBRARY_PATH):$(SYSTEMC_HOME)/lib:$(SYSTEMC_HOME)/lib-linux64,$(SYSTEMC_HOME)/lib:$(SYSTEMC_HOME)/lib-linux64)
export LD_LIBRARY_PATH
LIBDIRS += -L$(SYSTEMC_HOME)/lib -L$(SYSTEMC_HOME)/lib-linux64

# endif

# ---------------------------------------------------------------------

# Check: $(SYSTEMC_HOME)/include/systemc.h must exist
checkvar_SYSTEMC_HOME: $(SYSTEMC_HOME)/include/systemc.h

# Check: $(CONNECTIONS_HOME)/include/connections/connections.h must exist
checkvar_CONNECTIONS_HOME: $(CONNECTIONS_HOME)/include/connections/connections.h

# Check: $(MATCHLIB_HOME)/cmod/include/nvhls_marshaller.h
checkvar_MATCHLIB_HOME: $(MATCHLIB_HOME)/cmod/include/nvhls_marshaller.h

# Check: $(BOOST_HOME)/include/boost/preprocessor/arithmetic/add.hpp
checkvar_BOOST_HOME: $(BOOST_HOME)/include/boost/preprocessor/arithmetic/add.hpp

# Check: $(RAPIDJSON_HOME)/include/rapidjson/document.h
checkvar_RAPIDJSON_HOME: $(RAPIDJSON_HOME)/include/rapidjson/document.h

# Check: $(AC_TYPES)/include/ac_int.h
checkvar_AC_TYPES: $(AC_TYPES)/include/ac_int.h

# Check: $(AC_SIMUTILS)/include/mc_scverify.h
checkvar_AC_SIMUTILS: $(AC_SIMUTILS)/include/mc_scverify.h

# Rule to check that environment variables are set correctly
checkvars: checkvar_SYSTEMC_HOME checkvar_CONNECTIONS_HOME checkvar_MATCHLIB_HOME checkvar_BOOST_HOME checkvar_RAPIDJSON_HOME checkvar_AC_TYPES checkvar_AC_SIMUTILS
# =====================================================================

export CATAPULT_HOME SYSTEMC_HOME CONNECTIONS_HOME MATCHLIB_HOME BOOST_HOME RAPIDJSON_HOME AC_TYPES AC_SIMUTILS

# Determine the director containing the source files from the path to this Makefile
PWD := $(shell pwd)
SOURCE_DIR = $(subst syn,src/,$(PWD))
TB_DIR = $(subst syn,tb/,$(PWD))
INC_DIR = $(subst syn,inc/,$(PWD))
# INCDMA_DIR = $(subst syn,inc/core/systems,$(PWD))

SRCS := $(wildcard $(SOURCE_DIR)*.cpp)
SRCS += $(wildcard $(TB_DIR)*.cpp)

# SYS_FILE ?= ./conv_layer/conv_layer.v1/scverify/concat_sim_rtl_v_msim/system.cpp.cxxts
SYS_FILE ?= ./ffn/ffn.v1/scverify/concat_sim_rtl_v_msim/system.cpp.cxxts
SYS_FILE_SMALL ?= ./ffn_SMALL_dma$(DMA_WIDTH_SMALL)/ffn.v1/scverify/concat_sim_rtl_v_msim/system.cpp.cxxts
SYS_FILE_MEDIUM ?= ./ffn_MEDIUM_dma$(DMA_WIDTH_MEDIUM)/ffn.v1/scverify/concat_sim_rtl_v_msim/system.cpp.cxxts
SYS_FILE_FAST ?= ./ffn_FAST_dma$(DMA_WIDTH_FAST)/ffn.v1/scverify/concat_sim_rtl_v_msim/system.cpp.cxxts

INCDIRS += -I$(SOURCE_DIR) -I$(TB_DIR) -I$(INC_DIR)

INCDIRS += -I$(SYSTEMC_HOME)/include -I$(SYSTEMC_HOME)/src 
INCDIRS += -I$(MATCHLIB_TOOL)/../include
# INCDIRS += -I$(MATCHLIB_TOOL)/matchlib_examples/include
INCDIRS += -I$(CONNECTIONS_HOME)/include
INCDIRS += -I$(MATCHLIB_HOME)/cmod/include
INCDIRS += -I$(BOOST_HOME)/include
INCDIRS += -I$(RAPIDJSON_HOME)/include
INCDIRS += -I$(AC_TYPES)/include
INCDIRS += -I$(AC_MATH)/include
INCDIRS += -I$(AC_SIMUTILS)/include

CPPFLAGS += $(INCDIRS)
CPPFLAGS += -DSC_INCLUDE_DYNAMIC_PROCESSES 
CPPFLAGS += -DSEGMENT_BURST_SIZE=16
CXXFLAGS += -D__CUSTOM_SIM__
CXXFLAGS += -D__MATCHLIB_CONNECTIONS__
CXXFLAGS += -DHLS_CATAPULT

LIBS += -lsystemc -lpthread

CATAPULT_PRODUCT = ultra

HW_ROOT ?= $(shell realpath ../..)

SENTENCE_PATH ?= $(HW_ROOT)/data
SENTENCE = $(shell ls $(SENTENCE_PATH) | sed -e 's/.bin//' | awk -F/ '{print $$(NF-1)}')
SENTENCE-exe-syn = $(addsuffix -exe-syn, $(SENTENCE))
SENTENCE-exe-syn-tlm = $(addsuffix -exe-syn-tlm, $(SENTENCE))
SENTENCE-exe = $(addsuffix -exe, $(SENTENCE))


####### behavioral sim ############################
SENTENCE-small-exe = $(addsuffix -small-exe, $(SENTENCE))
SENTENCE-medium-exe = $(addsuffix -medium-exe, $(SENTENCE))
SENTENCE-fast-exe = $(addsuffix -fast-exe, $(SENTENCE))
SENTENCE-accelerated-small-exe = $(addsuffix -accelerated-small-exe, $(SENTENCE))
SENTENCE-accelerated-medium-exe = $(addsuffix -accelerated-medium-exe, $(SENTENCE))
SENTENCE-accelerated-fast-exe = $(addsuffix -accelerated-fast-exe, $(SENTENCE))

SENTENCE-view_wave = $(addsuffix -view_wave, $(SENTENCE))
SENTENCE-trace.vcd = $(addsuffix -trace.vcd, $(SENTENCE))
SENTENCE-trace.wlf = $(addsuffix -trace.wlf, $(SENTENCE))

SENTENCE-sim = $(addsuffix -sim, $(SENTENCE))
SENTENCE-sim-gui = $(addsuffix -sim-gui, $(SENTENCE))

####### rtl sim ############################
SENTENCE-small-sim = $(addsuffix -small-sim, $(SENTENCE))
SENTENCE-medium-sim = $(addsuffix -medium-sim, $(SENTENCE))
SENTENCE-fast-sim = $(addsuffix -fast-sim, $(SENTENCE))
SENTENCE-accelerated-small-sim = $(addsuffix -accelerated-small-sim, $(SENTENCE))
SENTENCE-accelerated-medium-sim = $(addsuffix -accelerated-medium-sim, $(SENTENCE))
SENTENCE-accelerated-fast-sim = $(addsuffix -accelerated-fast-sim, $(SENTENCE))

.PHONY: all build clean sim_clean sim hls
build: sim_sc_syn

all: hls sim

trace.vcd: sim_sc_syn
	-@echo "Starting execution in directory `pwd`"
	./$^



## BEHAVIORAL SIMULATION

sim_sc_syn: $(SRCS) 
	$(CXX) $(CXXFLAGS) -DHLS_READY -DCONNECTIONS_ACCURATE_SIM $(CPPFLAGS) $(LIBDIRS) $(SRCS) -o $@ $(LIBS)

sim_sc_syn_tlm: $(SRCS) 
	$(CXX) $(CXXFLAGS) -DHLS_READY -DCONNECTIONS_FAST_SIM $(CPPFLAGS) $(LIBDIRS) $(SRCS) -o $@ $(LIBS)

sim_sc: $(SRCS) 
	$(CXX) $(CXXFLAGS) -DDMA_WIDTH=64 -DACCELERATED_SIM=0 $(CPPFLAGS) $(LIBDIRS) $(SRCS) -o $@ $(LIBS)

sim_sc_small: $(SRCS) exe-clean
	$(CXX) $(CXXFLAGS) -DDMA_WIDTH=$(DMA_WIDTH_SMALL) -DARC_SMALL -DACCELERATED_SIM=0 $(CPPFLAGS) $(LIBDIRS) $(SRCS) -o $@ $(LIBS)

sim_sc_medium: $(SRCS) exe-clean
	$(CXX) $(CXXFLAGS) -DDMA_WIDTH=$(DMA_WIDTH_MEDIUM) -DARC_MEDIUM -DACCELERATED_SIM=0 $(CPPFLAGS) $(LIBDIRS) $(SRCS) -o $@ $(LIBS)

sim_sc_fast: $(SRCS) exe-clean
	$(CXX) $(CXXFLAGS) -DDMA_WIDTH=$(DMA_WIDTH_FAST) -DARC_FAST -DACCELERATED_SIM=0 $(CPPFLAGS) $(LIBDIRS) $(SRCS) -o $@ $(LIBS)

sim_sc_small_accelerated: $(SRCS) exe-clean
	$(CXX) $(CXXFLAGS) -DDMA_WIDTH=$(DMA_WIDTH_SMALL) -DARC_SMALL -DACCELERATED_SIM=1 $(CPPFLAGS) $(LIBDIRS) $(SRCS) -o $@ $(LIBS)

sim_sc_medium_accelerated: $(SRCS) exe-clean
	$(CXX) $(CXXFLAGS) -DDMA_WIDTH=$(DMA_WIDTH_MEDIUM) -DARC_MEDIUM -DACCELERATED_SIM=1 $(CPPFLAGS) $(LIBDIRS) $(SRCS) -o $@ $(LIBS)

sim_sc_fast_accelerated: $(SRCS) exe-clean
	$(CXX) $(CXXFLAGS) -DDMA_WIDTH=$(DMA_WIDTH_FAST) -DARC_FAST -DACCELERATED_SIM=1 $(CPPFLAGS) $(LIBDIRS) $(SRCS) -o $@ $(LIBS)

exe-clean:
	rm -f sim_sc
	rm -f sim_sc_small
	rm -f sim_sc_medium
	rm -f sim_sc_fast
	rm -f sim_sc_small_accelerated
	rm -f sim_sc_medium_accelerated
	rm -f sim_sc_fast_accelerated
	rm -f sim_sc_syn
	rm -f sim_sc_syn_tlm
	
# This target is used to run behavioral simulation
# of the non-synthesizable version of the design for any input sentence
$(SENTENCE-exe): sim_sc
	-@echo "Starting execution in directory `pwd`"
	SENTENCE=$(@:-exe=) ./$^ 
# ./$^ $(@:-exe=)

$(SENTENCE-small-exe): sim_sc_small
	-@echo "Starting execution in directory `pwd`"
	SENTENCE=$(@:-small-exe=) ./$^

$(SENTENCE-medium-exe): sim_sc_medium
	-@echo "Starting execution in directory `pwd`"
	SENTENCE=$(@:-medium-exe=) ./$^

$(SENTENCE-fast-exe): sim_sc_fast
	-@echo "Starting execution in directory `pwd`"
	SENTENCE=$(@:-fast-exe=) ./$^

$(SENTENCE-accelerated-small-exe): sim_sc_small_accelerated
	-@echo "Starting execution in directory `pwd`"
	SENTENCE=$(@:-accelerated-small-exe=) ./$^
	mkdir -p test/small/accuracy
	cp accelerated_data.txt test/small/accuracy/accelerated_data_exe_$(@:-accelerated-small-exe=).txt

$(SENTENCE-accelerated-medium-exe): sim_sc_medium_accelerated
	-@echo "Starting execution in directory `pwd`"
	SENTENCE=$(@:-accelerated-medium-exe=) ./$^

$(SENTENCE-accelerated-fast-exe): sim_sc_fast_accelerated
	-@echo "Starting execution in directory `pwd`"
	SENTENCE=$(@:-accelerated-fast-exe=) ./$^
	mkdir -p test/fast/accuracy
	cp accelerated_data.txt test/fast/accuracy/accelerated_data_exe_$(@:-accelerated-fast-exe=).txt

bert-dbg: sim_sc
	$(QUIET_RUN) LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(SYSTEMC)/lib-$(TARGET_ARCH) SENTENCE=boring gdb ./$<

# This target is used to run behavioral simulation
# of the synthesizable version of the design for any input sentence
$(SENTENCE-exe-syn): sim_sc_syn
	-@echo "Starting execution in directory `pwd`"
	./$^ $(@:-exe-syn=)

# This target is used to run behavioral simulation
# of the synthesizable version of the design for any input sentence with faster TLM mode
$(SENTENCE-exe-syn-tlm): sim_sc_syn_tlm
	-@echo "Starting execution in directory `pwd`"
	./$^ $(@:-exe-syn-tlm=)



##############

$(SENTENCE-trace.vcd): sim_sc_syn
	-@echo "Starting execution in directory `pwd`"
	./$^ $(@:-trace.vcd=)

$(SENTENCE-trace.wlf): %.wlf : %.vcd
	vcd2wlf trace.vcd trace.wlf

$(SENTENCE-view_wave): %-view_wave : %-trace.wlf
	vsim trace.wlf -nolog -do "add wave -r trace:/SystemC/*" -do "wave zoom full"

##############

## HLS

#This target is used to run hls

hls-clean:
	rm -rf ffn_dma64
	rm -f catapult.log
	rm -f ffn_dma64.ccs
	rm -f sim_sc

hls-clean-all-arch: exe-clean
	rm -rf ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL)
	rm -rf ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM)
	rm -rf ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST)
	rm -f catapult.log
	rm -f ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL).ccs
	rm -f ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM).ccs
	rm -f ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST).ccs

hls-clean-small-arch: exe-clean
	rm -rf ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL)
	rm -f catapult.log
	rm -f ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL).ccs

hls-clean-medium-arch: exe-clean
	rm -rf ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM)
	rm -f catapult.log
	rm -f ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM).ccs

hls-clean-fast-arch: exe-clean
	rm -rf ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST)
	rm -f catapult.log
	rm -f ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST).ccs


hls:
	@echo '   ' ;if test -e ./ffn_dma64; then \
		rm -r ffn_dma64; \
	fi;
	catapult -product $(CATAPULT_PRODUCT) -shell -f ./build_prj_top.tcl

hls-gui:
	@echo '   ' ;if test -e ./ffn_dma64; then \
		rm -r ffn_dma64; \
	fi;
	catapult -product $(CATAPULT_PRODUCT) -f ./build_prj_top.tcl

hls-small: hls-clean-small-arch
	@echo '';if test -e ./ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL); then \
		rm -rf ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL); \
	fi;
	@echo '';export arch=ARC_SMALL; export DMA_WIDTH=$(DMA_WIDTH_SMALL); \
	catapult -product $(CATAPULT_PRODUCT) -shell -f ./build_prj_top_test.tcl 

hls-medium: hls-clean-medium-arch
	@echo '';if test -e ./ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM); then \
		rm -rf ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM); \
	fi;
	@echo '';export arch=ARC_MEDIUM; export DMA_WIDTH=$(DMA_WIDTH_MEDIUM); \
	catapult -product $(CATAPULT_PRODUCT) -shell -f ./build_prj_top_test.tcl

hls-fast: hls-clean-fast-arch
	@echo '';if test -e ./ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST); then \
		rm -rf ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST); \
	fi;
	@echo '';export arch=ARC_FAST; export DMA_WIDTH=$(DMA_WIDTH_FAST); \
	catapult -product $(CATAPULT_PRODUCT) -shell -f ./build_prj_top_test.tcl

###### hls for submodules #####
hls-bias-small:
	cd ./ffn_bias; \
	if test -e ./ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL); then \
		rm -rf ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL); \
		rm -f ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL).ccs; \
		rm -f catapult*; \
	fi; \
	export arch=ARC_SMALL; export DMA_WIDTH=$(DMA_WIDTH_SMALL); \
	catapult -product $(CATAPULT_PRODUCT) -shell -f ./build_prj_top_test.tcl

hls-bias-medium:
	cd ./ffn_bias; \
	if test -e ./ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM); then \
		rm -rf ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM); \
		rm -f ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM).ccs; \
		rm -f catapult*; \
	fi; \
	export arch=ARC_MEDIUM; export DMA_WIDTH=$(DMA_WIDTH_MEDIUM); \
	catapult -product $(CATAPULT_PRODUCT) -shell -f ./build_prj_top_test.tcl

hls-bias-fast:
	cd ./ffn_bias; \
	if test -e ./ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST); then \
		rm -rf ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST); \
		rm -f ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST).ccs; \
		rm -f catapult*; \
	fi; \
	export arch=ARC_FAST; export DMA_WIDTH=$(DMA_WIDTH_FAST); \
	catapult -product $(CATAPULT_PRODUCT) -shell -f ./build_prj_top_test.tcl

hls-cfg-small:
	cd ./ffn_cfg; \
	if test -e ./ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL); then \
		rm -rf ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL); \
		rm -f ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL).ccs; \
		rm -f catapult*; \
	fi; \
	export arch=ARC_SMALL; export DMA_WIDTH=$(DMA_WIDTH_SMALL); \
	catapult -product $(CATAPULT_PRODUCT) -shell -f ./build_prj_top_test.tcl

hls-cfg-medium:
	cd ./ffn_cfg; \
	if test -e ./ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM); then \
		rm -rf ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM); \
		rm -f ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM).ccs; \
		rm -f catapult*; \
	fi; \
	export arch=ARC_MEDIUM; export DMA_WIDTH=$(DMA_WIDTH_MEDIUM); \
	catapult -product $(CATAPULT_PRODUCT) -shell -f ./build_prj_top_test.tcl

hls-cfg-fast:
	cd ./ffn_cfg; \
	if test -e ./ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST); then \
		rm -rf ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST); \
		rm -f ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST).ccs; \
		rm -f Catapult.ccs; \
		rm -f catapult*; \
	fi; \
	export arch=ARC_FAST; export DMA_WIDTH=$(DMA_WIDTH_FAST); \
	catapult -product $(CATAPULT_PRODUCT) -shell -f ./build_prj_top_test.tcl

hls-ctrl-small:
	cd ./ffn_ctrl; \
	if test -e ./ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL); then \
		rm -rf ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL); \
		rm -f ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL).ccs; \
		rm -f Catapult.ccs; \
		rm -f catapult*; \
	fi; \
	export arch=ARC_SMALL; export DMA_WIDTH=$(DMA_WIDTH_SMALL); \
	catapult -product $(CATAPULT_PRODUCT) -shell -f ./build_prj_top_test.tcl

hls-ctrl-medium:
	cd ./ffn_ctrl; \
	if test -e ./ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM); then \
		rm -rf ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM); \
		rm -f ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM).ccs; \
		rm -f catapult*; \
	fi; \
	export arch=ARC_MEDIUM; export DMA_WIDTH=$(DMA_WIDTH_MEDIUM); \
	catapult -product $(CATAPULT_PRODUCT) -shell -f ./build_prj_top_test.tcl

hls-ctrl-fast:
	cd ./ffn_ctrl; \
	if test -e ./ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST); then \
		rm -rf ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST); \
		rm -f ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST).ccs; \
		rm -f catapult*; \
	fi; \
	export arch=ARC_FAST; export DMA_WIDTH=$(DMA_WIDTH_FAST); \
	catapult -product $(CATAPULT_PRODUCT) -shell -f ./build_prj_top_test.tcl

hls-tiling-small:
	cd ./ffn_tiling; \
	if test -e ./ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL); then \
		rm -rf ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL); \
		rm -f ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL).ccs; \
		rm -f catapult*; \
	fi; \
	export arch=ARC_SMALL; export DMA_WIDTH=$(DMA_WIDTH_SMALL); \
	catapult -product $(CATAPULT_PRODUCT) -shell -f ./build_prj_top_test.tcl

hls-tiling-medium:
	cd ./ffn_tiling; \
	if test -e ./ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM); then \
		rm -rf ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM); \
		rm -f ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM).ccs; \
		rm -f catapult*; \
	fi; \
	export arch=ARC_MEDIUM; export DMA_WIDTH=$(DMA_WIDTH_MEDIUM); \
	catapult -product $(CATAPULT_PRODUCT) -shell -f ./build_prj_top_test.tcl

hls-tiling-fast:
	cd ./ffn_tiling; \
	if test -e ./ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST); then \
		rm -rf ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST); \
		rm -f ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST).ccs; \
		rm -f catapult*; \
	fi; \
	export arch=ARC_FAST; export DMA_WIDTH=$(DMA_WIDTH_FAST); \
	catapult -product $(CATAPULT_PRODUCT) -shell -f ./build_prj_top_test.tcl

hls-vec-small:
	cd ./ffn_vec; \
	if test -e ./ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL); then \
		rm -rf ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL); \
		rm -f ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL).ccs; \
		rm -f catapult*; \
	fi; \
	export arch=ARC_SMALL; export DMA_WIDTH=$(DMA_WIDTH_SMALL); \
	catapult -product $(CATAPULT_PRODUCT) -shell -f ./build_prj_top_test.tcl

hls-vec-medium:
	cd ./ffn_vec; \
	if test -e ./ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM); then \
		rm -rf ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM); \
		rm -f ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM).ccs; \
		rm -f catapult*; \
	fi; \
	export arch=ARC_MEDIUM; export DMA_WIDTH=$(DMA_WIDTH_MEDIUM); \
	catapult -product $(CATAPULT_PRODUCT) -shell -f ./build_prj_top_test.tcl

hls-vec-fast:
	cd ./ffn_vec; \
	if test -e ./ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST); then \
		rm -rf ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST); \
		rm -f ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST).ccs; \
		rm -f catapult*; \
	fi; \
	export arch=ARC_FAST; export DMA_WIDTH=$(DMA_WIDTH_FAST); \
	catapult -product $(CATAPULT_PRODUCT) -shell -f ./build_prj_top_test.tcl
##############

## RTL SIMULATION

#This target is used to run RTL-simulation
#for any input sentence
$(SENTENCE-sim):
	date +%Y%m%d_%H%M%S > simulation_time.txt
	if test -e $(SYS_FILE); then \
		rm $(SYS_FILE) & \
	fi;
	@echo '   ' ;export IMM=$(@:-sim=); \
	if test -e ./ffn_dma64; then \
		SENTENCE=$(@:-sim=) catapult -product $(CATAPULT_PRODUCT) -shell -f ./rtl_sim.tcl; \
	else \
		echo "Run HLS before RTL-simulation"; \
	fi;
	date +%Y%m%d_%H%M%S >> simulation_time.txt

$(SENTENCE-sim-gui):
	if test -e $(SYS_FILE); then \
		rm $(SYS_FILE) & \
	fi;
	@echo '   ' ;export IMM=$(@:-sim-gui=); \
	if test -e ./conv_layer; then \
		catapult -product $(CATAPULT_PRODUCT) -shell -f ./rtl_sim_gui.tcl; \
	else \
		echo "Run HLS before RTL-simulation"; \
	fi;

######## rtl sim for different arch
$(SENTENCE-small-sim): 
	date +%Y%m%d_%H%M%S > simulation_time.txt
	if test -e $(SYS_FILE_SMALL); then \
		rm $(SYS_FILE_SMALL) & \
	fi;
	@echo '   ' ;export RUN_ACCELERATED_SIM=0; export arch=ARC_SMALL; export DMA_WIDTH=$(DMA_WIDTH_SMALL);\
	if test -e ./ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL); then \
		SENTENCE=$(@:-small-sim=) catapult -product $(CATAPULT_PRODUCT) -shell -f ./rtl_sim_test.tcl; \
	else \
		echo "Run HLS before RTL-simulation"; \
	fi;
	date +%Y%m%d_%H%M%S >> simulation_time.txt

$(SENTENCE-medium-sim): 
	date +%Y%m%d_%H%M%S > simulation_time.txt
	if test -e $(SYS_FILE_MEDIUM); then \
		rm $(SYS_FILE_MEDIUM) & \
	fi;
	@echo '   ' ;export RUN_ACCELERATED_SIM=0; export arch=ARC_MEDIUM; export DMA_WIDTH=$(DMA_WIDTH_MEDIUM);\
	if test -e ./ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM); then \
		SENTENCE=$(@:-medium-sim=) catapult -product $(CATAPULT_PRODUCT) -shell -f ./rtl_sim_test.tcl; \
	else \
		echo "Run HLS before RTL-simulation"; \
	fi;
	date +%Y%m%d_%H%M%S >> simulation_time.txt

$(SENTENCE-fast-sim): 
	date +%Y%m%d_%H%M%S > simulation_time.txt
	if test -e $(SYS_FILE_FAST); then \
		rm $(SYS_FILE_FAST) & \
	fi;
	@echo '   ' ;export RUN_ACCELERATED_SIM=0; export arch=ARC_FAST; export DMA_WIDTH=$(DMA_WIDTH_FAST);\
	if test -e ./ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST); then \
		SENTENCE=$(@:-fast-sim=) catapult -product $(CATAPULT_PRODUCT) -shell -f ./rtl_sim_test.tcl; \
	else \
		echo "Run HLS before RTL-simulation"; \
	fi;
	date +%Y%m%d_%H%M%S >> simulation_time.txt

###### accelerated rtl sim for different arch
$(SENTENCE-accelerated-small-sim): 
	date +%Y%m%d_%H%M%S > simulation_time.txt
	if test -e $(SYS_FILE_SMALL); then \
		rm $(SYS_FILE_SMALL) & \
	fi;
	@echo '   ' ;export RUN_ACCELERATED_SIM=1; export arch=ARC_SMALL; export DMA_WIDTH=$(DMA_WIDTH_SMALL); \
	if test -e ./ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL); then \
		SENTENCE=$(@:-accelerated-small-sim=) catapult -product $(CATAPULT_PRODUCT) -shell -f ./rtl_sim_test.tcl; \
	else \
		echo "Run HLS before RTL-simulation"; \
	fi;
	date +%Y%m%d_%H%M%S >> simulation_time.txt
	mkdir -p test/small/accuracy
	cp accelerated_data.txt test/small/accuracy/accelerated_data_sim_$(@:-accelerated-small-sim=).txt

$(SENTENCE-accelerated-medium-sim): 
	date +%Y%m%d_%H%M%S > simulation_time.txt
	if test -e $(SYS_FILE_MEDIUM); then \
		rm $(SYS_FILE_MEDIUM) & \
	fi;
	@echo '   ' ;export RUN_ACCELERATED_SIM=1; export arch=ARC_MEDIUM; export DMA_WIDTH=$(DMA_WIDTH_MEDIUM);\
	if test -e ./ffn_ARC_MEDIUM_sysc_catapult_dma$(DMA_WIDTH_MEDIUM); then \
		SENTENCE=$(@:-accelerated-medium-sim=) catapult -product $(CATAPULT_PRODUCT) -shell -f ./rtl_sim_test.tcl; \
	else \
		echo "Run HLS before RTL-simulation"; \
	fi;
	date +%Y%m%d_%H%M%S >> simulation_time.txt

$(SENTENCE-accelerated-fast-sim): 
	date +%Y%m%d_%H%M%S > simulation_time.txt
	if test -e $(SYS_FILE_FAST); then \
		rm $(SYS_FILE_FAST) & \
	fi;
	@echo '   ' ;export RUN_ACCELERATED_SIM=1; export arch=ARC_FAST; export DMA_WIDTH=$(DMA_WIDTH_FAST);\
	if test -e ./ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST); then \
		SENTENCE=$(@:-accelerated-fast-sim=) catapult -product $(CATAPULT_PRODUCT) -shell -f ./rtl_sim_test.tcl; \
	else \
		echo "Run HLS before RTL-simulation"; \
	fi;
	date +%Y%m%d_%H%M%S >> simulation_time.txt
	mkdir -p test/fast/accuracy
	cp accelerated_data.txt test/fast/accuracy/accelerated_data_sim_$(@:-accelerated-fast-sim=).txt

######

test-SMALL:
	./test.sh boring small

test-MEDIUM:
	./test.sh boring medium

test-FAST:
	./test.sh boring fast

compile-SMALL:
	./compile_arch.sh boring small

compile-MEDIUM:
	./compile_arch.sh boring medium

compile-FAST:
	./compile_arch.sh boring fast

test-accuracy-SMALL:
	./test_accuracy.sh boring small

test-accuracy-MEDIUM:
	./test_accuracy.sh boring medium

test-accuracy-FAST:
	./test_accuracy.sh boring fast

sim_clean:
	@rm -rf sim_* *_data.txt *_names.txt

help: checkvars
	-@echo "Environment/Makefile Variables:"
	-@echo "  CATAPULT_HOME      = $(CATAPULT_HOME)"
	-@echo "  SYSTEMC_HOME       = $(SYSTEMC_HOME)"
	-@echo "  CONNECTIONS_HOME   = $(CONNECTIONS_HOME)"
	-@echo "  MATCHLIB_HOME      = $(MATCHLIB_HOME)"
	-@echo "  BOOST_HOME         = $(BOOST_HOME)"
	-@echo "  RAPIDJSON_HOME     = $(RAPIDJSON_HOME)"
	-@echo "  AC_TYPES           = $(AC_TYPES)"
	-@echo "  AC_SIMUTILS        = $(AC_SIMUTILS)"
	-@echo "  CXX                = $(CXX)"
	-@echo "  LIBDIRS            = $(LIBDIRS)"
	-@echo "  LD_LIBRARY_PATH    = $(LD_LIBRARY_PATH)"
	-@echo ""

clean: sim_clean
	@rm -rf *.vcd *.wlf *atap* wrapDir

clean-all: hls-clean-all-arch
	cd ./ffn_bias; make clean; rm -rf ffn_ARC_SMALL_sysc_catapult_dma*; rm -rf ffn_ARC_MEDIUM_sysc_catapult_dma*; rm -rf ffn_ARC_FAST_sysc_catapult_dma*;
	cd ./ffn_cfg; make clean; rm -rf ffn_ARC_SMALL_sysc_catapult_dma*; rm -rf ffn_ARC_MEDIUM_sysc_catapult_dma*; rm -rf ffn_ARC_FAST_sysc_catapult_dma*;
	cd ./ffn_ctrl; make clean; rm -rf ffn_ARC_SMALL_sysc_catapult_dma*; rm -rf ffn_ARC_MEDIUM_sysc_catapult_dma*; rm -rf ffn_ARC_FAST_sysc_catapult_dma*;
	cd ./ffn_tiling; make clean; rm -rf ffn_ARC_SMALL_sysc_catapult_dma*; rm -rf ffn_ARC_MEDIUM_sysc_catapult_dma*; rm -rf ffn_ARC_FAST_sysc_catapult_dma*;
	cd ./ffn_vec; make clean; rm -rf ffn_ARC_SMALL_sysc_catapult_dma*; rm -rf ffn_ARC_MEDIUM_sysc_catapult_dma*; rm -rf ffn_ARC_FAST_sysc_catapult_dma*;
	rm -f *.vstf

