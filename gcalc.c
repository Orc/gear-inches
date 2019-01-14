/*
 * gear inch calculator
 *
 * usage: http://.../gcalc.cgi
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <libgen.h>
#include <stdarg.h>
#include <inttypes.h>
#include <limits.h>

#include "functions.h"

#define if(x)	if((x))

int diameter;
int tire;

#define MAX_CHAINRINGS 3
#define MAX_COGS 12

int chainrings[1+MAX_CHAINRINGS];
int cogs[1+MAX_COGS];

int nr_chainrings;
int nr_cogs;


char iobuf[10240];

char errorbuf[2048];
int errorsize;

#define EEND	(errorbuf + errorsize)
#define ESIZE	(sizeof errorbuf - errorsize)

void
noerrors()
{
    memset(errorbuf, 0, sizeof errorbuf);
    errorsize = 0;
}

void
error(char *fmt, ...)
{
    va_list ptr;

    va_start(ptr,fmt);

    errorsize += snprintf(EEND, ESIZE, "<li>");
    errorsize += vsnprintf(EEND, ESIZE, fmt, ptr);
    errorsize += snprintf(EEND, ESIZE, "</li>");
    
    va_end(ptr);
}


int
has_errors()
{
    return errorsize > 0;
}


int
number(char *s)
{
    char *end;
    long value = strtoul(s, &end, 10);

    if ( *end ) 
	return -1;

    return (int) value;
}


char *
getnzenv(char *variable)
{
    char *p = getenv(variable);

    if ( p && (strlen(p) > 0) )
	return p;

    return 0;
}


void
populate()
{
    char *val;
    char variable[80];
    int i, teeth;

    diameter = 0;
    tire = 0;
    memset(chainrings, 0, sizeof chainrings);
    memset(cogs, 0, sizeof cogs);
    
    noerrors();
    
    if ( !getnzenv("WWW_cgi") )
	return;

    if ( val = getnzenv("WWW_diameter") ) {
	if ( (diameter = wheel_diameter(val)) == EOF )
	    error("wheel diameter [%s]?", val);
	else if ( diameter == 0 )
	    error("zero diameter wheel?");
    }
    else
	error("no tire diameter?");

    if ( val = getnzenv("WWW_tire") ) {
	tire = number(val);

	if ( tire < 0 )
	    error("tire size [%s]?", val);
    }
    else
	error("no tire size?");
    
    for ( nr_chainrings = i = 0; i <= MAX_CHAINRINGS; i++ ) {
	sprintf(variable, "WWW_ring%d", i);

	if ( val = getnzenv(variable) ) {
	    if ( (teeth = number(val)) < 0 )
		error("chainring [%s]?", val);
	    if ( teeth > 0 )
		chainrings[++nr_chainrings] = teeth;
	}
    }

    if ( nr_chainrings < 1 )
	error("no chainrings?");

    for ( nr_cogs = i = 0; i <= MAX_COGS; i++ ) {
	sprintf(variable, "WWW_cog%d", i);

	if ( val = getnzenv(variable) ) {
	    if ( (teeth = number(val)) < 0 )
		error("cog [%s]?", val);
	    if ( teeth > 0 )
		cogs[++nr_cogs] = teeth;
	}
    }

    if ( nr_cogs < 1)
	error("no cogs?");
}


void
cell(int bold, char *align, char *text)
{
    printf("    <t%c", bold ? 'h' : 'd');
    if ( align )
	printf(" align=\"%s\"", align);
    printf(">%s</t%c>\n", text ? text : "", bold ? 'h' : 'd');
}


void
cellnum(int bold, char *align, int value)
{
    printf("    <t%c", bold ? 'h' : 'd');
    if ( align )
	printf(" align=\"%s\"", align);
    putchar('>');
    if ( value == EOF )
	printf("-?-");
    else
	printf("%d", value);
    printf("</t%c>\n", bold ? 'h' : 'd');
}

void
row()
{
    puts("<tr>");
}

void
end()
{
    puts("</tr>");
}


void
input(char *name, int size, char *value)
{
    printf("    <input type=\"text\" name=\"%s\", size=\"%d\" value=\"%s\" />\n",
	    name, size, value ? value : "");
}
 

void
show_form()
{
    int i, j;
    char name[80];

    printf("Content-Type: text/html\n\n");


    puts("<html>");
    puts("<head><title>Gear Inch Calculator</title></head>");

    puts("<body>");

    puts("<form method=\"post\" action=\"gcalc.cgi\">");
    puts("<table>");
    row();
    cell(1, "right", "Wheel Diameter");
    puts("    <td>");
    input("diameter", 6, getnzenv("WWW_diameter"));
    puts("    </td>");
    end();

    row();
    cell(1, "right", "Tire Size");
    puts("    <td>");
    input("tire", 3, getnzenv("WWW_tire"));
    puts("    </td>");
    end();

    row();
    cell(1, "right", "Chainring(s)");
    puts("    <td>");
    for (i=1; i<=MAX_CHAINRINGS; i++) {
	sprintf(name, "WWW_ring%d", i);
	input(name+4, 2, getnzenv(name));
    }
    puts("    </td>");
    end();

    row();
    cell(1, "right", "Cog(s)");
    puts("    <td>");
    for (i=1; i<=MAX_COGS; i++) {
	sprintf(name, "WWW_cog%d", i);
	input(name+4, 2, getnzenv(name));
    }
    puts("    </td>");
    end();

    row();
    puts("    <td></td>");
    puts("    <th align=\"left\">");
    puts("    <input type=\"hidden\" id=\"cgi\" name=\"cgi\" value=\"T\" />");
    puts("    <input type=\"submit\" value=\"tell me my gear inches\"/>");
    puts("    </th>");
    end();

}


int /* Siiiiiigh, fine */
main(argc, argv)
char **argv;
{
    int i, j;
    char *arg;
    char strval[10];

    void uncgi();

    uncgi();


    setvbuf(stdout, iobuf, _IOFBF, sizeof iobuf);

    populate();
    show_form();

    if ( has_errors() ) {
	puts("</table>");
	puts("<p>Oops?</p>");
	puts("<ul>");
	puts(errorbuf);
	puts("</ul>");
    }
    else if ( nr_chainrings && nr_cogs ) {
	row();
	puts("    <td></td>");
	puts("    <td><table style=\"border:1px solid black\">");
	
	row();
	cell(0,0,0);
	for ( i=1; i <= nr_chainrings; i++ )
	    cellnum(1, 0, chainrings[i]);
	end();
	
	for ( i=1; i <= nr_cogs; i++ ) {
	    row();
	    cellnum(1, 0, cogs[i]);

	    for ( j = 1; j <= nr_chainrings; j++ )
		cellnum(0, "center",  gear_inches(chainrings[j], cogs[i]));
	    
	    end();
	}
	puts("    </table>");
	end();
	
	puts("</table>");
    }
    else
	puts("</table>");
    puts("</form>");
    puts("</body>");
    puts("</html>");
    exit(0);
}
