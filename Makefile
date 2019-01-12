OBJS=gi.o

gi:  $(OBJS)
	cc -o gi $(OBJS) -lm

clean spotless:
	rm -f $(OBJS) gi
