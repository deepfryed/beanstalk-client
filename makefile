SOURCES1  := $(wildcard examples/*.c)
SOURCES2  := $(wildcard test/*.cc)
EXAMPLES  := $(SOURCES1:%.c=%)
TESTS     := $(SOURCES2:%.cc=%)

SHAREDLIB  = /usr/lib/libbeanstalk.so.1.0.0
CFLAGS     = -Wall -g -I.
LDFLAGS    = -L. -lbeanstalk
CC         = gcc
CPP        = g++

all: $(EXAMPLES)

test: $(TESTS)
	test/run-all

$(TESTS): test/%:test/%.o libbeanstalk.so
	$(CPP) -o $@ $< $(LDFLAGS) -lgtest -lpthread

test/%.o: test/%.cc
	$(CPP) $(CFLAGS) -c -o $@ $<

$(EXAMPLES): examples/%:examples/%.o libbeanstalk.so
	$(CC) -o $@ $< $(LDFLAGS)

examples/%.o: examples/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

libbeanstalk.so: beanstalk.o
	gcc -shared -o libbeanstalk.so beanstalk.o

beanstalk.o: beanstalk.c makefile
	gcc -fPIC -c -o beanstalk.o beanstalk.c

install: libbeanstalk.so
	cp beanstalk.h /usr/include
	cp libbeanstalk.so $(SHAREDLIB)
	ln -s $(SHAREDLIB) /usr/lib/libbeanstalk.so.1
	ln -s $(SHAREDLIB) /usr/lib/libbeanstalk.so

uninstall:
	rm /usr/include/beanstalk.h
	rm $(SHAREDLIB)

clean:
	rm -f *.o *.so *.so.* test/test[0-9] test/*.o examples/*.o
