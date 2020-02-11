# Makefile
# Author: 	Jakub Lukac
# e-mail:	xlukac09@stud.fit.vutbr.cz

LOGIN=xlukac09
PROJECT_NAME=rbm

CPP=g++
CPPFLAGS=-std=c++11 -O2 -DNDEBUG -DNDEBUG_TRACE
# CPPFLAGS=-std=c++11 -pedantic -Wall -DNDEBUG -DNDEBUG_TRACE
DBGINFO=-UNDEBUG

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

CPPFILES=$(wildcard src/*.cpp)
HEADERS=$(wildcard src/*.h)
OBJS=$(subst .cpp,.o,$(CPPFILES))

ALLFILES=$(CPPFILES) $(HEADERS) Makefile CMakeLists.txt run.sh  README doc/doc.pdf

all: $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJS)
	$(CPP) $(CPPFLAGS) $^ $(LIBS) -o $@

%.o: %.c $(HEADERS)
	$(CPP) $(CPPFLAGS) -c $< -o $@

.PHONY: debug pack clean

# compile with -g debugging flag
debug: CPPFLAGS+=-g $(DBGINFO)
debug: clean all

pack:
	zip $(LOGIN).zip $(ALLFILES)

clean:
	rm -f $(PROJECT_NAME) $(OBJS) *h.gch index.html
