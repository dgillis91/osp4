IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=../obj

_DEPS = parse.h semutil.h shmutil.h pclock.h util.h procutil.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o parse.o shmutil.o semutil.o pclock.o util.o procutil.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_USER = user.o parse.o shmutil.o semutil.o pclock.o util.o procutil.o
USER = $(patsubst %,$(ODIR)/%,$(_USER))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

oss: $(OBJ)
	$(CC) -o ../bin/$@ $^ $(CFLAGS)

user: $(USER)
	$(CC) -o ../bin/$@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
