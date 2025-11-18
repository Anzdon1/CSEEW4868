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
    LATENCY_TRACE="${PWD}/ffn_${arc}_sysc_catapult_dma${DMA_WIDTH}/ffn_sysc_catapult.v1/scverify/concat_sim_ffn_sysc_catapult_v_msim/sim.log"
    LATENCY_LOG="${PWD}/test/${arch}/reports/latency_ffn_${arc}_sysc_catapult_dma${DMA_WIDTH}.log"

    if test ! -e $LATENCY_TRACE; then
	echo "--- latency trace log of $arc not found ---"
	continue
    fi

    # LATENCY=$(cat $LATENCY_TRACE | grep "testbench_inst #1" |  sed 's/\s\+/ /g'  | cut -d " " -f 2)

    # start time and end time
    start_line=$(grep "=== TEST BEGIN ===" "$LATENCY_TRACE" | head -n 1)
    end_line=$(grep "testbench operating accelerator completed" "$LATENCY_TRACE" | tail -n 1)

    if [ -z "$start_line" ] || [ -z "$end_line" ]; then
        echo "Error: Could not find start or end markers in log file."
        exit 1
    fi

    # value extraction
    start_value=$(echo "$start_line" | awk '{print $2}')
    start_unit=$(echo "$start_line" | awk '{print $3}')

    end_value=$(echo "$end_line" | awk '{print $2}')
    end_unit=$(echo "$end_line" | awk '{print $3}')

    # convert to ns
    convert_to_ns() {
        value=$1
        unit=$2
        case "$unit" in
            ns) echo "$value" ;;
            us) awk "BEGIN {print $value * 1000}" ;;
            ms) awk "BEGIN {print $value * 1000000}" ;;
            s)  awk "BEGIN {print $value * 1000000000}" ;;
            *)  echo "0" ;;
        esac
    }

    start_ns=$(convert_to_ns "$start_value" "$start_unit")
    end_ns=$(convert_to_ns "$end_value" "$end_unit")

    # runtime calculation
    runtime_ns=$(awk "BEGIN {print $end_ns - $start_ns}")

    # display results in both ns and us
    # runtime_us=$(awk "BEGIN {print $runtime_ns / 1000}")
    # echo "Simulation runtime: ${runtime_ns} ns (${runtime_us} us)"

    runtime_us=$(awk "BEGIN {print $runtime_ns / 1000}")
    echo "Simulation runtime: ${runtime_ns} us"

    LATENCY=$runtime_ns

    rm -f $LATENCY_LOG
    echo "$LATENCY" >> $LATENCY_LOG

done
