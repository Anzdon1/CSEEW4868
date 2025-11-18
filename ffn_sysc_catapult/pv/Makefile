# ----------------- Config -----------------
SENTENCE ?= love
INPUT    ?= ../data/$(SENTENCE).bin

CXX      ?= g++
CPPFLAGS += -I./utils
CXXFLAGS ?= -O0 -g -Wall -Werror -fmax-errors=5 -std=c++11
LDLIBS   += -lm  

# ----------------- Sources ----------------
SRCS := bert.c
BINS := $(SRCS:.c=)
HDRS := $(wildcard ./utils/*.h) $(wildcard ./*.h)

# ----------------- Build ------------------
all: $(BINS)

%: %.c $(HDRS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< -o $@ $(LDFLAGS) $(LDLIBS)

# ----------------- Data -------------------
BERT_PRETRAINED := ../models/classifier_b.bin \
                   ../models/classifier_w_t.bin \
                   ../models/dense_b.bin \
                   ../models/dense_w_t.bin \
                   ../models/ffn1_b.bin \
                   ../models/ffn1_w_t.bin \
                   ../models/ffn2_b.bin \
                   ../models/ffn2_w_t.bin \
                   ../models/key_b.bin \
                   ../models/key_w_t.bin \
                   ../models/pooler_b.bin \
                   ../models/pooler_w_t.bin \
                   ../models/query_b.bin \
                   ../models/query_w_t.bin \
                   ../models/value_b.bin \
                   ../models/value_w_t.bin

data: $(INPUT) $(BERT_PRETRAINED)
.PHONY: data

bert-data: data  # example per-target data dep

# ----------------- Run --------------------
RUNS := $(SRCS:.c=-run)

$(RUNS): %-run : %-data %
	./$(@:-run=) $(INPUT)

# ----------------- Clean ------------------
clean:
	rm -f $(BINS) out_*_pvc.txt

distclean: clean

.PHONY: all clean distclean $(RUNS)
