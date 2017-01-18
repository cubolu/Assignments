################################################################################
# Makefile                                                                     #
# Autor:       Jakub Lukac                                                     #
# Login:       xlukac09                                                        #
# Rocnik:      1. rocnik BIT                                                   #
# e-mail:      xlukac09@stud.fit.vutbr.cz                                      #
################################################################################

LOGIN=xlukac09

CC=gcc
CFLAGS=-Wall -Wextra -std=gnu99 -pedantic

CFILES=$(wildcard *.c)
HEADERS=$(wildcard *.h)
ALLFILES=$(CFILES) $(HEADERS) Makefile
OBJS=$(CFILES:.c=.o)

# make project
ALL: h2o

h2o: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -pthread

# make object
%.o: %.c h2o.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: pack debug clean

# pack project to upload
pack:
	zip -r $(LOGIN).zip $(ALLFILES)

# compile with -g debugging flag
debug: CFLAGS+=-g
debug: clean ALL

clean:
	rm -f $(OBJS) h2o h2o.out
