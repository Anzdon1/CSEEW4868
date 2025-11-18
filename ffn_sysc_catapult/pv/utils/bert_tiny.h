#ifndef _BERT_TINY_H_
#define _BERT_TINY_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_SEQ 32
#define D_MODEL 128
#define N_HEAD 2

char SEN_TYPE[10][50] = {"love", "happy", "great", "thoughtful", "confident", "slow", "boring", "noisy", "cringe", "hollow"};
char SEN[10][4096] = {
                        "A visually stunning rumination on love.",
                        "The film made me happy from start to end, and I enjoyed every single scene.",
                        "The actors did a great job, and I felt their emotions in every moment of the story.",
                        "Thoughtful writing elevates a simple story.",
                        "Beautiful score and confident direction.",
                        "I hated this movie a lot because the story was slow and nothing interesting ever happened.",
                        "The film made me feel upset and bored, and I almost wanted to walk out of the room.",
                        "A noisy mess with zero payoff.",
                        "The dialogue is cringe and the pacing is awful.",
                        "The plot drags and the characters feel hollow.",
                    };
int SEN_MASK[10] = {10, 19, 21, 11, 8, 19, 22, 10, 13, 12};


#define PI  3.14159265358979323846
#define A 0.044715

#endif