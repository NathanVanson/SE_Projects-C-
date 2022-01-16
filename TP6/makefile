CC = gcc
CFLAGS = -g
LIBFLAGS = 
JOBS = jobs 
BUILTIN = builtin

shell: shell.o jobs.o builtin.o
	@$(CC) -o $@ $^ $(LIBFLAGS)

%.o: %.c
	@$(CC) -o $@ -c $^ $(CFLAGS)

.PHONY: clean

clean:
	@rm -rf *.o