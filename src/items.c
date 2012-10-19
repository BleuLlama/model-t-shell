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
/*
 * #if defined __APPLE__ || defined __linux__ || defined __MINGW32__
 * #endif
*/
#include <curses.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <dirent.h>	/* for directory listings */

#include <sys/types.h>	/* for directory scanning */
#include <sys/stat.h>	/* for directory scanning */

#include <time.h>	/* for localtime, time */
#include <ctype.h>	/* for isprint */

#include "vals.h"
#include "utils.h"
#include "items.h"

/* ********************************************************************** */

anItem _itemList[kMaxItems];
anItem *itemList = &_itemList[0];

int exitNow = 0;
int selection = 0;
int gridtall = 1;
int gridwide = 1;

char * internalKeywords[ 8 ] =
	{ "BASIC", "TEXT", "TELECOM",
	"ADDRESS", "SCHEDULE", "CONFIG", "EXIT", NULL };

void items_Init( int mx, int my )
{
	int pad = 1;

	gridwide = (int) floor( mx/14 );
	gridtall = my -2 - (pad * 2);
}

void items_Populate( void )
{
	/*int maxItems = gridtall * gridwide; */
	int idx;

	/* first, clear the entire array */
	for( idx = 0 ; idx < kMaxItems ; idx++ ) {
		itemList[idx].flags = kFlagEmpty;
		if( itemList[idx].full ) free( itemList[idx].full );
		itemList[idx].full = NULL;
	}

	/* possible options for this:
		1. everything.  internal, .., directory contents 
			- messy, hard to understand
		2. partial updated: (Current)
			keywords, spacer, .., directory contents
		3. Operational based:
			verbs (keywords, exeutables in the current dir
			spacer
			locations (directories)
			spacer
			nouns (items in the current dir)
	*/

	/* this needs range checking! */

	/* first, copy over the internal keywords */
	for( idx = 0 ; internalKeywords[idx] != NULL ; idx++ )
	{
		strncpy( itemList[idx].name, internalKeywords[idx], kMaxBuf );
		itemList[idx].flags = kFlagInternal | kFlagItem;
	}

	/* and pad to the end of the row */
#define SPACERS_TO_END_OF_ROW() \
	while( (idx % gridwide) != 0 ) {\
		strncpy( itemList[idx].name, kSpacerItem, kMaxBuf ); \
		itemList[idx].flags = kFlagSpacer; \
		idx++; \
	}

#define SPACERS_FOR_A_ROW() \
	do { \
		strncpy( itemList[idx].name, kSpacerItem, kMaxBuf ); \
		itemList[idx].flags = kFlagSpacer; \
		idx++; \
	} while( (idx % gridwide) != 0 )

	SPACERS_TO_END_OF_ROW();
	SPACERS_FOR_A_ROW();

	/* now the parent directory item */
	do {
		strncpy( itemList[idx].name, kNameParent, kMaxBuf );
		itemList[idx].full = strdup( kNameParent );
		itemList[idx].flags = kFlagInternal | kFlagDirectory;
		idx++;
	} while( 0 );

/*
	SPACERS_TO_END_OF_ROW();
	SPACERS_FOR_A_ROW();
*/

	/* then, append a current directory listing */
	do {
		struct stat status;
		struct dirent *theDirEnt;
		DIR * theDir = opendir( cwd );
		char fullpath[256];

		if( !theDir ) continue;

		theDirEnt = readdir( theDir );
		while( theDirEnt && idx < kMaxItems ) {
			int skip = 0;

			/* always skip */
			if( !strcmp( theDirEnt->d_name, "." )) skip = 1;
			if( !strcmp( theDirEnt->d_name, ".." )) skip = 1;

#ifdef kSkipDotFiles
			if( theDirEnt->d_name[0] == '.' ) skip = 1;
#endif
			if( !skip ) {
				itemList[idx].full = strdup( theDirEnt->d_name );
				strncpy( itemList[idx].name, theDirEnt->d_name, kItemSize );
				snprintf( fullpath, 256, "%s/%s", cwd, theDirEnt->d_name );
				stat( fullpath, &status );
				if( status.st_mode & S_IFDIR ) {
					itemList[idx].flags = kFlagDirectory;
				} else if( status.st_mode & S_IXUSR ) {
					itemList[idx].flags = kFlagExecutable;
				} else /* S_ISREG, link, etc */ {
					itemList[idx].flags = kFlagItem;
				}
				idx++;
			}

			theDirEnt = readdir( theDir );
		}
		closedir( theDir );

	} while( 0 );

	/* and tweak a pointer */
	itemList = &_itemList[0];
}

void items_SelectDelta( int dx, int dy )
{
	int maxitems = (gridtall * gridwide);

	if( dx ) {
		selection += dx;
		if( selection < 0 ) selection = 0;
		if( selection >= maxitems-1 ) selection = maxitems -1;
	}

	if( dy < 0 ) {
		if( selection >= gridwide ) selection -= gridwide;
	}
	if( dy > 0 ) {
		if( (selection + gridwide) <= maxitems-1) selection += gridwide;
	}
}

void items_Select( int idx )
{
	if( idx < 0 ) idx = 0;
	if( idx > 9999 ) idx = 0;

	selection = idx;
}

char * items_GetName( int idx )
{
	/* originally were 11 characters,
		"TESTFILE.BA"
	   we'll stick with 12
		"TESTFILE.BAS"
	   with one space at the start and one at the end yields 14 chars
	*/

/*
	if( itemList[idx].flags == kFlagSpacer ){
		return kSpacerItem;
	}
*/
	
	if( itemList[idx].flags != kFlagEmpty ) {
		return itemList[ idx ].name;
	} 
	return kEmptyItem;
}
