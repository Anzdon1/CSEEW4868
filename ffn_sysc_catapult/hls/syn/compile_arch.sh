#!/bin/bash

# Usage:
# ./test.sh <image> <arch> <layer>
#   e.g. ./test.sh cat SMALL TARGET_LAYER_1

source ./dma.mk

if [[ $# < 2 ]] ; then
    echo 'usage: ./test1.sh <sentence> <arch> '
    echo ' e.g.: ./test1.sh boring ARC_SMALL'
    exit 1
fi

# Sentence (boring, etc.)
sentence_var=$1

# Arch (SMALL, MEDIUM, FAST)
arch=$2
arch_name=""
DMA_WIDTH=""

if [ $arch == "small" ]; then
    arch_name="ARC_SMALL"
    DMA_WIDTH=${DMA_WIDTH_SMALL}
elif [ $arch == "medium" ]; then
    arch_name="ARC_MEDIUM"
    DMA_WIDTH=${DMA_WIDTH_MEDIUM}
elif [ $arch == "fast" ]; then
    arch_name="ARC_FAST"
    DMA_WIDTH=${DMA_WIDTH_FAST}
else
    echo "Unknown arch $arch"
    exit 1
fi


rm -rf test/${arch}

# mkdir -p test
mkdir -p test/${arch}/accuracy

LOGFILE=test/${arch}/test_${arch_name}.log

if test -e $LOGFILE; then
    mv $LOGFILE $LOGFILE.bak
fi

echo "======================" >> $LOGFILE
echo "=== Automated test ===" >> $LOGFILE
echo "======================" >> $LOGFILE
echo "" >> $LOGFILE

echo "=== BEHAV sim ===" >> $LOGFILE

# check the correctness of all classification
# SENTENCE_LIST="boring confident cringe great happy hollow love noisy slow thoughtful"
# for sentence_local in $SENTENCE_LIST; do
#     timeout --signal=KILL 5m make ${sentence_local}-${arch}-exe > test/${arch}/accuracy/test_beh_${arch_name}_${sentence_local}.log
#     grep -i "Validation succeeded for total" test/${arch}/accuracy/test_beh_${arch_name}_${sentence_local}.log
#     r=$?
#     if [ $r -eq 1 ]; then
#         echo "BEHAV simulation of ${sentence_local} FAILED..." >> $LOGFILE
#         echo "" >> $LOGFILE
#         exit
#     else
#         echo "BEHAV simulation of ${sentence_local} PASSED" >> $LOGFILE
#     fi
# done

echo "finished the BEHAV simulation for all sentences. Please check the results under test/${arch}/accuracy/" >> $LOGFILE
echo "" >> $LOGFILE

# HLS
echo "=== ${arch} ===" >> $LOGFILE
echo "- HLS ${arch} ... " >> $LOGFILE
# touch hls_output.txt

# submodules ############
timeout --signal=KILL 1h make hls-bias-${arch} | tee test/${arch}/hls_bias_output.txt
grep -i "error" test/${arch}/hls_bias_output.txt
r=$?
if [ $r -eq 1 ]; then
    echo "HLS BIAS PASSED" >> $LOGFILE
else
    echo "HLS BIAS FAILED (skip simulation)" >> $LOGFILE
    echo "" >> $LOGFILE
    exit
fi

timeout --signal=KILL 1h make hls-cfg-${arch} | tee test/${arch}/hls_cfg_output.txt
grep -i "error" test/${arch}/hls_cfg_output.txt
r=$?
if [ $r -eq 1 ]; then
    echo "HLS CFG PASSED" >> $LOGFILE
else
    echo "HLS CFG FAILED (skip simulation)" >> $LOGFILE
    echo "" >> $LOGFILE
    exit
fi

timeout --signal=KILL 1h make hls-ctrl-${arch} | tee test/${arch}/hls_ctrl_output.txt
grep -i "error" test/${arch}/hls_ctrl_output.txt
r=$?
if [ $r -eq 1 ]; then
    echo "HLS CTRL PASSED" >> $LOGFILE
else
    echo "HLS CTRL FAILED (skip simulation)" >> $LOGFILE
    echo "" >> $LOGFILE
    exit
fi

timeout --signal=KILL 1h make hls-tiling-${arch} | tee test/${arch}/hls_tiling_output.txt
grep -i "error" test/${arch}/hls_tiling_output.txt
r=$?
if [ $r -eq 1 ]; then
    echo "HLS TILING PASSED" >> $LOGFILE
else
    echo "HLS TILING FAILED (skip simulation)" >> $LOGFILE
    echo "" >> $LOGFILE
    exit
fi

timeout --signal=KILL 1h make hls-vec-${arch} | tee test/${arch}/hls_vec_output.txt
grep -i "error" test/${arch}/hls_vec_output.txt
r=$?
if [ $r -eq 1 ]; then
    echo "HLS VEC PASSED" >> $LOGFILE
else
    echo "HLS VEC FAILED (skip simulation)" >> $LOGFILE
    echo "" >> $LOGFILE
    exit
fi
#########################

timeout --signal=KILL 1h make hls-${arch} | tee test/${arch}/hls_output.txt
hls_log=ffn_${arch_name}_sysc_catapult_dma${DMA_WIDTH}/ffn_sysc_catapult.v1/messages.txt 
grep -i "error" $hls_log
r=$?
if [ $r -eq 1 ]; then
    echo "HLS MAIN PASSED" >> $LOGFILE
else
    echo "HLS MAIN FAILED (skip simulation)" >> $LOGFILE
    echo "" >> $LOGFILE
    exit
fi
rm test/${arch}/hls_output.txt

echo "" >> $LOGFILE
# echo "== RTL sim ==" >> $LOGFILE

# RTL accelerated
# echo -n "- RTL ${arch} ... " >> $LOGFILE
# timeout --signal=KILL 60m make ${sentence_var}-accelerated-${arch}-sim | tee -a test/${arch}/test_rtl_${arch_name}_${sentence_var}.log
# r=$?
# if [ $r -eq 0 ]; then
#     echo "RTL ${arch} PASSED" >> $LOGFILE
# else
#     echo "RTL ${arch} FAILED (Aborting)" >> $LOGFILE
#     exit
# fi

# echo "" >> $LOGFILE

# # collecting metrics
# bash get_latency_group.sh
# bash get_area_group.sh
