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


char *usage = "wheel tire_size chainring{,chainring} cog{,cog}";

int diameter;
int tire;

#define MAX_CHAINRINGS 10
#define MAX_COGS 20

int chainrings[MAX_CHAINRINGS];
int cogs[MAX_CHAINRINGS];

int nr_chainrings;
int nr_cogs;


int web = 0;

struct { int diameter; char *alias; } diameters[] = {
    { 630, "27" },
    { 622, "700c" },
    { 590, "650a" },
    { 487, "700d" },
    { 584, "650b" },
    { 584, "27.5" },
    { 571, "650c" },
    { 559, "26" },
} ;
#define NR_DIAMETERS (sizeof diameters / sizeof diameters[0])


int
gi(int chainring, int cog)
{
    float mm_gain = (diameter + tire);

    mm_gain *= chainring;
    mm_gain /= cog;

    return mm_gain / 25.4;
}


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
    else {
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
    char *pgm = basename(argc ? argv[0] : "gi");

    web = (strcasecmp(pgm, "webgi") == 0);
    
    if ( argc != 5 )
	error("usage: %s %s", pgm, usage);

    diameter = 0;
    for (i = 0; i < NR_DIAMETERS; i++ )
	if ( strcasecmp(argv[1], diameters[i].alias) == 0 ) {
	    diameter = diameters[i].diameter;
	    break;
	}

    if ( diameter == 0 ) {
	diameter = atoi(argv[1]);
	if ( diameter <= 0 )
	    error("%s: wheel diameter <%s>?", pgm, argv[1]);
    }

    tire = atoi(argv[2]);

    if ( tire <= 0 )
	error("%s: tire size <%s>?", pgm, argv[2]);

    nr_chainrings = 0;
    for ( arg = strtok(argv[3], ","); arg; arg = strtok(0, ",") ) {
	if ( nr_chainrings >= MAX_CHAINRINGS )
	    error("%s: too many chainrings (%d max)", pgm, MAX_CHAINRINGS);
	    
	if ( (i = atoi(arg)) <= 0 ) {
	    error("%s: chainring <%s>?", pgm, arg);
	    exit(1);
	}
	chainrings[nr_chainrings++] = i;
    }

    nr_cogs = 0;

    for ( arg = strtok(argv[4], ","); arg; arg = strtok(0, ",") ) {
	if ( nr_cogs >= MAX_COGS ) {
	    error("%s: too many cogs (%d max)", pgm, MAX_COGS);
	    exit(1);
	}
	i = atoi(arg);
	if ( i <= 0 ) {
	    error("%s: cog <%s>?", pgm, arg);
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

	for ( j = 0; j < nr_chainrings; j++ )
	    printf(web ? "<td>%d</td>" : "%8d", gi(chainrings[j], cogs[i]));
	printf(web ? "</tr>\n" : "\n");
    }
    if ( web )
	puts("</table>");
}
