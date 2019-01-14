/*
 * gear inch calculator
 *
 * usage: gi wheel tire chainring{,chainring} cog{,cog}
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <libgen.h>
#include <stdarg.h>

#include "functions.h"

char *usage = "wheel tire_size chainring{,chainring} cog{,cog}";

#define MAX_CHAINRINGS 10
#define MAX_COGS 20

int chainrings[MAX_CHAINRINGS];
int cogs[MAX_CHAINRINGS];

int nr_chainrings;
int nr_cogs;


int web = 0;
char *pgm = "gi";

void
error(char *fmt, ...)
{
    va_list ptr;

    va_start(ptr,fmt);

    if ( web ) {
	printf("<h1>oops!</h1>\n");
	printf("<p>");
	vprintf(fmt,ptr);
	printf("</p>\n");
    }
    else if ( fmt == usage ) {
	fprintf(stderr, "usage: %s %s\n", pgm, usage);
    }
    else {
	fprintf(stderr, "%s: ", pgm);
	vfprintf(stderr, fmt, ptr);
	putc('\n', stderr);
    }
    
    va_end(ptr);

    exit(1);
}


int /* Siiiiiigh, fine */
main(argc, argv)
char **argv;
{
    int i, j;
    char *arg;

    pgm = basename(argc ? argv[0] : "gi");

    web = (strcasecmp(pgm, "webgi") == 0);
    
    if ( argc != 5 )
	error(usage);

    if ( (diameter = wheel_diameter(argv[1])) == EOF )
	error("wheel diameter [%s]?", argv[1]);
    else if ( diameter == 0 )
	error("zero diameter wheel?");

    
    tire = atoi(argv[2]);

    if ( tire <= 0 )
	error("tire size [%s]?", argv[2]);

    nr_chainrings = 0;
    for ( arg = strtok(argv[3], ","); arg; arg = strtok(0, ",") ) {
	if ( nr_chainrings >= MAX_CHAINRINGS )
	    error("too many chainrings (%d max)", MAX_CHAINRINGS);
	    
	if ( (i = atoi(arg)) <= 0 ) {
	    error("chainring [%s]?", arg);
	    exit(1);
	}
	chainrings[nr_chainrings++] = i;
    }

    nr_cogs = 0;

    for ( arg = strtok(argv[4], ","); arg; arg = strtok(0, ",") ) {
	if ( nr_cogs >= MAX_COGS ) {
	    error("too many cogs (%d max)", MAX_COGS);
	    exit(1);
	}
	i = atoi(arg);
	if ( i <= 0 ) {
	    error("cog [%s]?", arg);
	    exit(1);
	}
	cogs[nr_cogs++] = i;
    }


    if ( web ) {
	puts("<table>");
	puts("<caption>Gear&nbsp;Inches</caption>");
	printf("<tr><td></td>");
	for ( j = 0; j < nr_chainrings; j++ ) {
	    printf("<th>%d</th>", chainrings[j]);
	}
	puts("</tr>");
    }
    else {
	printf("%7s|", "");
	for ( j = 0; j < nr_chainrings; j++ )
	    printf("%8d", chainrings[j]);
	putchar('\n');

	printf("%.7s+", "---------");
	for ( j = 0; j < nr_chainrings; j++ )
	    printf("%.8s", "--------");
	putchar('\n');
    }

    for ( i=0; i < nr_cogs; i++ ) {
	printf(web ? "<tr><th>%d</th>" : "%7d|", cogs[i]);

	for ( j = 0; j < nr_chainrings; j++ ) {
	    int gi = gear_inches(chainrings[j], cogs[i]);

	    if ( gi == EOF )
		printf(web ? "<td>%s</td>" : "%8s", "-?-");
	    else
		printf(web ? "<td>%d</td>" : "%8d", gi);
	}
	printf(web ? "</tr>\n" : "\n");
    }
    if ( web )
	puts("</table>");
}
