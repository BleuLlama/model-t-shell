/*
	Model T Shell - conf
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

typedef struct confItem {
	char key[kMaxBuf];
	char value[kMaxBuf];
	char d0[kMaxBuf];
} confItem;


void conf_Init( void );
void conf_Load( void );
void conf_Save( void );

confItem * conf_Find( char * key );

char * conf_Get( char * key );
int conf_GetInt( char * key );

void conf_Set( char * key, char * val );
void conf_SetInt( char * key, int val );

char * conf_TrimString( char * in );
char * conf_InGroup( char * group, char * key );

/* for accessing, externally */
int conf_NextInGroup( char * group, int after );
#define conf_FirstInGroup( G ) \
	conf_NextInGroup( G, -1 )
confItem * conf_Item( int idx );
