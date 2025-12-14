#!/bin/bash

ARCHS="ARC_SMALL ARC_FAST"
source ./dma.mk

for arc in $ARCHS; do

    arch=""
    # Get area of each architecture
    if [ $arc == "ARC_SMALL" ]; then
        DMA_WIDTH=$DMA_WIDTH_SMALL
        arch="small"
    elif [ $arc == "ARC_FAST" ]; then
        DMA_WIDTH=$DMA_WIDTH_FAST
        arch="fast"
    fi

    mkdir -p ${PWD}/test/${arch}/reports
    AREA_TRACE="${PWD}/ffn_${arc}_sysc_catapult_dma${DMA_WIDTH}/ffn_sysc_catapult.v1/vivado_v/utilization_synth.rpt"
    AREA_LOG="${PWD}/test/${arch}/reports/area_ffn_${arc}_sysc_catapult_dma${DMA_WIDTH}.log"

    if test ! -e $AREA_TRACE; then
        echo "--- area trace log of $arc not found ---"
        continue
    fi

    LUTS=$(cat $AREA_TRACE | grep "Slice LUTs\*" |  sed 's/\s\+/ /g'  | cut -d " " -f 5)
    FFS=$(cat $AREA_TRACE | grep "Register as Flip Flop" |  sed 's/\s\+/ /g'  | cut -d " " -f 7)
    FFS1=$(cat $AREA_TRACE | grep "Register as Latch" |  sed 's/\s\+/ /g'  | cut -d " " -f 6)
    MULTS=$(cat $AREA_TRACE | grep "DSPs" | head -1 | sed 's/\s\+/ /g'  | cut -d " " -f 4)

    RAM18=$(cat $AREA_TRACE | grep "RAMB18 " | head -1 | sed 's/\s\+/ /g'  | cut -d " " -f 4)
    RAM36=$(cat $AREA_TRACE | grep "RAMB36/FIFO*" | head -1 | sed 's/\s\+/ /g'  | cut -d " " -f 4)

    # Set default values if variables are empty
    LUTS=${LUTS:-0}
    FFS=${FFS:-0}
    FFS1=${FFS1:-0}
    MULTS=${MULTS:-0}
    RAM18=${RAM18:-0}
    RAM36=${RAM36:-0}

    # Debug output
    echo "LUTS=$LUTS"
    echo "FFS=$FFS"
    echo "FFS1=$FFS1"
    echo "MULTS=$MULTS"
    echo "RAM18=$RAM18"
    echo "RAM36=$RAM36"
    

    # area formula: BRAM*0.00142 + LUTs*0.00000434 + Mults*0.000578704
    AREA_LUTS=$(echo "$LUTS*0.00000085" | bc -l)
    AREA_FFS=$(echo "$FFS*0.00000042" | bc -l)
    AREA_FFS1=$(echo "$FFS1*0.00000042" | bc -l)
    AREA_MULTS=$(echo "$MULTS*0.0001462" | bc -l)

    # AREA_URAM=$(echo "$URAM*0.00104" | bc -l)
    AREA_RAM18=$(echo "$RAM18*0.00023148" | bc -l)
    AREA_RAM36=$(echo "$RAM36*0.00046296" | bc -l)


    TOT_AREA=$(echo "$AREA_RAM18 + $AREA_RAM36 + $AREA_LUTS + $AREA_MULTS + $AREA_FFS + $AREA_FFS1" | bc -l)
    AVG_AREA=$(echo "$TOT_AREA / 6" | bc -l)


    rm -f $AREA_LOG
    echo "$AVG_AREA" >> $AREA_LOG
    echo "$AREA_URAM" >> $AREA_LOG
    echo "$AREA_RAM18" >> $AREA_LOG
    echo "$AREA_RAM36" >> $AREA_LOG
    echo "$AREA_LUTS" >> $AREA_LOG
    echo "$AREA_MULTS" >> $AREA_LOG
    echo "$AREA_FFS" >> $AREA_LOG
    echo "$AREA_FFS1" >> $AREA_LOG

done
