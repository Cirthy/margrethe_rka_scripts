#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "read_anf.h"
#include "utils.h"
#include "degree.h"



//adapted from https://stackoverflow.com/questions/18693841/read-contents-of-a-file-as-hex-in-c
int ascii_to_hex(unsigned char c) {
        int num = (int) c;
        if(num < 58 && num > 47)
            return num - 48; 
        if(num < 103 && num > 96)
            return num - 87;
        return num;
}


uint32_t read_mon_from_file(FILE *fd) {
	uint32_t mon = 0;
	char c = fgetc(fd);

	while(c != '\n' && c != EOF) {
		mon = (mon << 4) + ascii_to_hex(c);
		c = fgetc(fd);
	}

	return mon;
}


Anf read_anf(int n, int min_d, char *path) {
	Anf ret_anf = new_anf(n, min_d);
	uint32_t mon;

	FILE *fd = fopen(path, "r");
	if(fd == NULL) {
		printf("File not found in read_anf\n");
		return ret_anf;
	}
	while((mon = read_mon_from_file(fd)))
		add_monomial(&ret_anf, mon);


	return ret_anf;
}
