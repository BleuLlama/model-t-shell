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
/* Version History */

#define kVersion "0.01"
#define kDate	 "2012-Oct-16"

/*
 * 0.01		2012-October-16
 *		Initial version, with demo items in the list
 *		ncurses based display
 *		Inspired by Jake Bickhard's post on the M100 list
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


#if defined __MINGW32__
#include <windows.h>
#include <Lmcons.h>
#else
#include <pwd.h>	/* for 'whoami' */
#endif

#include <time.h>	/* for localtime, time */
#include <ctype.h>	/* for isprint */

/* ********************************************************************** */
/* the color pairs for the screen */
#define kColorTopBar		(1)
#define kColorBottomBar		(2)
#define kColorText		(3)
#define kColorTextSelected	(4)


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
	init_pair( kColorBottomBar, COLOR_BLACK, COLOR_CYAN );
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


char * whoiam = NULL;

char * whoami( void )
{
#if defined __MINGW32__
	/* for mingw */
	TCHAR name[ UNLEN +1 ];
	DWORD size = UNLEN +1;
	if( GetUserName( (TCHAR *)name, &size ))
	{
		whoiam = strdup( name );
	} else {
		whoiam = strdup( "You" );
	}

#else
	/* for OS X, Linux */
	register struct passwd *pw;
	register uid_t uid;

	if( whoiam != NULL ) return whoiam;

	uid = geteuid ();
	pw = getpwuid (uid);
	if (pw) {
		whoiam = strdup( pw->pw_name );
	} else {
		whoiam = strdup( "You" );
	}
#endif

	return whoiam;
}


WINDOW * win;
int fullRedraw = 1;
int winw, winh;

#define kMaxBuf (256)

char userInput[kMaxBuf];


void showTopBar( int mx, int my )
{
	int x1, x2;
	char tbuf[ kMaxBuf ];
	time_t rawtime;
	struct tm *timeinfo;

	/****** First, do the top bar ******/
	wattron( win, COLOR_PAIR( kColorTopBar ));

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
	snprintf( tbuf, kMaxBuf, "32765 Bytes Free" );
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



#define kMaxItems 256

char * itemList[ kMaxItems ] =
/*{ "BASIC", "TEXT", "TELECOM", "ADDRSS", "SCHEDL" }; */
{ "BASIC", "TEXT", "TELECOM", "ADDRESS", "SCHEDULE", "EXIT", NULL };

int exitNow = 0;
int selection = 0;
int gridtall = 1;
int gridwide = 1;

void copyItemToUserInput( void )
{
	if( itemList[ selection ] ) {
		strcpy( userInput, itemList[ selection ] );
	} else {
		strcpy( userInput, "" );
	}
	strcat( userInput, "_" );
}

void selectDelta( int dx, int dy )
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

	copyItemToUserInput();
}

void selectItem( int idx )
{
	if( idx < 0 ) idx = 0;
	if( idx > 9999 ) idx = 0;

	selection = idx;
	copyItemToUserInput();
}

char * getItemAtIndex( int idx )
{
	/* originally were 11 characters,
		"TESTFILE.BA"
	   we'll stick with 12
		"TESTFILE.BAS"
	   with one space at the start and one at the end yields 14 chars
	*/
	
	if( itemList[idx] != NULL ) {
		return itemList[ idx ];
	} 
	return "--.--";
}

int sameCI( char * a, char * b )
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

void clearInput( void )
{
	userInput[0] = '_';
	userInput[1] = '\0';
}

void executeSelection( void )
{
	/* remove the underscore */
	userInput[ strlen( userInput )-1 ] = '\0';

	/* check keywords */
	if(    sameCI( "EXIT", userInput )
            || sameCI( "EXIT", getItemAtIndex( selection ))) {
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
	gridwide = (int)floor( mx/14 );	/* force for now */
	gridtall = my -2 - (pad *2);

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
			wprintw( win, " %-12s ", getItemAtIndex( cidx ));
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
		selectDelta( 0, 1 );
		break;

	case( KEY_UP ):
		selectDelta( 0, -1 );
		break;

	case( KEY_LEFT ):
		selectDelta( -1, 0 );
		break;

	case( KEY_RIGHT ):
		selectDelta( 1, 0 );
		break;

	case( KEY_HOME ):
		selectItem( 0 );
		break;

	case( KEY_END ):
		selectItem( -1 );
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

