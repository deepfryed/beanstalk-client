OS := $(shell uname)

SOURCES1    := $(wildcard test/*.cc)
SOURCES2    := $(wildcard examples/c/*.c)
SOURCES3    := $(wildcard examples/cpp/*.cc)
TESTS       := $(SOURCES1:%.cc=%)
CEXAMPLES   := $(SOURCES2:%.c=%)
CPPEXAMPLES := $(SOURCES3:%.cc=%)

ifeq ($(OS), FreeBSD)
DESTDIR      =
PREFIX       = /usr/local
INCLUDEDIR   = $(PREFIX)/include/
LIBDIR       = $(PREFIX)/lib/
PKGCONFIGDIR = $(PREFIX)/libdata/pkgconfig/
else
DESTDIR      =
PREFIX       = /usr
INCLUDEDIR   = $(PREFIX)/include/
LIBDIR       = $(PREFIX)/lib/
PKGCONFIGDIR = $(LIBDIR)/pkgconfig/
endif

VERSION      = $(shell cat beanstalk.h | grep BS_.*_VERSION | sed 's/^.*VERSION *//' | xargs echo | sed 's/ /./g')

ifeq ($(OS), Darwin)
SHAREDLIB    = libbeanstalk.dylib
LINKER       = -shared -Wl,-dylib_install_name,$(SHAREDLIB).1
LNOPTS       = -sf
else ifeq ($(OS), FreeBSD)
SHAREDLIB    = libbeanstalk.dylib
LINKER       = -shared -Wl,-soname,$(SHAREDLIB).1
LNOPTS       = -sf
else
SHAREDLIB    = libbeanstalk.so
LINKER       = -shared -Wl,-soname,$(SHAREDLIB).1
LNOPTS       = -sfT
endif

STATICLIB    = libbeanstalk.a
CFLAGS       = -Wall -Wno-sign-compare -g -I.
CXXFLAGS     = -std=c++11 $(CFLAGS)
LDFLAGS      = -L.
CC           ?= gcc
CXX          ?= g++

all: $(CEXAMPLES) $(CPPEXAMPLES) benchmark

test: $(TESTS)
	test/run-all

$(TESTS): test/%:test/%.o $(SHAREDLIB)
	$(CXX) -o $@ $< $(LDFLAGS) -L. -lbeanstalk -lpthread

test/%.o: test/%.cc test/test.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

benchmark: benchmark.cc $(SHAREDLIB)
	$(CXX) $(CXXFLAGS) -o benchmark benchmark.cc $(LDFLAGS) -lbeanstalk -lpthread

$(CEXAMPLES): examples/c/%:examples/c/%.o $(SHAREDLIB)
	$(CC) -o $@ $< $(LDFLAGS) -L. -lbeanstalk

examples/c/%.o: examples/c/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(CPPEXAMPLES): examples/cpp/%:examples/cpp/%.o $(SHAREDLIB)
	$(CXX) -o $@ $< $(LDFLAGS) -L. -lbeanstalk

examples/cpp/%.o: examples/cpp/%.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(STATICLIB): beanstalk.o beanstalkcpp.o
	rm -f $@
	ar -cq $@ $^

$(SHAREDLIB): beanstalk.o beanstalkcpp.o
	$(CXX) $(CFLAGS) $(LINKER) $(LDFLAGS) -o $(SHAREDLIB)  beanstalk.o beanstalkcpp.o
	rm -f $(SHAREDLIB).1
	ln -s $(SHAREDLIB) $(SHAREDLIB).1

beanstalk.o: beanstalk.c beanstalk.h makefile
	$(CC) $(CFLAGS) -fPIC -c -o beanstalk.o beanstalk.c

beanstalkcpp.o: beanstalk.cc beanstalk.hpp makefile
	$(CXX) $(CXXFLAGS) -fPIC -c -o beanstalkcpp.o beanstalk.cc

install: $(SHAREDLIB) $(STATICLIB)
	install -d $(DESTDIR)$(INCLUDEDIR)
	install -m 0644 beanstalk.h $(DESTDIR)$(INCLUDEDIR)
	install -m 0644 beanstalk.hpp $(DESTDIR)$(INCLUDEDIR)

	install -d $(DESTDIR)$(LIBDIR)
	install -m 0755 $(SHAREDLIB) $(DESTDIR)$(LIBDIR)/$(SHAREDLIB).$(VERSION)
	ln $(LNOPTS) $(SHAREDLIB).$(VERSION) $(DESTDIR)$(LIBDIR)/$(SHAREDLIB).1
	ln $(LNOPTS) $(SHAREDLIB).$(VERSION) $(DESTDIR)$(LIBDIR)/$(SHAREDLIB)

	install -m 0644 $(STATICLIB) $(DESTDIR)$(LIBDIR)/$(STATICLIB).$(VERSION)
	ln $(LNOPTS) $(STATICLIB).$(VERSION) $(DESTDIR)$(LIBDIR)/$(STATICLIB).1
	ln $(LNOPTS) $(STATICLIB).$(VERSION) $(DESTDIR)$(LIBDIR)/$(STATICLIB)

	install -d $(DESTDIR)$(PKGCONFIGDIR)
	install -m 0644 beanstalk-client.pc $(DESTDIR)$(PKGCONFIGDIR)/libbeanstalk.pc
	sed -i -e 's/@VERSION@/$(VERSION)/' $(DESTDIR)$(PKGCONFIGDIR)/libbeanstalk.pc
	sed -i -e 's,@prefix@,$(PREFIX),' $(DESTDIR)$(PKGCONFIGDIR)/libbeanstalk.pc
	sed -i -e 's,@libdir@,$(LIBDIR),' $(DESTDIR)$(PKGCONFIGDIR)/libbeanstalk.pc
	sed -i -e 's,@includedir@,$(INCLUDEDIR),' $(DESTDIR)$(PKGCONFIGDIR)/libbeanstalk.pc

uninstall:
	rm -f $(DESTDIR)$(INCLUDEDIR)/beanstalk.h
	rm -f $(DESTDIR)$(INCLUDEDIR)/beanstalk.hpp
	rm -f $(DESTDIR)$(LIBDIR)$(SHAREDLIB)*
	rm -f $(DESTDIR)$(LIBDIR)$(STATICLIB)*
	rm -f $(DESTDIR)$(PKGCONFIGDIR)/libbeanstalk.pc

clean:
	rm -f *.o *.so *.so.* $(STATICLIB) test/test[0-9] test/*.o examples/**/*.o examples/**/example?

.PHONY: test clean install uninstall
