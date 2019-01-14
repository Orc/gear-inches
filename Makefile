OBJS=gi.o
LIBS=-lm

all:  gi gcalc.cgi

gi:  gi.o functions.o
	$(CC) -o gi gi.o functions.o $(LIBS)

gcalc.cgi:  gcalc.o uncgi.o functions.o
	$(CC) -o gcalc.cgi gcalc.o uncgi.o functions.o $(LIBS)

uncgi.o: uncgi.c
	$(CC) -c -DLIBRARY -DNO_QUERY_OK uncgi.c

clean spotless:
	rm -f *.o gi gcalc.cgi
