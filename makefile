OS := $(shell uname)

SOURCES1    := $(wildcard test/*.cc)
SOURCES2    := $(wildcard examples/c/*.c)
SOURCES3    := $(wildcard examples/cpp/*.cc)
TESTS       := $(SOURCES1:%.cc=%)
CEXAMPLES   := $(SOURCES2:%.c=%)
CPPEXAMPLES := $(SOURCES3:%.cc=%)
DESTDIR      = /

# $(shell cat beanstalk.h | grep BS_.*_VERSION | sed 's/^.*VERSION *//' | xargs echo | sed 's/ /./g')
VERSION      = 1.2.0

ifeq ($(OS), Darwin)
SHAREDLIB    = libbeanstalk.dylib
LINKER       = -shared -Wl,-dylib_install_name,$(SHAREDLIB).1
LNOPTS       = -sf
else
SHAREDLIB    = libbeanstalk.so
LINKER       = -shared -Wl,-soname,$(SHAREDLIB).1
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
	$(CPP) $(LINKER) -o $(SHAREDLIB)  beanstalk.o beanstalkcpp.o

beanstalk.o: beanstalk.c beanstalk.h makefile
	$(CC) $(CFLAGS) -fPIC -c -o beanstalk.o beanstalk.c

beanstalkcpp.o: beanstalk.cc beanstalk.hpp makefile
	$(CPP) $(CFLAGS) -fPIC -c -o beanstalkcpp.o beanstalk.cc

install: $(SHAREDLIB) $(STATICLIB)
	mkdir -p $(DESTDIR)/usr/include $(DESTDIR)/usr/lib $(DESTDIR)/usr/lib/pkgconfig
	cp beanstalk.h $(DESTDIR)/usr/include
	cp beanstalk.hpp $(DESTDIR)/usr/include
	cp $(SHAREDLIB) $(DESTDIR)/usr/lib/$(SHAREDLIB).$(VERSION)
	cd $(DESTDIR)/usr/lib && ln $(LNOPTS) $(SHAREDLIB).$(VERSION) $(SHAREDLIB).1
	cd $(DESTDIR)/usr/lib && ln $(LNOPTS) $(SHAREDLIB).$(VERSION) $(SHAREDLIB)
	cp $(STATICLIB) $(DESTDIR)/usr/lib/$(STATICLIB).$(VERSION)
	cd $(DESTDIR)/usr/lib && ln $(LNOPTS) $(STATICLIB).$(VERSION) $(STATICLIB).1
	cd $(DESTDIR)/usr/lib && ln $(LNOPTS) $(STATICLIB).$(VERSION) $(STATICLIB)
	cp beanstalk-client.pc $(DESTDIR)/usr/lib/pkgconfig/libbeanstalk.pc
	sed -i -e 's/@VERSION@/$(VERSION)/' $(DESTDIR)/usr/lib/pkgconfig/libbeanstalk.pc

uninstall:
	rm -f $(DESTDIR)usr/include/beanstalk.h
	rm -f $(DESTDIR)usr/include/beanstalk.hpp
	rm -f $(DESTDIR)usr/lib/$(SHAREDLIB)*
	rm -f $(DESTDIR)usr/lib/$(STATICLIB)*

clean:
	rm -f *.o *.so *.so.* test/test[0-9] test/*.o examples/**/*.o examples/**/example?
