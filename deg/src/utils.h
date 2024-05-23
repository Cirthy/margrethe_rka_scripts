#ifndef UTILS_H
#define UTILS_H

#include "degree.h"

void display_mon(uint32_t mon);
void display_anf(Anf *anf);
void display_deg_ind(Deg_ind *deg_ind);
int binom(int n, int k);
int int_index(const uint32_t *l, int length, uint32_t e);
void set_deg_ind(Deg_ind *deg_ind, int deg, uint32_t u, int i, int v_i, int j, int v_j);

Anf new_anf(int n, int min_d);
void free_anf(Anf *anf);
Anf copy_anf(const Anf *anf);


#endif
