#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "degree.h"
#include "utils.h"
#include "read_anf.h"



// Returns the sorted list of u' such that u_i = 1 => u'_i = 1
uint32_t *new_monoms_diff(int new_monoms_length, uint32_t u) {
	int offset = 0, ret_offset = 0;
	uint32_t *new_monoms = calloc(new_monoms_length, sizeof(uint32_t));

	while(u) {
		while((u & 0x1) == 0) {
			offset ++;
			u >>= 1;
		}
		for(int i = 0; i < new_monoms_length; i++)
			if(i & (0x1 << ret_offset))
				new_monoms[i] += (1 << offset);
		ret_offset ++;
		offset++;
		u >>= 1;
	}
	return new_monoms;
}


void add_monomial(Anf* anf, uint32_t mon) {
	if(!mon)
		return;
	int d = __builtin_popcount(mon);
	if(d < anf->min_d)
		return;
	int ind = int_index(anf->sorted_monomials[d - anf->min_d], anf->length[d - anf->min_d], mon);

	if(ind >= 0)
		anf->sorted_monomials[d - anf->min_d][ind] = 0;
	else
		anf->sorted_monomials[d - anf->min_d][-ind - 1] = mon;
}


Anf diff_anf(const Anf* anf, uint32_t u) {
	int new_monoms_length = 1 << __builtin_popcount(u);
	uint32_t *sub_diffs = new_monoms_diff(new_monoms_length, u);
	Anf ret_anf = copy_anf(anf);

	for(int d = anf->n - anf->min_d; d >= 0; d--)
		for(int i = 0; i < anf->length[d]; i++)
			if(anf->sorted_monomials[d][i])
				for(int m = 0; m < new_monoms_length; m++)
					if((sub_diffs[m] & (~anf->sorted_monomials[d][i])) == 0)
						add_monomial(&ret_anf, anf->sorted_monomials[d][i] & ~sub_diffs[m]);
	free(sub_diffs);
	return ret_anf;
}


Deg_ind min_degree_two_guesses_diff(const Anf *anf) {
	Anf current_anf;
	Deg_ind ret_deg;
	int current_deg;

	ret_deg.deg = anf->n;
	for(int u = 1; u < 1<<anf->n; u++) {
		current_anf = diff_anf(anf, u);
		for(int i = 0; i < anf->n; i++)
				for(int v_i = 0; v_i < 2; v_i++)
					for(int j = i + 1; j < anf->n; j++)
						for(int v_j = 0; v_j < 2; v_j++) {
							current_deg = degree_two_guesses(&current_anf, i, v_i, j, v_j);
							if(current_deg < ret_deg.deg)
								set_deg_ind(&ret_deg, current_deg, u, i, v_i, j, v_j);		
						}
		free_anf(&current_anf);
	}
	return ret_deg;
}


Deg_ind min_degree_two_guesses_diff_timing(const Anf *anf) {
	Anf current_anf;
	Deg_ind ret_deg;
	int current_deg;
	int cnt = 0;
	clock_t start, end;
	double time;
	int tick_display = 14;

	ret_deg.deg = anf->n;
	start = clock();
	for(int u = 1; u < 1<<anf->n; u++) {
		if(u % (1<<tick_display) == 0) {
			end = clock();
			time = ((double) (end - start)) / CLOCKS_PER_SEC;
			printf("%d - u: %x\n\tTime spent: %fs\n\tExpected total time: %fs\n", cnt, u, time, (1<<(anf->n - tick_display)) * time);
			cnt++;
			start = clock();
		}

		current_anf = diff_anf(anf, u);
		for(int i = 0; i < anf->n; i++)
				for(int v_i = 0; v_i < 2; v_i++)
					for(int j = i + 1; j < anf->n; j++)
						for(int v_j = 0; v_j < 2; v_j++) {
							current_deg = degree_two_guesses(&current_anf, i, v_i, j, v_j);
							if(current_deg < ret_deg.deg)
								set_deg_ind(&ret_deg, current_deg, u, i, v_i, j, v_j);		
						}
		free_anf(&current_anf);
	}
	return ret_deg;
}


int degree_two_guesses(const Anf *anf, int i, int v_i, int j, int v_j) {
	int i_in_mon, j_in_mon;
	uint32_t tmp;
	Anf anf_cp = copy_anf(anf);

	for(int d = anf->n - anf->min_d; d >= 0; d--) {
		for(int k = 0; k < anf->length[d]; k++) {
			if(anf_cp.sorted_monomials[d][k]) {
				i_in_mon = anf_cp.sorted_monomials[d][k] >> i & 0x1;
				j_in_mon = anf_cp.sorted_monomials[d][k] >> j & 0x1;
				if(!i_in_mon & !j_in_mon) {
					free_anf(&anf_cp);
					return d + anf->min_d;
				}
				if((!i_in_mon || v_i) && (!j_in_mon || v_j)) {
					tmp = anf_cp.sorted_monomials[d][k] ^ ((i_in_mon && v_i) ? 1<<i : 0) ^ ((j_in_mon && v_j) ? 1<<j : 0);
					add_monomial(&anf_cp, tmp);
				}
				anf_cp.sorted_monomials[d][k] = 0;
			}
		}
	}
	free_anf(&anf_cp);
	return 0;
}
