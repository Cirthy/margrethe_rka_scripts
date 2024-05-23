CC = gcc
FLAGS = -Wall -Wno-unused-function -Wno-unused-result -funroll-all-loops -march=native -lm
OPT_FLAGS = -O1 -fwhole-program -flto  $(FLAGS)

all: opt 

opt: opt.c subtt.c
	$(CC) -g -o opt $^ $(OPT_FLAGS) 

ref: ref.c
	$(CC) -g -o ref $^ $(OPT_FLAGS)  