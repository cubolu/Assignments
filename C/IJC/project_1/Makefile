################################################################################
# Makefile                                                                     #
# Autor:       Jakub Lukac                                                     #
# Login:       xlukac09                                                        #
# Rocnik:      1. rocnik BIT                                                   #
# Datum:       26.03.2015                                                      #
# e-mail:      xlukac09@stud.fit.vutbr.cz                                      #
# subory: prvocisla.c, bit-array.h, eratosthenes.c, eratosthenes.h, error.c,   #
#         error.h, ppm.c, ppm.h, steg-decode.c                                 #
################################################################################

CC=gcc
CFLAGS=-O2 -std=c99 -Wall -Wextra -pedantic -g
LDLIBS=-lm

# obsah projektu
ALLFILES=*.c \
			Makefile\
			*.h

# preklad
ALL: prvocisla prvocisla-inline steg-decode

# make error.o
error.o: error.c error.h
	$(CC) $(CFLAGS) -c error.c -o error.o
# make eratosthenes.o
eratosthenes.o: eratosthenes.c eratosthenes.h bit-array.h error.h
	$(CC) $(CFLAGS) -c eratosthenes.c -o eratosthenes.o
# make ppm.o
ppm.o: ppm.c ppm.h error.h
	$(CC) $(CFLAGS) -c ppm.c -o ppm.o

# make prvocisla.o
prvocisla.o: prvocisla.c eratosthenes.h bit-array.h
	$(CC) $(CFLAGS) -c prvocisla.c -o prvocisla.o
# make prvocisla
prvocisla: prvocisla.o eratosthenes.o error.o
	$(CC) $(CFLAGS) prvocisla.o eratosthenes.o error.o $(LDLIBS) -o prvocisla
# make prvocisla-inline.o
prvocisla-inline.o: prvocisla.c eratosthenes.h bit-array.h
	$(CC) $(CFLAGS) -c -D USE_INLINE prvocisla.c -o prvocisla-inline.o
# make prvocisla-inline
prvocisla-inline: prvocisla-inline.o eratosthenes.o error.o
	$(CC) $(CFLAGS) -D USE_INLINE prvocisla-inline.o eratosthenes.o error.o $(LDLIBS) -o prvocisla-inline

# make steg-decode.o
steg-decode.o: steg-decode.c ppm.h eratosthenes.h error.h bit-array.h
	$(CC) $(CFLAGS) -c steg-decode.c -o steg-decode.o
# make steg-decode
steg-decode: steg-decode.o ppm.o eratosthenes.o error.o
	$(CC) $(CFLAGS) steg-decode.o ppm.o eratosthenes.o error.o $(LDLIBS) -o steg-decode

.PHONY: zip clean

# pack project
zip:
	zip xlukac09.zip $(ALLFILES)

clean:
	rm -f prvocisla
	rm -f prvocisla-inline
	rm -f steg-decode
	rm -f *.o
