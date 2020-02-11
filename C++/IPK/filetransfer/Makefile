# Makefile
# Author: 	Jakub Lukac
# e-mail:	xlukac09@stud.fit.vutbr.cz

LOGIN=xlukac09
PROJECT_NAME=client server

CPP=c++
CPPFLAGS=-std=c++11 -DNDEBUG
DBGINFO=-UNDEBUG

CPPFILES=client.cc server.cc
OBJS=client.o server.o

ALLFILES=$(CPPFILES) $(HEADERS) Makefile README.txt test.sh test/ doc.pdf

all: $(PROJECT_NAME)

client: client.o
	$(CPP) $(CPPFLAGS) client.o -o $@
client.o: client.cc
	$(CPP) $(CPPFLAGS) -c client.cc -o $@
server: server.o
	$(CPP) $(CPPFLAGS) server.o -o $@
server.o: server.cc
	$(CPP) $(CPPFLAGS) -c server.cc -o $@

.PHONY: test debug pack clean

test: test.sh
	chmod +x test.sh
	./test.sh

debug: CPPFLAGS+=-g $(DBGINFO)
debug: clean all

pack:
	tar -cvzf $(LOGIN).tar.gz $(ALLFILES)

clean:
	rm -f $(PROJECT_NAME) $(OBJS) *h.gch
