SOURCES1    := $(wildcard test/*.cc)
SOURCES2    := $(wildcard examples/c/*.c)
SOURCES3    := $(wildcard examples/cpp/*.cc)
TESTS       := $(SOURCES1:%.cc=%)
CEXAMPLES   := $(SOURCES2:%.c=%)
CPPEXAMPLES := $(SOURCES3:%.cc=%)

VERSION      = 1.0.0
CSHAREDLIB   = libbeanstalk.so
CPPSHAREDLIB = libbeanstalkcpp.so
CFLAGS       = -Wall -g -I.
CLDFLAGS     = -L. -lbeanstalk
CPPLDFLAGS   = -L. -lbeanstalkcpp
CC           = gcc
CPP          = g++

all: $(CEXAMPLES) $(CPPEXAMPLES)

test: $(TESTS)
	test/run-all

$(TESTS): test/%:test/%.o libbeanstalk.so
	$(CPP) -o $@ $< $(CLDFLAGS) -lgtest -lpthread

test/%.o: test/%.cc
	$(CPP) $(CFLAGS) -c -o $@ $<

$(CEXAMPLES): examples/c/%:examples/c/%.o libbeanstalk.so
	$(CC) -o $@ $< $(CLDFLAGS)

examples/c/%.o: examples/c/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(CPPEXAMPLES): examples/cpp/%:examples/cpp/%.o libbeanstalkcpp.so
	$(CPP) -o $@ $< $(CPPLDFLAGS)

examples/cpp/%.o: examples/cpp/%.cc
	$(CPP) $(CFLAGS) -c -o $@ $<

$(CSHAREDLIB): beanstalk.o
	$(CC) -shared -o $(CSHAREDLIB) beanstalk.o

beanstalk.o: beanstalk.c beanstalk.h makefile
	$(CC) $(CFLAGS) -fPIC -c -o beanstalk.o beanstalk.c

$(CPPSHAREDLIB): beanstalkcpp.o beanstalk.o
	$(CPP) -shared -o $(CPPSHAREDLIB) beanstalkcpp.o beanstalk.o

beanstalkcpp.o: beanstalk.cc beanstalk.hpp makefile
	$(CPP) $(CFLAGS) -fPIC -c -o beanstalkcpp.o beanstalk.cc

install: $(CSHAREDLIB) $(CPPSHAREDLIB)
	cp beanstalk.h /usr/include
	cp beanstalk.hpp /usr/include
	cp $(CSHAREDLIB) /usr/lib/$(CSHAREDLIB).$(VERSION)
	ln -sfT /usr/lib/$(CSHAREDLIB).$(VERSION) /usr/lib/$(CSHAREDLIB).1
	ln -sfT /usr/lib/$(CSHAREDLIB).$(VERSION) /usr/lib/$(CSHAREDLIB)
	cp $(CPPSHAREDLIB) /usr/lib/$(CPPSHAREDLIB).$(VERSION)
	ln -sfT /usr/lib/$(CPPSHAREDLIB).$(VERSION) /usr/lib/$(CPPSHAREDLIB).1
	ln -sfT /usr/lib/$(CPPSHAREDLIB).$(VERSION) /usr/lib/$(CPPSHAREDLIB)

uninstall:
	rm -f /usr/include/beanstalk.h
	rm -f /usr/include/beanstalk.hpp
	rm -f /usr/lib/$(CSHAREDLIB)*
	rm -f /usr/lib/$(CPPSHAREDLIB)*

clean:
	rm -f *.o *.so *.so.* test/test[0-9] test/*.o examples/**/*.o examples/**/example?
