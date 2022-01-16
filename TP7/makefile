CC = gcc
CFLAGS = -g
LIBFLAGS = -pthread -lrt
WAITER = waiter
COOK = cook

all: $(WAITER) $(COOK)

waiter: waiter.o
	@$(CC) -o $@ $^ $(LIBFLAGS)

cook: cook.o
	@$(CC) -o $@ $^ $(LIBFLAGS)

%.o: %.c
	@$(CC) -o $@ -c $^ $(CFLAGS)

.PHONY: clean

clean:
	@rm -rf *.o