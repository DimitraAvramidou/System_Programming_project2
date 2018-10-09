all: myhttpd mycrawler

myhttpd: myhttpd.o
	gcc myhttpd.o queue.o -o myhttpd -lpthread

mycrawler: mycrawler.o
		gcc mycrawler.o queue.o -o mycrawler -lpthread

myhttpd.o : myhttpd.c
		gcc -c -g myhttpd.c queue.c -lpthread

mycrawler.o : mycrawler.c
		gcc -c -g mycrawler.c queue.c -lpthread

queue.o : queue.c
		gcc -c -g queue.c

clean:
		rm -f *.o myhttpd
