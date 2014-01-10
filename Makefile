# This is Makefile for mpi-ping-pong-bench.
# Copyright(c) 2014 Sugizaki Yukimasa
# Licenced in GPL

PROG=mpi-ping-pong-bench
OBJS=mpi-ping-pong-bench.c.o
ALLDEPENDS+=$(MAKEFILE_LIST)
TOCLEAN+=res-*.txt

CC=mpicc
RM:=rm -f
MPIEXEC:=mpiexec

MEMSIZE:=100000

MPIEXEC_ARGS:=-n 2 ./$(PROG) $(MEMSIZE)

all: $(PROG)

$(PROG): $(OBJS)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) $(OUTPUT_OPTION)

%.c.o: %.c $(ALLDEPENDS)
	$(COMPILE.c) $< $(OUTPUT_OPTION)

.PHONY: clean
clean:
	$(RM) $(PROG)
	$(RM) $(OBJS)
	$(RM) $(TOCLEAN)

run: $(PROG)
	$(MPIEXEC) $(MPIEXEC_ARGS)
