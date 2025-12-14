#Copyright (c) 2011-2025 Columbia University, System Level Design Group
#SPDX-License-Identifier: Apache-2.0

CATAPULT_PRODUCT = ultra
# export DMA_WIDTH = 64 #set to target DMA used when running the top level
include ../dma.mk
export ACCELERATOR = DUMMY
export TECH_PATH = DUMMY
export MEMTECH_PATH = DUMMY
export ESP_ROOT = ../../../../../../
export TECH = virtex7

all: hls
.PHONY: all

# hls:
# 	catapult -product $(CATAPULT_PRODUCT) -shell -f ./build_prj_top.tcl 
	
hls-test:
	@echo '';export arch=ARC_SMALL; export DMA_WIDTH=$(DMA_WIDTH_SMALL); \
	catapult -product $(CATAPULT_PRODUCT) -shell -f ./build_prj_top_test.tcl
.PHONY: hls

clean:
	rm -rf catapult_cache*
	rm -f catapult.pinfo
	rm -f trace.vcd
	rm -f transcript
	rm -f catapult.log
	rm -f vsim_stacktrace.vstf
	rm -rf Catapult* 
	if test -e ./ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL); then \
		rm -rf ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL); \
		rm -f ffn_ARC_SMALL_sysc_catapult_dma$(DMA_WIDTH_SMALL).ccs; \
		rm -f catapult*; \
	fi; 
	if test -e ./ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST); then \
		rm -rf ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST); \
		rm -f ffn_ARC_FAST_sysc_catapult_dma$(DMA_WIDTH_FAST).ccs; \
		rm -f catapult*; \
	fi; 
	
.PHONY: clean

distclean: clean
	@rm -rf $(RTL_OUT)
.PHONY: distclean
