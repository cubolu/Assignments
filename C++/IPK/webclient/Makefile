# Makefile
# Author: 	Jakub Lukac
# e-mail:	xlukac09@stud.fit.vutbr.cz

LOGIN=xlukac09
PROJECT_NAME=webclient

CPP=g++
CPPFLAGS=-std=c++11 -pedantic -Wall -DNDEBUG
DBGINFO=-UNDEBUG

CPPFILES=$(wildcard *.cc)
#HEADERS=$(wildcard *.h)
OBJS=$(subst .cc,.o,$(CPPFILES))

ALLFILES=$(CPPFILES) $(HEADERS) Makefile

all: $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJS)
	$(CPP) $(CPPFLAGS) $^ -o $@

webclient.o: webclient.cc #webclient.h
	$(CPP) $(CPPFLAGS) -c $^ -o $@

.PHONY: debug pack clean

# compile with -g debugging flag
debug: CPPFLAGS+=-g $(DBGINFO)
debug: clean all

pack:
	tar -cvzf $(LOGIN).tar.gz $(ALLFILES)

clean:
	rm -f $(PROJECT_NAME) $(OBJS) *h.gch index.html
