outfile=wc

CC = gcc
CCFLAGS = -Wall -O0

SOURCES = wc.c utils.c

all:
	@mkdir -p bin log test
	@$(CC) $(CCFLAGS) $(SOURCES) -o $(outfile)
	@echo "Built "bin/$(app).$(version)

clean:
	@rm -rf bin log init
