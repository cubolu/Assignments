################################################################################
# Makefile                                                                     #
# Autor:       Jakub Lukac                                                     #
# Login:       xlukac09                                                        #
# Rocnik:      1. rocnik BIT                                                   #
# e-mail:      xlukac09@stud.fit.vutbr.cz                                      #
################################################################################
LOGIN=xlukac09

CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -pedantic -O2
CPP=g++
CXXFLAGS=-std=c++11 -pedantic -Wall


CFILES=$(wildcard *.c)
CPPFILES=$(wildcard *.cc)
HEADERS=$(wildcard *.h)
ALLFILES=$(CFILES) $(CPPFILES) $(HEADERS) Makefile
OBJS=$(CFILES:.c=.o)

# make project
ALL: tail wordcount wordcount-dynamic

tail: tail.o
	$(CC) $(CFLAGS) $^ -o $@
wordcount: wordcount.o io.o htable.h libhtable.a
	$(CC) $(CFLAGS) $^ -o $@ -static -L. -lhtable
wordcount-dynamic: wordcount.o io.o htable.h libhtable.so
	$(CC) $(CFLAGS) $^ -o $@ -L. -lhtable

libhtable.a: htab_init.o htab_lookup.o htab_remove.o htab_clear.o htab_foreach.o htab_stats.o hash_function.o
	ar crs $@ $^
libhtable.so: htab_init.o htab_lookup.o htab_remove.o htab_clear.o htab_foreach.o htab_stats.o hash_function.o
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^

# make object files for libhtable
htab_%.o: htab_%.c htable.h
	$(CC) $(CFLAGS) -fPIC -c $< -o $@
hash_function.o: hash_function.c hash_function.h
	$(CC) $(CFLAGS) -fPIC -c $< -o $@
# make object files
tail.o: tail.c tail.h
	$(CC) $(CFLAGS) -c $< -o $@
io.o: io.c io.h
	$(CC) $(CFLAGS) -c $< -o $@
wordcount.o: wordcount.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: pack debug clean stats

# compile with -g debugging flag
debug: CFLAGS+=-g
debug: clean ALL

# print table statistics
stats: CFLAGS+=-D P_STATS
stats: clean ALL

# pack project to upload
pack:
	zip -r $(LOGIN).zip $(ALLFILES)

clean:
	rm -f $(OBJS) tail wordcount wordcount-dynamic libhtable.a libhtable.so
