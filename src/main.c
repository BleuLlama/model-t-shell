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

#include "version.h"
#include "vals.h"
#include "utils.h"
#include "items.h"

/* ********************************************************************** */

/* initScreen
 *	initialize the curses screen, and get it all setup the way we want
 */
void initScreen( void )
{
	initscr();
	cbreak();
	noecho();
	timeout( 0 );
	curs_set( 0 );

	nonl();
	intrflush( stdscr, FALSE );
	keypad( stdscr, TRUE );

	mousemask( ALL_MOUSE_EVENTS, NULL );

	start_color();
	init_pair( kColorTopBar, COLOR_BLACK, COLOR_GREEN );
	init_pair( kColorBottomBar, COLOR_WHITE, COLOR_BLUE );
	init_pair( kColorText, COLOR_WHITE, COLOR_BLACK );
	init_pair( kColorTextSelected, COLOR_BLACK, COLOR_YELLOW );
}


/* deinitScreen
 *
 *	close down curses, restore stuff
 */
void deinitScreen( void )
{
	curs_set( 1 );
	endwin();
}

/* ********************************************************************** */
WINDOW * win;
int fullRedraw = 1;
int winw, winh;

char userInput[kMaxBuf];

void showTopBar( int mx, int my )
{
	int x1, x2;
	char tbuf[ kMaxBuf ];
	time_t rawtime;
	struct tm *timeinfo;

	/****** First, do the top bar ******/
	wattron( win, COLOR_PAIR( kColorTopBar ) );

	/* do the right side first */
	snprintf( tbuf, kMaxBuf, "%s - Model T Shell v%s", whoami(), kVersion );
	x2 = winw - strlen( tbuf );
	wmove( win, 0, x2 );
	wprintw( win, "%s", tbuf );

	/* then the right side */
	time( &rawtime );
	timeinfo = localtime( &rawtime );

#ifdef kUse24HourTime
	strftime( tbuf, kMaxBuf, "%b %d,%Y %a %H:%M:%S", timeinfo );
#else
	strftime( tbuf, kMaxBuf, "%b %d,%Y %a %I:%M:%S %p", timeinfo );
#endif

	x1 = strlen( tbuf );
	wmove( win, 0, 0 );
	wprintw( win, "%s", tbuf );

	/* and fill it in */
	wmove( win, 0, x1 );
	for( ; x1<x2 ;x1++ )
	{
		wprintw( win, " " );
	}
	
	wattroff( win, COLOR_PAIR( kColorTopBar ));
}


void showBottomBar( int mx, int my )
{
	int x1, x2;
	char tbuf[ kMaxBuf ];

	/* right portion */
	wattron( win, COLOR_PAIR( kColorBottomBar ));
	tbuf[0] = '\0';
	if( itemList[ selection ].flags == kFlagEmpty ) {
	} else if( itemList[ selection ].flags & kFlagInternal ) {
		snprintf( tbuf, kMaxBuf, "%s", cwd );
	} else if( itemList[ selection ].flags & kFlagDirectory ) {
		snprintf( tbuf, kMaxBuf, "directory" );
	} else if( itemList[ selection ].flags & kFlagExecutable ) {
		snprintf( tbuf, kMaxBuf, "run" );
	} else if( itemList[ selection ].flags & kFlagItem ) {
		snprintf( tbuf, kMaxBuf, "file" );
	} else {
		snprintf( tbuf, kMaxBuf, " " );
	}

	x2 = winw - strlen( tbuf );
	wmove( win, winh-1, x2 );
	wprintw( win, "%s", tbuf );

	/* left portion */
	snprintf( tbuf, kMaxBuf, "Select: %s", userInput );
	x1 = strlen( tbuf );
	wmove( win, winh-1, 0 );
	wprintw( win, "%s", tbuf );

	/* fill it in */
	wmove( win, winh-1, x1 );
	for( ; x1<x2 ; x1++ )
	{
		wprintw( win, " " );
	}

	wattroff( win, COLOR_PAIR( kColorBottomBar ));
}

void clearInput( void )
{
	userInput[0] = '_';
	userInput[1] = '\0';
}

void copyItemToUserInput( void )
{
	if( itemList[ selection ].flags & kFlagItem ) {
		strcpy( userInput, itemList[ selection ].name );
	} else {
		strcpy( userInput, "" );
	}
	strcat( userInput, "_" );
}


void executeSelection( void )
{
	/* remove the underscore */
	userInput[ strlen( userInput )-1 ] = '\0';

	if( itemList[ selection ].flags & kFlagDirectory )
	{
		changeDirectory( itemList[ selection ].full );
		items_Populate();
	}

	/* check keywords */
	if(    sameCI( "EXIT", userInput )
            || sameCI( "EXIT", items_GetName( selection ))) {
		exitNow = 1;
		return;
	}

	clearInput();
}


/* ********************************************************************** */

void showMiddleBlob( int mx, int my )
{
	int pad = 1; 	/* additional spaces at the top and bottom */
	int col;		/* current item color */
	int cidx;		/* index for rendering */
	int x,y;

	/* determine our grid size */
	items_Init( mx, my );

	cidx = 0;

	for( y=0 ; y<gridtall ; y++ )
	{
		for( x=0 ; x<gridwide ; x++ )
		{
			if( cidx == selection ) {
				col = COLOR_PAIR( kColorTextSelected );
			} else {
				col = COLOR_PAIR( kColorText );
			}

			wattron( win, col );
			wmove( win, y+1 + pad,
				    x*14 );
			wprintw( win, " %-12s ", items_GetName( cidx ));
			wattroff( win, col );

			cidx++;
		}
	}
}


/* showDisplay
 *
 *	display the editor window to the screen
 */	
void showDisplay( int mx, int my )
{
	if( !win ) {
		winw = mx;
		winh = my;
		win = newwin( winh, winw, my-winh, 0 );

		/* we don't really need to update this every frame */
		/* wclear( ew ); */
/*
		wattron( win, COLOR_PAIR( kColorTopBar ) );
		wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
		wattroff( win, COLOR_PAIR( kColorTopBar ) );
*/
	}

	if( fullRedraw ) {
		fullRedraw = 0;
		wclear( win );
/*
		wattron( win, COLOR_PAIR( kColorTopBar ) );
		wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
		wattroff( win, COLOR_PAIR( kColorTopBar ) );
*/
	}

	showTopBar( mx, my );
	showBottomBar( mx, my );
	showMiddleBlob( mx, my );

	/* wrefresh( win ); */
	wnoutrefresh( win );
}


int isValidInput( int ch )
{
	if( isprint( ch )) return 1;
	return 0;
}

int handleKey( int ch )
{
	int ret = 1;
	int len = 0;

	switch( ch ) {
	case( KEY_DOWN ):
		items_SelectDelta( 0, 1 );
		copyItemToUserInput();
		break;

	case( KEY_UP ):
		items_SelectDelta( 0, -1 );
		copyItemToUserInput();
		break;

	case( KEY_LEFT ):
		items_SelectDelta( -1, 0 );
		copyItemToUserInput();
		break;

	case( KEY_RIGHT ):
		items_SelectDelta( 1, 0 );
		copyItemToUserInput();
		break;

	case( KEY_HOME ):
		items_Select( 0 );
		copyItemToUserInput();
		break;

	case( KEY_END ):
		items_Select( -1 );
		copyItemToUserInput();
		break;

	case( KEY_ENTER ):
	case( '\n' ): case( '\r' ):
		/* do something with the selection/text */
		executeSelection();
		break;

	case( 127 ):
	case( KEY_BACKSPACE ):
	case( KEY_DC ):
		/* remove a character */
		len = strlen( userInput );
		if( len >= 2  ) {
			userInput[ len-2 ] = '_';
			userInput[ len-1 ] = '\0';
		}
		

	default:
		/* tack it on if we can */
		len = strlen( userInput );
		if( isValidInput( ch ) && len < 32 ) {
			userInput[ len-1 ] = ch;
			userInput[ len ] = '_';
			userInput[ len+1 ] = '\0';
		}
		ret = 0;
		break;
	}
	
	return ret;
}

/* handleMouse
 *
 *	handle mosue clicks and such
 */
int handleMouse( int ch )
{
	int ret = 0;
	int my = 0;
	int mx = 0;

	/*MEVENT event; */
	my=my; mx=mx;

	if( ch != KEY_MOUSE ) return ret;

	getmaxyx( stdscr, my, mx );
	return ret;
}

int handleResize( int ch )
{
	if( ch != KEY_RESIZE ) return 0;

	delwin( win );
	win = NULL;

	return 1;
}


void doCursesInterface( void )
{
	int ch = 0;
/*
	int lastch = 0;
*/
	int mx = 0, my = 0;

	/* initialize input */
	clearInput();

	exitNow = 0;

	/* inital population of the screen */
	changeDirectory( NULL ); /* current directory */

	/* kickstart our item list */
	getmaxyx( stdscr, my, mx );
	items_Init( mx, my );
	items_Populate();

	/* display the TUI */
	while( !exitNow )
	{
		getmaxyx( stdscr, my, mx );

		ch = getch();
		if( handleResize( ch )) ch = -1;

		if( ch == -1 )
		{
			/*clear();*/
			showDisplay( mx, my );
			doupdate();
/*
		} else {
			lastch = ch;
*/
		}

		handleMouse( ch );
		handleKey( ch );
	}
	
	/* shut down the TUI */
	deinitScreen();
}



/* ********************************************************************** */

/* usage
 *
 *	display command line usage
 */
void usage( char * pn )
{
	fprintf( stderr, "Model \"T\" Shell v%s  %s Scott Lawrence, yorgle@gmail.com\n",
		kVersion, kDate );
	fprintf( stderr, "Usage:\n" );
	fprintf( stderr, "       %s\n", pn );

}


/* handleOptions
 *
 *	handle some command line options (i know it's messy, sorry.)
 */
int handleOptions( int argc, char ** argv )
{
	if( argc != 1 ) {
		usage( argv[0] );
		return -1;
	}

/*
	int ret = 0;
	int ac = 1;
	int i;

	char buf[512];
	for( ac = 1 ; ac<argc ; ac++ ) {
		if( !strcmp( argv[ac], "-xxxyyy" )) {
			ac++;
			if( argv[ac] ) {
				float junk = atof( argv[ac] );
			}
		} else 

		if( !strcmp( argv[ac], "-zzzyyyy" )) {
			int junkb = 0
		} else {
			return argv[ac];
		}
	}
*/
	
	return 0;
}

/* main
 *
 *	do main-type-stuff
 */
int main( int argc, char ** argv )
{
	/* check args */
	if( handleOptions( argc, argv )) {
		return -1;
	}

	/* initialize the screen */
	initScreen();

	/* other init will go here */

	doCursesInterface();

	return 0;
}

