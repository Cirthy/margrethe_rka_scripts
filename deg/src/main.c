#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>


#include "degree.h"
#include "utils.h"
#include "read_anf.h"


int min_nb_mon(const Anf *anf) {
	Anf current_anf;
	int cnt = 0;
	int nb_mon = 1<<anf->n;
	int j;

//	clock_t start, end;
//	double time;
	int tick_display = 4;

//	start = clock();
	for(int u = 1; u < 1<<anf->n; u++) {
		if(u % (1<<tick_display) == 0) {
//			end = clock();
//			time = ((double) (end - start)) / CLOCKS_PER_SEC;
			printf("%d\n", nb_mon);
			//printf("%d/%d\n\tTime spent: %fs\n\tExpected total time: %fs\n", u / (1<<tick_display), 1<<(anf->n - tick_display), time, (1<<(anf->n - tick_display)) * time);
//			start = clock();
		}
		current_anf = diff_anf(anf, u);
		for(int d = 0; d <= anf->n; d++) {
			j = 0;
			while(anf->sorted_monomials[d][j] != 0 && j < anf->length[d]) {
				j++;
				cnt++;
			}
		}
		if(cnt < nb_mon)
			nb_mon = cnt;
		free_anf(&current_anf);
	}
	return cnt;
}


int main(int argc, char *argv[])
{
	if(argc == 1) {
		printf("Provide argument\n");
		return 0;
	}

	char filename[14] = "../data/anf_";
	
	strcat(filename, argv[1]);

/*	# monomials: Estimated time = 20-40 days per component, may be optimized with AVX512 implementation of 
	Anf anf = read_anf(18, 0, filename);
	printf("ANF loaded\n");

	int nb_mon = min_nb_mon(&anf);
	printf("Lower number of monomials: %d\n", nb_mon);
*/

//	Degree: Estimated time = 20-40 hours per component
	Anf anf = read_anf(18, 13, filename);
	printf("ANF loaded\n");

	Deg_ind ret = min_degree_two_guesses_diff_timing(&anf);	
	display_deg_ind(&ret);

	free_anf(&anf);
	return 0;
}
