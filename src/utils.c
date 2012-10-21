/*
	Model T Shell - Utils
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
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


#include <dirent.h>	/* for directory listings */

#include <sys/types.h>	/* for directory scanning */
#include <sys/stat.h>	/* for directory scanning */

#if defined __MINGW32__
#include <windows.h>
#include <Lmcons.h>
#else
#include <pwd.h>	/* for 'whoami' */
#endif

#include <time.h>	/* for localtime, time */
#include <ctype.h>	/* for isprint */

#include "vals.h"
#include "items.h"


static char * whoiam = NULL;

char * utils_whoami( void )
{
#if defined __MINGW32__
	/* for mingw */
	TCHAR name[ UNLEN +1 ];
	DWORD size = UNLEN +1;
	if( GetUserName( (TCHAR *)name, &size ))
	{
		whoiam = strdup( name );
	} else {
		whoiam = strdup( kUnknownPerson );
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
		whoiam = strdup( kUnknownPerson );
	}
#endif
	return whoiam;
}


int utils_sameCI( char * a, char * b )
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


char * cwd = NULL;

void utils_getcwd( char * buf, int bufsize )
{
#if defined __MINGW32__
	_getcwd( buf, bufsize );
#else
	getcwd( buf, bufsize );
#endif
}

void utils_changeDirectory( char * diff )
{
	char buf[ 256 ];
	int idx = 0;
	int lastslash = 0;

	if( diff == NULL ) { 
		char pth[FILENAME_MAX];
		/* special case, get the current working directory */
		utils_getcwd( pth, sizeof( pth ));
		cwd = strdup( pth );
		return;
	}

	if( !strcmp( diff, kNameParent )) {
		/* special case */
		/* cwd already exists, so we can just work on it */

		for( idx=0 ; idx< strlen( cwd ) ; idx++ ) {
			if( cwd[idx] == '/' ) lastslash = idx;
		}
		if( lastslash > 0 ) cwd[lastslash] = '\0';
		if( lastslash == 0 )  cwd[lastslash+1] = '\0';
		return;
	}

	if( cwd != NULL ) {
		snprintf( buf, 256, "%s/%s", cwd, diff );
		free( cwd );
		cwd = strdup( buf );
	} else {
		cwd = strdup( diff );
	}
}
