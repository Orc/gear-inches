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

#define if(x)	if((x))

char outputbuffer[10240];

int diameter;
int tire;

#define MAX_CHAINRINGS 3
#define MAX_COGS 12

int chainrings[1+MAX_CHAINRINGS];
int cogs[1+MAX_COGS];

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
    float gear_mm = (diameter + tire);


    if ( cog <= 0 )
	return EOF;

    gear_mm *= chainring;
    gear_mm /= cog;

    return gear_mm / 25.4;
}


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
	for (i = 0; i < NR_DIAMETERS; i++ )
	    if ( strcasecmp(val, diameters[i].alias) == 0 ) {
		diameter = diameters[i].diameter;
		break;
	    }

	if ( diameter == 0 )
	    diameter = number(val);
	
	if ( diameter < 0 )
	    error("wheel diameter [%s]?", val);
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
    printf("</t%c>\n", value, bold ? 'h' : 'd');
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

    setvbuf(stdout, outputbuffer, _IOFBF, sizeof outputbuffer);

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
		cellnum(0, "center",  gi(chainrings[j], cogs[i]));
	    
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
