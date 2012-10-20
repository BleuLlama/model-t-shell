/*
	Model T Shell - items
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
#include <stdio.h>
/* ********************************************************************** */

/* size per item for display */
#define kItemSize 14

/* item flags */
#define kFlagEmpty 	0x00		/* item slot is empty */
#define kFlagItem  	0x01		/* item is occupied */
#define kFlagInternal 	0x02		/* item is an internal verb */
#define kFlagExecutable 0x10		/* item is runnable */
#define kFlagDirectory  0x20		/* item is a directory */
#define kFlagSpacer	0x80		/* item is blank space */

#define kNameParent ".. (Parent)"	/* parent directory */
#define kEmptyItem "--.--"		/* empty screen slot */
#define kSpacerItem " "			/* spacer */

typedef struct anItem {
	char name[kItemSize];
	char * full;
	int flags;
} anItem;

extern anItem *itemList;

extern int exitNow;
extern int selection;
extern int gridtall;
extern int gridwide;


void items_Init( int mx, int my );

void items_Populate( void );

void items_SelectDelta( int dx, int dy );
void items_Select( int idx );

char * items_GetName( int idx );
