#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "degree.h"
#include "utils.h"



void display_mon(uint32_t mon) {
	int i = 0;

	while(mon) {
		if(mon & 0x1)
			printf("X%d", i);
		i++;
		mon >>= 1;
	}
}


void display_anf(Anf *anf) {
	int first = 1;

	for(int d = anf->n - anf->min_d; d >= 0; d--) {
		for(int i = 0; i < anf->length[d]; i++) {
			if(anf->sorted_monomials[d][i]) {
				if(!first)
					printf(" + ");
				first = 0;
				display_mon(anf->sorted_monomials[d][i]);
			}
		}
	}
	printf("\n");
}


void display_deg_ind(Deg_ind *deg_ind) {
	printf("Deg = %d\nU = %x\nX%d = %d\nX%d = %d\n", deg_ind->deg, deg_ind->u, deg_ind->i, deg_ind->v_i, deg_ind->j, deg_ind->v_j);
}


int binom(int n, int k) {
	int res = 1;

	if(k > n - k)
		k = n - k;
	for(int i = 0; i < k; i++) {
		res *= (n - i);
		res /= (i + 1);
	}
	return res;
}


// Warning: kinda specific to add_monomial
int int_index(const uint32_t *l, int length, uint32_t e) {
	int first_zero = 0;
	for(int i = 0; i < length; i++)
		if(l[i] == e)
			return i;
		else if(!first_zero && l[i] == 0)
			first_zero = i + 1;
	return -first_zero;
}


void set_deg_ind(Deg_ind *deg_ind, int deg, uint32_t u, int i, int v_i, int j, int v_j) {
	deg_ind->deg = deg;
	deg_ind->u = u;
	deg_ind->i = i;
	deg_ind->v_i = v_i;
	deg_ind->j = j;
	deg_ind->v_j = v_j;
}


// =============== ANF manipulation ===============


Anf new_anf(int n, int min_d) {
	Anf anf;

	anf.n = n;
	anf.min_d = min_d;
	anf.sorted_monomials = (uint32_t**)malloc(sizeof(uint32_t*) * (anf.n - anf.min_d + 1));
	anf.length = (int*)malloc(sizeof(int) * (anf.n - anf.min_d + 1));
	for(int d = 0; d < anf.n - anf.min_d + 1; d++) {
		anf.length[d] = binom(n, d + anf.min_d);
		anf.sorted_monomials[d] = (uint32_t*)calloc(anf.length[d], sizeof(uint32_t));
	}

	return anf;
}


void free_anf(Anf *anf) {
	for(int d = 0; d <= anf->n - anf->min_d; d++) {
		free(anf->sorted_monomials[d]);
	}
	free(anf->sorted_monomials);
	free(anf->length);
}


Anf copy_anf(const Anf *anf) {
	Anf ret_anf = new_anf(anf->n, anf->min_d);

	for(int d = 0; d < anf->n - anf->min_d + 1; d++) {
		ret_anf.length[d] = anf->length[d];
		for(int i = 0; i < anf->length[d]; i++)
			ret_anf.sorted_monomials[d][i] = anf->sorted_monomials[d][i];
	}

	return ret_anf;
}
