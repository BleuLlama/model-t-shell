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
#include "conf.h"
#include "items.h"

/* ********************************************************************** */

anItem *itemList = NULL;

int exitNow = 0;
int selection = 0;
int gridtall = 1;
int gridwide = 1;

int nItems = 0;
int maxItems = 0;

/*
char * internalKeywords[ 8 ] =
	{ "BASIC", "TEXT", "TELECOM", "ADDRESS", "SCHEDULE", "CONFIG", "EXIT", NULL };
*/

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


void items_Add( char * display, char * full, int flags )
{
	strncpy( itemList[ nItems ].display, display, kMaxBuf );
	if( full ) itemList[ nItems ].full = strdup( full );
	itemList[ nItems ].flags = flags;
	nItems++;
}


void items_Populate( void )
{
	int f;
	confItem * ci;

	/*int maxItems = gridtall * gridwide; */
	int idx;
	/*int ii; */

	/* first, clear the entire array */
	for( idx = 0 ; idx < maxItems ; idx++ ) {
		itemList[idx].flags = kFlagEmpty;
		if( itemList[idx].full ) free( itemList[idx].full );
		itemList[idx].full = NULL;
	}
	nItems = 0;

	/* now fill it accordingly:

		1. Places = All keys labelled "Places.DISPLAYNAME"
		2. Complete to end of row
		3. Row of spacers

		4. Verbs -  all keys labelled "Verbs.DISPLAYNAME"
		5. Complete to end of row
		6. Row of spacers


		7. Nouns - Current directory contents.
			ITEM
			DIRECTORY/
			EXECUTABLE*
	*/

	/* and pad to the end of the row */
#define SPACERS_TO_END_OF_ROW() \
	while( nItems % gridwide ) items_Add( kSpacerItem, NULL, kFlagSpacer );

#define SPACERS_FOR_A_ROW() \
	do { \
		items_Add( kSpacerItem, NULL, kFlagSpacer ); \
	} while( nItems % gridwide )


	/* Places */
	f = conf_FirstInGroup( "Places" );
	while( f > 0 ) {
		ci = conf_Item( f );
		if( ci ) {
			/* add the item */
			items_Add( &ci->key[7], ci->value, kFlagDirectory | kFlagAbsolute );
		}
		
		f = conf_NextInGroup( "Places", f );
	}
	SPACERS_TO_END_OF_ROW();


	/* subfolders */
	do {
		int ii = 1;

		struct stat status;
		struct dirent *theDirEnt;
		DIR * theDir = opendir( conf_Get( "Places.Cwd" ));
		char fullpath[1024];
		int skipDotFiles = conf_GetInt( "System.SkipDotFiles" );


		if( !theDir ) continue;

		theDirEnt = readdir( theDir );
		idx = 0;
		while( theDirEnt && idx < maxItems ) {
			int skip = 0;

			/* always skip */
			if( !strcmp( theDirEnt->d_name, "." )) skip = 1;
			if( !strcmp( theDirEnt->d_name, ".." )) skip = 1;

			if( skipDotFiles && theDirEnt->d_name[0] == '.' ) skip = 1;

			if( !skip ) {
				snprintf( fullpath, 1024, "%s/%s", conf_Get( "Places.Cwd" ), theDirEnt->d_name );
				stat( fullpath, &status );
				if( status.st_mode & S_IFDIR ) {
					if( ii == 1 ) 
						items_Add( theDirEnt->d_name, theDirEnt->d_name, kFlagDirectory | kFlagAbsolute );
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

	SPACERS_TO_END_OF_ROW();
	SPACERS_FOR_A_ROW();


	/* Verbs */
	f = conf_FirstInGroup( "Verbs" );
	while( f > 0 ) {
		ci = conf_Item( f );
		if( ci ) {
			/* add the item */
			if( ci->value[0] == '%' ) {
				items_Add( &ci->key[6], &ci->value[1], kFlagItem | kFlagInternal );
			} else {
				items_Add( &ci->key[6], ci->value, kFlagItem );
			}
		}
		
		f = conf_NextInGroup( "Verbs", f );
	}
	SPACERS_TO_END_OF_ROW();
	SPACERS_FOR_A_ROW();
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

void items_SelectNextSection( void )
{
	/* iterate through the selection index to find:
		1. next item after whitespace
		or
		2. n
	*/
	int maxitems = (gridtall * gridwide);
	int lastFlags = itemList[selection].flags;

	do {
		selection++;
		if( selection > maxitems ) {
			selection = 0;
			return;
		}

		if( (lastFlags & kFlagSpacer) == kFlagSpacer ) {
			if( (itemList[selection].flags & kFlagSpacer) != kFlagSpacer ) {
				return;
			}
		}
		lastFlags = itemList[selection].flags;
	} while( 1 );
}

char * items_GetDisplay( int idx )
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
		return itemList[ idx ].display;
	} 
	return kEmptyItem;
}
