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

anItem *itemList = NULL;

int exitNow = 0;
int selection = 0;
int gridtall = 1;
int gridwide = 1;

int nItems = 0;
int maxItems = 0;

char * internalKeywords[ 8 ] =
	{ "BASIC", "TEXT", "TELECOM",
	"ADDRESS", "SCHEDULE", "CONFIG", "EXIT", NULL };

void items_Init( int mx, int my )
{
	int pad = 1;
	int nmi = 0;

	gridwide = (int) floor( mx/14 );
	gridtall = my -2 - (pad * 2);

	nmi = gridwide * gridtall;

	if( nmi == maxItems ) return;

	if( itemList ) {
		int idx;
		for( idx=0; idx<maxItems ; idx++ ) {
			if( itemList[idx].full ) free( itemList[idx].full );
		}
		free( itemList );
	} 

	maxItems = gridwide * gridtall;

	itemList = (anItem *)calloc( maxItems, sizeof( anItem ) );
}


void items_Add( char * name, char * full, int flags )
{
	strncpy( itemList[ nItems ].name, name, kMaxBuf );
	if( full ) itemList[ nItems ].full = strdup( full );
	itemList[ nItems ].flags = flags;
	nItems++;
}


void items_Populate( void )
{
	/*int maxItems = gridtall * gridwide; */
	int idx;
	int ii;

	/* first, clear the entire array */
	for( idx = 0 ; idx < maxItems ; idx++ ) {
		itemList[idx].flags = kFlagEmpty;
		if( itemList[idx].full ) free( itemList[idx].full );
		itemList[idx].full = NULL;
	}
	nItems = 0;

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

	/* and pad to the end of the row */
#define SPACERS_TO_END_OF_ROW() \
	while( nItems % gridwide ) items_Add( kSpacerItem, NULL, kFlagSpacer );

#define SPACERS_FOR_A_ROW() \
	do { \
		items_Add( kSpacerItem, NULL, kFlagSpacer ); \
	} while( nItems % gridwide )


	for( ii=0 ; ii<3 ; ii++ ) {
		if( ii==0 ) { /* verbs */
			/* initial: internals */
			for( idx = 0 ; internalKeywords[idx] != NULL ; idx++ )
			{
				items_Add( internalKeywords[idx], NULL, kFlagInternal | kFlagItem );
			}

		}

		if( ii==1 ) { /* places */
			/* now the parent directory item */
			items_Add( kNameParent, kNameParent, kFlagInternal | kFlagDirectory );
		}

		if( ii==2 ) { /* nouns */
		}


		/* then, append a current directory listing */
		do {
			struct stat status;
			struct dirent *theDirEnt;
			DIR * theDir = opendir( cwd );
			char fullpath[1024];

			if( !theDir ) continue;

			theDirEnt = readdir( theDir );
			while( theDirEnt && idx < maxItems ) {
				int skip = 0;

				/* always skip */
				if( !strcmp( theDirEnt->d_name, "." )) skip = 1;
				if( !strcmp( theDirEnt->d_name, ".." )) skip = 1;

	#ifdef kSkipDotFiles
				if( theDirEnt->d_name[0] == '.' ) skip = 1;
	#endif

				if( !skip ) {
					snprintf( fullpath, 1024, "%s/%s", cwd, theDirEnt->d_name );
					stat( fullpath, &status );
					if( status.st_mode & S_IFDIR ) {
						if( ii == 1 ) 
							items_Add( theDirEnt->d_name, theDirEnt->d_name, kFlagDirectory );
					} else if( status.st_mode & S_IXUSR ) {
						if( ii == 0 ) 
							items_Add( theDirEnt->d_name, theDirEnt->d_name, kFlagExecutable );


					} else if( ii==2 )/* S_ISREG, link, etc */ {
						items_Add( theDirEnt->d_name, theDirEnt->d_name, kFlagItem );
					}
				}

				theDirEnt = readdir( theDir );
			}
			closedir( theDir );

		} while( 0 );



		if( ii==0 ) { /* verbs */
			/* finally: spacers */
			SPACERS_TO_END_OF_ROW();
			SPACERS_FOR_A_ROW();
		}

		if( ii==1 ) { /* places */
			SPACERS_TO_END_OF_ROW();
			SPACERS_FOR_A_ROW();
		}

		if( ii==2 ) { /* nouns */
		}
	}

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
	
	if( idx < 0 || idx >= maxItems ) {
		return "--";
	}

	if( itemList[idx].flags != kFlagEmpty ) {
		return itemList[ idx ].name;
	} 
	return kEmptyItem;
}
