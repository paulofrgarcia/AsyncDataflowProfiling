CC=gcc
CXX=g++
CPPFLAGS=-std=c++0x
RM=rm -f
#LDFLAGS=-g $(shell root-config --ldflags)
#LDLIBS=$(shell root-config --libs)

SRCS=main.cpp Actor.cpp Action.cpp InPort.cpp Network.cpp helper.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: test

test: $(OBJS)
	$(CXX) -o test $(OBJS) $(CPPFLAGS) 

main.o: main.cpp types.h

Actor.o: Actor.cpp types.h

Action.o: Action.cpp types.h

InPort.o: InPort.cpp types.h

Network.o: Network.cpp types.h

helper.o: helper.cpp 

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) test
