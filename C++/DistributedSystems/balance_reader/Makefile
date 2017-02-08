CXXFLAGS =	-std=c++14 -O2 -g -Wall -fmessage-length=0

OBJS =		balance_reader.o

LIBS =		-lpthread

TARGET =	balance_reader

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
