/*
	Model T Shell
	2012-October
	Scott Lawrence
	yorgle@gmail.com
*/

/* 
    Copyright (C) 2012 Scott Lawrence

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* ********************************************************************** */
/* some string routines. -- these are used for formatting text to the 
** various horizontal rows of the display, simplifying displaying text
*/

void string_Prep( char * dest, int width )
{
	int i = 0;
	if( !dest ) return;
	/* assume dest is big enough */

	for( i=0 ; i<width ; i++ ) {
		dest[i] = ' ';
	}
	dest[i] = '\0';
}

void string_Overlay( char * dest, char * src )
{
	int i=0;

	if( !dest || !src ) return;

	while( src[i] != '\0' && dest[i] != '\0' )
	{
		dest[i] = src[i];
		i++;
	}

}

void string_Left( char * dest, char * src )
{
	if( !src || !dest ) return;
	string_Overlay( dest, src );
}

void string_Right( char * dest, char * src )
{
	int sl, dl;

	if( !src || !dest ) return;

	sl = strlen( src );
	dl = strlen( dest );

	string_Overlay( dest + (dl-sl), src );
}

void string_Center( char * dest, char * src )
{
	int sl, dl;

	if( !src || !dest ) return;

	sl = strlen( src );
	dl = strlen( dest );

	string_Overlay( dest + ((dl - sl)/2), src );
}


/* ********************************************************************** */

int string_sameCI( char * a, char * b )
{
	if( !a && !b ) return 1;
	if( !a || !b ) return 0;
	while( *a && *b ) {
		if( toupper(*a) != toupper(*b) ) return 0;
		a++; b++;
	}
	if( toupper(*a) == toupper(*b) ) return 1;
	return 0;
}

int string_startsWith( char * haystack, char * needle )
{
	if( !haystack || !needle ) return 0;

	if( !strncmp( haystack, needle, strlen( needle )) ) {
		return 1; 
	}

	return 0;
}

char * string_fileExtension( char * fn )
{
	char * dot = NULL;

	if( !fn ) return NULL;

	/* find the last "." */
	while( *fn != '\0' ) {
		if( *fn == '.' ) dot = fn;
		fn++;
	}
	return dot;
}
