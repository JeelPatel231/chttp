CC := gcc
OUT_DIR := build

CFLAGS := -ggdb

all: log_config mk_out_dir main

# tasks for logging info
log_config:
	@echo Using ${CC} as C Compiler

# makes required directories silently
mk_out_dir:
	@echo making Output Directory
	mkdir -p ${OUT_DIR}

main: main.c
	${CC} main.c ${CFLAGS} -o ${OUT_DIR}/main

clean:
	rm -rvf build
