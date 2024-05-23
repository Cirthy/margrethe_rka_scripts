IDIR = ./src
ODIR = ./src

CC = gcc
CFLAGS = -Wall -Werror -Wextra -I$(IDIR)

_DEPS = utils.h degree.h read_anf.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = utils.o degree.o read_anf.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

deg: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

run: deg
	./deg

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~

fclean: clean
	rm test
