test: test.c makefile libbeanstalk.so
	gcc -o test test.c -L. -lbeanstalk

libbeanstalk.so: beanstalk.o
	gcc -shared -o libbeanstalk.so beanstalk.o

beanstalk.o: beanstalk.c makefile
	gcc -fPIC -c -o beanstalk.o beanstalk.c

SHAREDLIB=/usr/lib/libbeanstalk.so.1.0.0

install: libbeanstalk.so
	cp beanstalk.h /usr/include
	cp libbeanstalk.so $(SHAREDLIB)
	ln -s $(SHAREDLIB) /usr/lib/libbeanstalk.so.1
	ln -s $(SHAREDLIB) /usr/lib/libbeanstalk.so

uninstall:
	rm /usr/include/beanstalk.h
	rm $(SHAREDLIB)

clean:
	rm -f *.o *.so *.so.* test
