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


char *usage = "wheel tire_size chainring{,chainring} cog{,cog}";

int diameter;
int tire;

#define MAX_CHAINRINGS 10
#define MAX_COGS 20

int chainrings[MAX_CHAINRINGS];
int cogs[MAX_CHAINRINGS];

int nr_chainrings;
int nr_cogs;

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

int /* Siiiiiigh, fine */
main(argc, argv)
char **argv;
{
    int i, j;
    char *arg;
    char *pgm = basename(argc ? argv[0] : "gi");

    if ( argc != 5 ) {
	fprintf(stderr, "usage: %s %s\n", pgm, usage);
	exit(1);
    }

    diameter = 0;
    for (i = 0; i < NR_DIAMETERS; i++ )
	if ( strcasecmp(argv[1], diameters[i].alias) == 0 ) {
	    diameter = diameters[i].diameter;
	    break;
	}

    if ( diameter == 0 ) {
	diameter = atoi(argv[1]);
	if ( diameter <= 0 ) {
	    fprintf(stderr, "%s: wheel diameter <%s>?\n", pgm, argv[1]);
	    exit(1);
	}
    }

    tire = atoi(argv[2]);

    if ( tire <= 0 ) {
	fprintf(stderr, "%s: tire size <%s>?\n", pgm, argv[2]);
	exit(1);
    }

    nr_chainrings = 0;
    for ( arg = strtok(argv[3], ","); arg; arg = strtok(0, ",") ) {
	if ( nr_chainrings >= MAX_CHAINRINGS ) {
	    fprintf(stderr, "%s: too many chainrings (%d max)\n", pgm, MAX_CHAINRINGS);
	    exit(1);
	}
	i = atoi(arg);
	if ( i <= 0 ) {
	    fprintf(stderr, "%s: chainring <%s>?\n", pgm, arg);
	    exit(1);
	}
	chainrings[nr_chainrings++] = i;
    }

    nr_cogs = 0;

    for ( arg = strtok(argv[4], ","); arg; arg = strtok(0, ",") ) {
	if ( nr_cogs >= MAX_COGS ) {
	    fprintf(stderr, "%s: too many cogs (%d max)\n", pgm, MAX_COGS);
	    exit(1);
	}
	i = atoi(arg);
	if ( i <= 0 ) {
	    fprintf(stderr, "%s: cog <%s>?\n", pgm, arg);
	    exit(1);
	}
	cogs[nr_cogs++] = i;
    }


    printf("%7s|", "");
    for ( j = 0; j < nr_chainrings; j++ )
	printf("%8d", chainrings[j]);
    putchar('\n');

    printf("%.7s+", "---------");
    for ( j = 0; j < nr_chainrings; j++ )
	printf("%.8s", "--------");
    putchar('\n');

    for ( i=0; i < nr_cogs; i++ ) {
	printf("%7d|", cogs[i]);

	for ( j = 0; j < nr_chainrings; j++ )
	    printf("%8d", gi(chainrings[j], cogs[i]));
	putchar('\n');
    }
}
