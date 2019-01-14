#include <stdio.h>  /* for EOF definition */
#include <strings.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

#include "functions.h"

struct { int diameter; char *alias; } diameters[] = {
    { 630, "27" },
    { 622, "700c" },
    { 590, "650a" },
    { 587, "700d" },
    { 584, "650b" },
    { 584, "27.5" },
    { 571, "650c" },
    { 559, "26" },
};

#define NR_DIAMETERS (sizeof diameters / sizeof diameters[0])


int diameter, tire;

/* check wheel diameter parameter for well-known aliases */
int
wheel_diameter(char *s)
{
    int i;
    char *e;


    for ( i=0; i<NR_DIAMETERS; i++ )
	if ( strcasecmp(s, diameters[i].alias) == 0 )
	    return diameters[i].diameter;

    i = strtoul(s,&e,10);

    if ( *e )					/* trailing garbage on token */
	return EOF;

    if ( i == ULONG_MAX && errno == ERANGE )	/* overflow */
	return EOF;

    return i;
} /* wheel_diameter */


/* compute gear inches */
int
gear_inches(int chainring, int cog)
{
    float gear_mm = (diameter + tire);


    if ( cog <= 0 )
	return EOF;

    gear_mm *= chainring;
    gear_mm /= cog;

    return gear_mm / 25.4;
} /* gear_inches */
