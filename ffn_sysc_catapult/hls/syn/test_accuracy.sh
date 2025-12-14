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

# has Negative vibe TT
# has Positive vibe 0_<


# check the correctness of all classification
SENTENCE_LIST="boring confident cringe great happy hollow love noisy slow thoughtful"
# Positive sentences: confident, great, happy, love, thoughtful
# Negative sentences: boring, cringe, hollow, noisy, slow

total=0
correct=0

for sentence_local in $SENTENCE_LIST; do
    total=$((total + 1))
    echo "Testing ${sentence_local}..."
    timeout --signal=KILL 5m make ${sentence_local}-${arch}-exe > test/${arch}/accuracy/test_beh_${arch_name}_${sentence_local}.log 2>&1
    
    # Check if sentence is positive or negative
    if [[ "$sentence_local" == "confident" || "$sentence_local" == "great" || \
          "$sentence_local" == "happy" || "$sentence_local" == "love" || \
          "$sentence_local" == "thoughtful" ]]; then
        # Positive sentence - should have "has Positive vibe"
        grep -i "has Positive vibe" test/${arch}/accuracy/test_beh_${arch_name}_${sentence_local}.log > /dev/null 2>&1
        r=$?
        if [ $r -eq 0 ]; then
            correct=$((correct + 1))
            msg="BEHAV simulation of ${sentence_local} PASSED (Positive vibe)"
            echo "$msg" | tee -a $LOGFILE
        else
            msg="BEHAV simulation of ${sentence_local} FAILED (expected Positive vibe)"
            echo "$msg" | tee -a $LOGFILE
        fi
    else
        # Negative sentence - should have "has Negative vibe"
        grep -i "has Negative vibe" test/${arch}/accuracy/test_beh_${arch_name}_${sentence_local}.log > /dev/null 2>&1
        r=$?
        if [ $r -eq 0 ]; then
            correct=$((correct + 1))
            msg="BEHAV simulation of ${sentence_local} PASSED (Negative vibe)"
            echo "$msg" | tee -a $LOGFILE
        else
            msg="BEHAV simulation of ${sentence_local} FAILED (expected Negative vibe)"
            echo "$msg" | tee -a $LOGFILE
        fi
    fi
done

# Calculate accuracy percentage
# Multiply by 100 first to avoid integer division, then divide by total
accuracy_percent=$((correct * 100 / total))
echo "" | tee -a $LOGFILE
msg="Accuracy: $correct/$total = ${accuracy_percent}%"
echo "$msg" | tee -a $LOGFILE

# Check if accuracy is >= 70%
if [ $accuracy_percent -ge 70 ]; then
    msg="BEHAV simulation PASSED (accuracy >= 70%)"
    echo "$msg" | tee -a $LOGFILE
else
    msg="BEHAV simulation FAILED (accuracy < 70%)"
    echo "$msg" | tee -a $LOGFILE
    exit 1
fi

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


