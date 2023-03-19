CC=gcc
FLAGS=-c
all: main.o hash.o skip_list.o bloom_filter.o funcs.o
	$(CC) main.o hash.o skip_list.o bloom_filter.o funcs.o -o vaccineMonitor
bloom_filter.o: bloom_filter.c
	$(CC) $(FLAGS) bloom_filter.c
main.o: main.c
	$(CC) $(FLAGS) main.c
funcs.o: funcs.c
	$(CC) $(FLAGS) funcs.c
list.o: list.c
	$(CC) $(FLAGS) list.c
skip_list.o: skip_list.c
	$(CC) $(FLAGS) skip_list.c
hash.o: hash.c
	$(CC) $(FLAGS) hash.c
clean:
	rm *.o vaccineMonitor
