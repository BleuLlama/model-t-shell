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

/* ********************************************************************** */
/* some string routines. -- these are used for formatting text to the 
** various horizontal rows of the display, simplifying displaying text
*/

/* prepare a string
 * - fill with ' ', end witn '\0' 
 */
void string_Prep( char * dest, int width );

/* overlay source onto destination
 * - basically stringcopy without the null
 */
void string_Overlay( char * dest, char * src );

/* add src to the beginning of dest */
void string_Left( char * dest, char * src );

/* add src to the end of dest */
void string_Right( char * dest, char * src );

/* add src to the middle of dest */
void string_Center( char * dest, char * src );

/* ********************************************************************** */

int string_sameCI( char * a, char * b );
int string_startsWith( char * haystack, char * needle );
char * string_fileExtension( char * fn );
