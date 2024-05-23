#ifndef DEGREE_H
#define DEGREE_H


typedef struct {
	uint32_t **sorted_monomials;
	int *length;
	int n;
	int min_d; 
} Anf;


typedef struct {
	int deg;
	uint32_t u;
	int i;
	int v_i;
	int j;
	int v_j;
} Deg_ind;


uint32_t *new_monoms_diff(int new_monoms_length, uint32_t u);
void add_monomial(Anf* anf, uint32_t mon);
Anf diff_anf(const Anf* anf, uint32_t u);
Deg_ind min_degree_two_guesses(Anf *anf);
Deg_ind min_degree_two_guesses_diff_timing(const Anf *anf);
int degree_two_guesses(const Anf *anf, int i, int v_i, int j, int v_j);


#endif
