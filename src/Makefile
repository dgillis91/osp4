IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=../obj

_DEPS = semutil.h shmutil.h pclock.h proc_table.h pcb.h util.h sharedvals.h procq.h sched.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = oss.o shmutil.o semutil.o pclock.o proc_table.o pcb.o util.o procq.o sched.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_USER = user.o shmutil.o semutil.o pclock.o proc_table.o pcb.o util.o procq.o sched.o
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
