#include <stdio.h>  /* for EOF definition */
#include <strings.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>

#include "functions.h"

struct _diameter { int diameter; char *alias; } ;

struct _diameter diameters[] = {
    { 642, "700a" },
    { 635, "700b" },
    { 622, "700c" },
    { 587, "700d" },
    { 590, "650a" },
    { 584, "650b" },
    { 571, "650c" },
    { 540, "600a" },
    { 490, "550a" },
    { 440, "500a" },
    { 390, "450a" },
    { 340, "400a" },
    { 584, "27.5" },
    { 584, "27.5\"" },
};
#define NR_DIAMETERS (sizeof diameters / sizeof diameters[0])

struct _diameter inches[] = {
    { 622, "29" },
    { 630, "27" },
    { 559, "26" },
    { 520, "24" },
    { 451, "20" },
};
#define NR_INCHES (sizeof inches / sizeof inches[0])


static int
lookup(char *s, struct _diameter dia[], int nr_dia)
{
    int i;
    
    for ( i=nr_dia-1; i >= 0; --i)
	if ( strcasecmp(s, dia[i].alias) == 0 )
	    return i;

    return EOF;
}


int diameter, tire;

/* check wheel diameter parameter for well-known aliases */
int
wheel_diameter(char *s)
{
    int i;
    char *e;
    char *sfx;


    if ( (i = lookup(s, diameters, NR_DIAMETERS)) != EOF )
	return diameters[i].diameter;

    i = strtoul(s,&e,10);

    if ( *e ) {
	for (sfx=e; isspace(*sfx); ++sfx)
	    ;
	
	if ( strcmp(sfx, "inch") == 0 ) {
	    *e = 0;

	    if ( (i = lookup(s, inches, NR_INCHES)) != EOF )
		return inches[i].diameter;
	}
	else if ( strcmp(sfx, "\"") == 0 ) {
	    *e = 0;

	    if ( (i=lookup(s,inches, NR_INCHES)) != EOF )
		return inches[i].diameter;
	}
	else if ( strcmp(sfx, "mm") != 0 ) {
	    /* trailing garbage on token */
	    return EOF;
	}
    }

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
