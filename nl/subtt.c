#include "subtt.h"


void tt_from_file(uint32_t *ret, char *filename, int pos) {
	FILE *fd = fopen(filename, "r");
	char c;
	uint32_t cnt = 0;

	while(pos != 0)
		if((c = fgetc(fd)) == '\n')
			pos--;

	while((c = fgetc(fd)) != '\n')
		ret[cnt++] = (c - '0');
}


void diff_tt2(float *ret_tt, uint32_t *tt, uint32_t u) {
	uint32_t mod_buf = (1ULL<<SIZE) - 1;

	for(int i = 0; i < 1<<(SIZE-1); i++)
		ret_tt[i] = (float) (tt[i] ^ tt[(i + u) & mod_buf]);
}


void diff_tt(float *ret_tt, uint32_t *tt, uint32_t u) {
	uint32_t mod_buf = (1ULL<<(SIZE - 2)) - 1;
	// const uint32_t mod_bufv = mod_buf >> 4;
	const int64_t loop_size = 1ULL<<(SIZE-2);
	assert(loop_size > 16);
	__m512i * ttv = (__m512i *) tt;
	__m512i * ret_ttv = (__m512i *) ret_tt;

	for(int i = 0; i < loop_size/16; i++){
		ret_ttv[i] = _mm512_xor_epi32(ttv[i], _mm512_loadu_epi32(&tt[(i*16 + u) & mod_buf]));
	}
} 


void one_guess_tt(uint32_t *ret_tt, uint32_t *tt, int i, int v_i) {
	int cnt = 0;

	for(int k = 0; k < (1ULL<<SIZE); k++){
		if(((k>>i) & 0x1) == v_i){
			ret_tt[cnt++] = tt[k];
		}
	}
	
	assert(cnt == (1ULL<<(SIZE-1)));
}
 


void two_guesses_tt(uint32_t *ret_tt, uint32_t *tt, int i, int v_i, int j, int v_j) {
	int cnt = 0;

	for(int k = 0; k < (1ULL<<SIZE); k++){
		if((((k>>i) & 0x1) == v_i) && (((k>>j) & 0x1) == v_j)){
			ret_tt[cnt++] = tt[k];
		}
	}
	
	assert(cnt == (1ULL<<(SIZE-2)));
}
