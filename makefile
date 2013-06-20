OS := $(shell uname)

SOURCES1    := $(wildcard test/*.cc)
SOURCES2    := $(wildcard examples/c/*.c)
SOURCES3    := $(wildcard examples/cpp/*.cc)
TESTS       := $(SOURCES1:%.cc=%)
CEXAMPLES   := $(SOURCES2:%.c=%)
CPPEXAMPLES := $(SOURCES3:%.cc=%)

# $(shell cat beanstalk.h | grep BS_.*_VERSION | sed 's/^.*VERSION *//' | xargs echo | sed 's/ /./g')
VERSION      = 1.0.0

ifeq ($(OS), Darwin)
SHAREDLIB    = libbeanstalk.dylib
LNOPTS       = -sf
else
SHAREDLIB    = libbeanstalk.so
LNOPTS       = -sfT
endif

STATICLIB    = libbeanstalk.a
CFLAGS       = -Wall -Wno-sign-compare -g -I.
LDFLAGS      = -L. -lbeanstalk
CC           = gcc
CPP          = g++

all: $(CEXAMPLES) $(CPPEXAMPLES) benchmark

test: $(TESTS)
	test/run-all

$(TESTS): test/%:test/%.o $(SHAREDLIB)
	$(CPP) -o $@ $< $(LDFLAGS) -lgtest -lpthread

test/%.o: test/%.cc
	$(CPP) $(CFLAGS) -c -o $@ $<

benchmark: benchmark.cc $(SHAREDLIB)
	$(CPP) $(CFLAGS) -o benchmark benchmark.cc $(LDFLAGS) -lpthread

$(CEXAMPLES): examples/c/%:examples/c/%.o $(SHAREDLIB)
	$(CC) -o $@ $< $(LDFLAGS)

examples/c/%.o: examples/c/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(CPPEXAMPLES): examples/cpp/%:examples/cpp/%.o $(SHAREDLIB)
	$(CPP) -o $@ $< $(LDFLAGS)

examples/cpp/%.o: examples/cpp/%.cc
	$(CPP) $(CFLAGS) -c -o $@ $<

$(STATICLIB): beanstalk.o beanstalkcpp.o
	rm -f $@
	ar -cq $@ $^

$(SHAREDLIB): beanstalk.o beanstalkcpp.o
	$(CPP) -shared -o $(SHAREDLIB) beanstalk.o beanstalkcpp.o

beanstalk.o: beanstalk.c beanstalk.h makefile
	$(CC) $(CFLAGS) -fPIC -c -o beanstalk.o beanstalk.c

beanstalkcpp.o: beanstalk.cc beanstalk.hpp makefile
	$(CPP) $(CFLAGS) -fPIC -c -o beanstalkcpp.o beanstalk.cc

install: $(SHAREDLIB) $(STATICLIB)
	cp beanstalk.h /usr/include
	cp beanstalk.hpp /usr/include
	cp $(SHAREDLIB) /usr/lib/$(SHAREDLIB).$(VERSION)
	ln $(LNOPTS) /usr/lib/$(SHAREDLIB).$(VERSION) /usr/lib/$(SHAREDLIB).1
	ln $(LNOPTS) /usr/lib/$(SHAREDLIB).$(VERSION) /usr/lib/$(SHAREDLIB)
	cp $(STATICLIB) /usr/lib/$(STATICLIB).$(VERSION)
	ln $(LNOPTS) /usr/lib/$(STATICLIB).$(VERSION) /usr/lib/$(STATICLIB).1
	ln $(LNOPTS) /usr/lib/$(STATICLIB).$(VERSION) /usr/lib/$(STATICLIB)

uninstall:
	rm -f /usr/include/beanstalk.h
	rm -f /usr/include/beanstalk.hpp
	rm -f /usr/lib/$(SHAREDLIB)*
	rm -f /usr/lib/$(STATICLIB)*

clean:
	rm -f *.o *.so *.so.* test/test[0-9] test/*.o examples/**/*.o examples/**/example?
