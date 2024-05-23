#ifndef SUBTT_H
#define SUBTT_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <immintrin.h>
#include <assert.h>

#define SIZE 18

void tt_from_file(uint32_t *ret, char *filename, int pos);
void diff_tt(float *ret_tt, uint32_t *tt, uint32_t u);
void diff_tt2(float *ret_tt, uint32_t *tt, uint32_t u);
void one_guess_tt(uint32_t *ret_tt, uint32_t *tt, int i, int v_i);
void two_guesses_tt(uint32_t *ret_tt, uint32_t *tt, int i, int v_i, int j, int v_j);


#endif
