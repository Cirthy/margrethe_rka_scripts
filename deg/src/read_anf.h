#ifndef READ_ANF_H
#define READ_ANF_H

#include "degree.h"


int ascii_to_hex(unsigned char c);
uint32_t read_mon_from_file(FILE *fd);
Anf read_anf(int n, int min_d, char *path);


#endif
