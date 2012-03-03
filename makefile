test: test.c makefile libbeanstalk.so
	gcc -o test test.c -L. -lbeanstalk

libbeanstalk.so: beanstalk.o
	gcc -shared -o libbeanstalk.so beanstalk.o

beanstalk.o: beanstalk.c makefile
	gcc -fPIC -c -o beanstalk.o beanstalk.c

install:
	cp 
