OBJS=gi.o
LIBS=-lm

all:  gi gcalc

gi:  gi.o
	$(CC) -o gi gi.o $(LIBS)

gcalc:  gcalc.o uncgi.o
	$(CC) -o gcalc gcalc.o uncgi.o $(LIBS)

uncgi.o: uncgi.c
	$(CC) -c -DLIBRARY -DNO_QUERY_OK uncgi.c

clean spotless:
	rm -f *.o gi gcalc
