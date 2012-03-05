SOURCES1    := $(wildcard test/*.cc)
SOURCES2    := $(wildcard examples/c/*.c)
SOURCES3    := $(wildcard examples/cpp/*.cc)
TESTS       := $(SOURCES1:%.cc=%)
CEXAMPLES   := $(SOURCES2:%.c=%)
CPPEXAMPLES := $(SOURCES3:%.cc=%)

VERSION      = 1.0.0
SHAREDLIB    = libbeanstalk.so
CFLAGS       = -Wall -g -I.
LDFLAGS      = -L. -lbeanstalk
CC           = gcc
CPP          = g++

all: $(CEXAMPLES) $(CPPEXAMPLES)

test: $(TESTS)
	test/run-all

$(TESTS): test/%:test/%.o $(SHAREDLIB)
	$(CPP) -o $@ $< $(LDFLAGS) -lgtest -lpthread

test/%.o: test/%.cc
	$(CPP) $(CFLAGS) -c -o $@ $<

$(CEXAMPLES): examples/c/%:examples/c/%.o libbeanstalk.so
	$(CC) -o $@ $< $(LDFLAGS)

examples/c/%.o: examples/c/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(CPPEXAMPLES): examples/cpp/%:examples/cpp/%.o libbeanstalk.so
	$(CPP) -o $@ $< $(LDFLAGS)

examples/cpp/%.o: examples/cpp/%.cc
	$(CPP) $(CFLAGS) -c -o $@ $<

$(SHAREDLIB): beanstalk.o beanstalkcpp.o
	$(CPP) -shared -o $(SHAREDLIB) beanstalk.o beanstalkcpp.o

beanstalk.o: beanstalk.c beanstalk.h makefile
	$(CC) $(CFLAGS) -fPIC -c -o beanstalk.o beanstalk.c

beanstalkcpp.o: beanstalk.cc beanstalk.hpp makefile
	$(CPP) $(CFLAGS) -fPIC -c -o beanstalkcpp.o beanstalk.cc

install: $(SHAREDLIB)
	cp beanstalk.h /usr/include
	cp beanstalk.hpp /usr/include
	cp $(SHAREDLIB) /usr/lib/$(SHAREDLIB).$(VERSION)
	ln -sfT /usr/lib/$(SHAREDLIB).$(VERSION) /usr/lib/$(SHAREDLIB).1
	ln -sfT /usr/lib/$(SHAREDLIB).$(VERSION) /usr/lib/$(SHAREDLIB)

uninstall:
	rm -f /usr/include/beanstalk.h
	rm -f /usr/include/beanstalk.hpp
	rm -f /usr/lib/$(SHAREDLIB)*

clean:
	rm -f *.o *.so *.so.* test/test[0-9] test/*.o examples/**/*.o examples/**/example?
