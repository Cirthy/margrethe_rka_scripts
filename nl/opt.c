#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <immintrin.h>
#include "test_data.h"
#include "benchmark_util.h"
#include "subtt.h"

void print_vec(uint32_t * vec, size_t len){
  for (size_t i = 0; i < len -1; i++)
  {
    printf("%d, ", vec[i]);
  }
  printf("%d\n", vec[len-1]);
  
}

// MAX_REC_LEN defines the size (in the number of m512 registers) to stop the recursive algorithm and run the iteractive one. Optimizing it depends on the specific machine.
#define MAX_REC_LEN (1<<5)

// Iteractive Hadamard
void hadamard_iter(__m512 *p, size_t len){
  uint64_t divide = len >> 1;
  while (divide != 0){
    const uint64_t offset = divide;
    for (size_t i = 0; i < len; i+=(divide<<1)){
      for (size_t j = i; j < i + offset; j++){
        const __m512 x = p[j];
        const __m512 y = p[j + offset];
        p[j] = _mm512_add_ps (x, y);
        p[j + offset] = _mm512_sub_ps (x, y);
      }
    }
    divide >>= 1;
  }

  // divide = 8;
  const __m512i mask8_1 = _mm512_set_epi32(7, 6, 5, 4, 3, 2, 1, 0,
                                          7, 6, 5, 4, 3, 2, 1, 0);
  const __m512i mask8_2 = _mm512_set_epi32(15, 14, 13, 12, 11, 10, 9, 8,
                                         15, 14, 13, 12, 11, 10, 9, 8);
  const __m512 mask8_3 = _mm512_set_ps(-1, -1, -1, -1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1, 1, 1);
  for (size_t i = 0; i < len; i++){
    const __m512 x = _mm512_permutexvar_ps(mask8_1, p[i]);
    const __m512 y = _mm512_permutexvar_ps(mask8_2, p[i]);
    p[i] = _mm512_fmadd_ps(y, mask8_3, x);
  }

  // divide = 4;
  const __m512i mask4_1 = _mm512_set_epi32(11, 10, 9, 8, 11, 10, 9, 8, 3, 2, 1, 0, 3, 2, 1, 0);
  const __m512i mask4_2 = _mm512_set_epi32(15, 14, 13, 12, 15, 14, 13, 12, 7, 6, 5, 4, 7, 6, 5, 4);
  const __m512 mask4_3 = _mm512_set_ps(-1, -1, -1, -1, 1, 1, 1, 1, -1, -1, -1, -1, 1, 1, 1, 1);
  for (size_t i = 0; i < len; i++){
    const __m512 x = _mm512_permutexvar_ps(mask4_1, p[i]);
    const __m512 y = _mm512_permutexvar_ps(mask4_2, p[i]);
    p[i] = _mm512_fmadd_ps(y, mask4_3, x);
  }

  // divide = 2;
  const __m512i mask2_1 = _mm512_set_epi32(13, 12, 13, 12, 9, 8, 9, 8, 5, 4, 5, 4, 1, 0, 1, 0);
  const __m512i mask2_2 = _mm512_set_epi32(15, 14, 15, 14, 11, 10, 11, 10, 7, 6, 7, 6, 3, 2, 3, 2);
  const __m512 mask2_3 = _mm512_set_ps(-1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1);
  for (size_t i = 0; i < len; i++){
    const __m512 x = _mm512_permutexvar_ps(mask2_1, p[i]);
    const __m512 y = _mm512_permutexvar_ps(mask2_2, p[i]);
    p[i] = _mm512_fmadd_ps(y, mask2_3, x);
  }

  // divide = 1;
  const __m512i mask1_1 = _mm512_set_epi32(14, 14, 12, 12, 10, 10, 8, 8, 6, 6, 4, 4, 2, 2, 0, 0);
  const __m512i mask1_2 = _mm512_set_epi32(15, 15, 13, 13, 11, 11, 9, 9, 7, 7, 5, 5, 3, 3, 1, 1);
  const __m512 mask1_3 = _mm512_set_ps(-1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1);
  for (size_t i = 0; i < len; i++){
    const __m512 x = _mm512_permutexvar_ps(mask1_1, p[i]);
    const __m512 y = _mm512_permutexvar_ps(mask1_2, p[i]);
    p[i] = _mm512_fmadd_ps(y, mask1_3, x);
  }
}

// Recursive Hadamard with iteractive base case
void hadamard(__m512 *p, size_t len) {
  if(len <= MAX_REC_LEN) {
    hadamard_iter(p, len);
  } else {
    hadamard(p, len/2);
    hadamard(p+len/2, len/2);

    for(int i = 0; i < len/2; i++) {
      const __m512 x = p[i];
      const __m512 y = p[i + len/2];
      p[i] = _mm512_add_ps (x, y);
      p[i + len/2] = _mm512_sub_ps (x, y);
     }
  }
}

// 
void * safe_aligned_malloc(size_t size){
  void * ptr;
  int err = posix_memalign(&ptr, 64, size);
  if (err || (!ptr && (size > 0))) {
    perror("aligned malloc failed!");
    exit(EXIT_FAILURE);
  }
  return ptr;
}

// returns maximum(|current_maximum|, |hadamard(p, len)|) 
__m512 hadamard_max_with(__m512 current_maximum, __m512 *p, size_t len){
  __m512 max = current_maximum;
  hadamard(p, len);
  for (size_t i = 0; i < len; i++){
    // range with control 0b1011 = 0xb is absolute maximum with positive result
    max = _mm512_range_ps (max, p[i], 0xB);
  }
  return max;
}



//#define IN_SIZE (1<<18)
#define NB_VAR 18
#define TT_SIZE (1<<NB_VAR)
#define SUBTT_SIZE (1<<(NB_VAR))

void test_diff_tt(){
  uint32_t tt[TT_SIZE]  __attribute__ ((aligned(64)));
  uint32_t stt[SUBTT_SIZE]  __attribute__ ((aligned(64)));
  float diff_stt[SUBTT_SIZE]  __attribute__ ((aligned(64)));
  float diff_stt2[SUBTT_SIZE]  __attribute__ ((aligned(64)));
  tt_from_file(tt, "data/tts", atoi("0"));
  two_guesses_tt(stt, tt, 0, 0, 1, 0);
  diff_tt(diff_stt, stt, 1);
  diff_tt2(diff_stt2, stt, 1);
  for (size_t i = 0; i < SUBTT_SIZE; i++){
    printf("%d -> ", tt[i]);
    printf("%d -> ", stt[i]);
    printf("%f\n", diff_stt[i]);
    if(diff_stt[i] != diff_stt2[i]){
      printf(" failed %lu. Expected: %f\n", i, diff_stt2[i]);
      // exit(0);
    }
  }
  printf(" pass\n");
}

void bench_diff_tt(){
  uint32_t tt[TT_SIZE]  __attribute__ ((aligned(64)));
  uint32_t stt[SUBTT_SIZE]  __attribute__ ((aligned(64)));
  float diff_stt[SUBTT_SIZE]  __attribute__ ((aligned(64)));
  tt_from_file(tt, "data/tts", atoi("0"));
  two_guesses_tt(stt, tt, 0, 0, 1, 0);
  __m512 max = {0.};
  BENCH_LINE(
    diff_tt(diff_stt, stt, 1);
  );
  BENCH_LINE(
    max = hadamard_max_with(max, (__m512 *) diff_stt, SUBTT_SIZE/16);
  );

}

int main(int argc, char* argv[]) {
  // test_diff_tt(); return 0;
  // bench_diff_tt(); return 0;
  // Note: input must be aligned
  // use the "safe_aligned_malloc" function 
  // or static allocation with "__attribute__ ((aligned(64)))"
  uint32_t tt[TT_SIZE]  __attribute__ ((aligned(64)));
  uint32_t stt[SUBTT_SIZE]  __attribute__ ((aligned(64)));
  float diff_stt[SUBTT_SIZE]  __attribute__ ((aligned(64)));
  // uint32_t min_nl = (1ULL<<32) - 1;
  __m512 max = {0.};
  if(argc == 1) {
    printf("Give tt index\n");
    return 0;
  }
  tt_from_file(tt, "data/tts", atoi(argv[1]));
  uint32_t cnt = 0;
  for(uint32_t u = 1; u < SUBTT_SIZE; u++){
    diff_tt2(diff_stt, tt, u);
    max = hadamard_max_with(max, (__m512 *) diff_stt, SUBTT_SIZE/16);
    // printf("Maximum %f\n", _mm512_reduce_max_ps (max));

  }
  printf("Progress: %d - ", cnt++);
  printf("Maximum %f\n", _mm512_reduce_max_ps (max));

  /* Main before Clement's commit
  float input[IN_SIZE]  __attribute__ ((aligned(64)));
  __m512 max = {0.};
  for (size_t i = 0; i < IN_SIZE; i++){
    input[i] = i;
  }
  max = hadamard_max_with(max, (__m512 *) input, IN_SIZE/16);
  printf("Maximum %f\n", _mm512_reduce_max_ps (max));*/
  // uncomment to check result with the one produce by the python script (run the python script with the same input before)
  // for (size_t i = 0; i < IN_SIZE; i++){
  //   if(a[i] != __expected_out[i]){
  //     printf("(%ld) Result: %f Expected: %f\n", i, a[i],  __expected_out[i]);
  //     exit(0);
  //   }
  // }
  // printf("Test Pass\n");
}


