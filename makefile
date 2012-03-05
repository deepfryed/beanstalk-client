SOURCES1    := $(wildcard test/*.cc)
SOURCES2    := $(wildcard examples/c/*.c)
SOURCES3    := $(wildcard examples/cpp/*.cc)
TESTS       := $(SOURCES1:%.cc=%)
CEXAMPLES   := $(SOURCES2:%.c=%)
CPPEXAMPLES := $(SOURCES3:%.cc=%)

VERSION      = 1.0.0
CSHAREDLIB   = /usr/lib/libbeanstalk.so.$(VERSION)
CPPSHAREDLIB = /usr/lib/libbeanstalkcpp.so.$(VERSION)
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

libbeanstalk.so: beanstalk.o
	$(CC) -shared -o libbeanstalk.so beanstalk.o

beanstalk.o: beanstalk.c beanstalk.h makefile
	$(CC) $(CFLAGS) -fPIC -c -o beanstalk.o beanstalk.c

libbeanstalkcpp.so: beanstalkcpp.o beanstalk.o
	$(CPP) -shared -o libbeanstalkcpp.so beanstalkcpp.o beanstalk.o -L. -lbeanstalk

beanstalkcpp.o: beanstalk.cc beanstalk.hpp makefile
	$(CPP) $(CFLAGS) -fPIC -c -o beanstalkcpp.o beanstalk.cc

install: libbeanstalk.so
	cp beanstalk.h /usr/include
	cp libbeanstalk.so $(CSHAREDLIB)
	ln -s $(CSHAREDLIB) /usr/lib/libbeanstalk.so.1
	ln -s $(CSHAREDLIB) /usr/lib/libbeanstalk.so

uninstall:
	rm /usr/include/beanstalk.h
	rm $(CSHAREDLIB)

clean:
	rm -f *.o *.so *.so.* test/test[0-9] test/*.o examples/**/*.o examples/**/example?
