CC=gcc
CXX=g++
CPPFLAGS=-std=c++0x
RM=rm -f
#LDFLAGS=-g $(shell root-config --ldflags)
#LDLIBS=$(shell root-config --libs)

SRCS=main.cpp Actor.cpp Action.cpp InPort.cpp Network.cpp
OBJS=$(subst .cc,.o,$(SRCS))

all: test

test: $(OBJS)
	$(CXX) -o test $(OBJS) $(CPPFLAGS) 

main.o: main.cpp types.h

Actor.o: Actor.cpp types.h

Action.o: Action.cpp types.h

InPort.o: InPort.cpp types.h

Network.o: Network.cpp types.h

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) test
