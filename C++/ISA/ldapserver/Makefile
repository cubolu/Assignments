# Makefile
# Author: 	Jakub Lukac
# e-mail:	xlukac09@stud.fit.vutbr.cz

LOGIN=xlukac09
PROJECT_NAME=myldap

CPP=g++
CPPFLAGS=-std=c++11 -DNDEBUG -DNDEBUG_TRACE
# CPPFLAGS=-std=c++11 -pedantic -Wall -DNDEBUG -DNDEBUG_TRACE
DBGINFO=-UNDEBUG

CPPFILES=$(wildcard *.cpp)
HEADERS=$(wildcard *.h)
OBJS=$(subst .cpp,.o,$(CPPFILES))

ALLFILES=$(CPPFILES) $(HEADERS) Makefile CMakeLists.txt data/isa2017-ldap.csv README manual.pdf

all: $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJS)
	$(CPP) $(CPPFLAGS) $^ -o $@

%.o: %.c $(HEADERS)
	$(CC) $(CPPFLAGS) -c $< -o $@

.PHONY: debug pack clean

# compile with -g debugging flag
debug: CPPFLAGS+=-g $(DBGINFO)
debug: clean all

pack:
	tar -cvf $(LOGIN).tar $(ALLFILES)

clean:
	rm -f $(PROJECT_NAME) $(OBJS) *h.gch index.html
