E_SOURCES := $(wildcard examples/*.c)
E_OBJECTS := $(E_SOURCES:%.c=%.o)
E_ELVES   := $(E_SOURCES:%.c=%)

T_SOURCES := $(wildcard test/*.cc)
T_OBJECTS := $(T_SOURCES:%.cc=%.o)
T_ELVES   := $(T_SOURCES:%.cc=%)

SHAREDLIB  = /usr/lib/libbeanstalk.so.1.0.0
CFLAGS     = -Wall -g -I.
LDFLAGS    = -L. -lbeanstalk

CC         = gcc
CPP        = g++

all: $(T_ELVES) $(E_ELVES)

test: $(T_ELVES)
	test/run-all

$(T_ELVES): $(T_OBJECTS) libbeanstalk.so
	$(CPP) -o $@ $< $(LDFLAGS) -lgtest -lpthread

$(T_OBJECTS): $(T_SOURCES)
	$(CPP) $(CFLAGS) -c -o $@ $<

$(E_ELVES): $(E_OBJECTS) libbeanstalk.so
	$(CC) -o $@ $< $(LDFLAGS)

$(E_OBJECTS): $(E_SOURCES)
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
	rm -f *.o *.so *.so.* test/test[0-9]
