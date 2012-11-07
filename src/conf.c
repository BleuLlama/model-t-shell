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

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>	/* mkdir */
#include <string.h>
#include <ctype.h> 	/* isspace */
#include "vals.h"
#include "error.h"
#include "utils.h"
#include "conf.h"
#include "txt_conf.h"

/*
 * #if defined __APPLE__ || defined __linux__ || defined __MINGW32__
 * #endif
*/

/* ********************************************************************** */

char confFile[kMaxBuf];

static confItem cis[100] = {
	{ "" }
};


void conf_Init( void )
{
	char * homedir = NULL;
	FILE * fp;
	int newConf = 0;
	confItem *ci;

	/* first, let's build up the confFile string */
	confFile[0] = '\0';

	homedir = getenv ("HOME");
	if( homedir ) {
		snprintf( confFile, kMaxBuf, "%s/.modeltsh", homedir );
		(void)mkdir( confFile, 000755 );
		strncat( confFile, "/conf.txt", kMaxBuf );

		fp = fopen( confFile, "r" );
		if( !fp ) {
			newConf = 1;
/*
			errCode = kErrorNoConf;
			return;
*/
		}
		else fclose( fp );
	}

	if( newConf ) {
		/* synthesize the file */
		FILE * of = fopen( confFile, "w" );
		fwrite( conf_txt, strlen( conf_txt ), 1, of );
		fclose( of );

		/* load in the default values */
		conf_Load();

		/* set up some live defaults */
		conf_Set( "HomeDir", homedir );
		conf_Set( "ConfFile", confFile );
		ci = conf_Find( "StartDir" );
		if( ci ) {
			utils_getcwd( ci->value, kMaxBuf );
		}

	}
	conf_Load();
}

/* InGroup
	if the key was the group, return the KEY 
	keys are in the format:  GROUP.KEY = VALUE
*/
char * conf_InGroup( char * group, char * key )
{
	int glen; 
	if( !group || !key ) return NULL;

	glen = strlen( group );

	if( !strncmp( group, key, glen )) {
		if( *(key+glen) == '.' ) {
			return key+glen+1;
		}
	}

	return NULL;
}

char * conf_TrimString( char * in )
{
	char * ret = in;
	int idx;
	if( !in ) return ret;

	/* trim the front */
	for( idx=0 ; isspace( in[idx] ) ; idx++ );
	ret = &in[idx];

	/* trim the back */
	for( idx=strlen(ret)-1 ; isspace( ret[idx] )&& idx >=0 ; idx-- );
	if( idx >= 0 ) ret[idx+1] = '\0';

	return ret;
}

void conf_Load( void )
{
	int valid;
	char line[kMaxBuf];
	char * key;
	char * value;
	int idx;

	FILE * fp = fopen( confFile, "r" );
	if( !fp ) return;

	while( fgets( line, kMaxBuf, fp )) {
		/* eliminate any comments */
		for( idx=0 ; line[idx] != '\0' ; idx++ ) {
			if( line[idx] == '#' ) line[idx] = '\0';
		}
		
		/* set the key pointer */
		key = &line[0];

		/* determine the location of the =, set value pointer */
		valid = 0;
		for( idx=0 ; line[idx] != '\0' ; idx++ ) {
			if( line[idx] == '=' ) {
				value = &line[idx+1];
				line[idx] = '\0';
				valid = 1;
			}
		}

		/* if it was a valid key-value pair, trim and set */
		if( valid ) {
			key = conf_TrimString( key );
			value = conf_TrimString( value );
			conf_Set( key, value );
/* debug stuff
			printf( "Set key \"%s\" to value \"%s\"\n", key, value );

			do {
				char *k = conf_InGroup( "Verbs", key );
				if( k ) {
					printf( "%s -> %s\n", key, k );
				}
			} while( 0 );
*/
		}
	}

	fclose( fp );
}


confItem * conf_Find( char * key )
{
	int ci = 0;
	while( cis[ci].key[0] != '\0' )
	{
		if( utils_sameCI( key, cis[ci].key )) return( &cis[ci] );
		if( !strcmp( key, cis[ci].key )) return( &cis[ci] );
		ci++;
	}

	return NULL;
}

void conf_Save( void )
{
	FILE * fp;
	int ci = 0;

	fp = fopen( confFile, "w" );
	if( !fp ) return;

	while( cis[ci].key[0] != '\0' )
	{
		fprintf( fp, "%s=%s\n", cis[ci].key, cis[ci].value );
		ci++;
	}

	fclose( fp );
}



char * conf_Get( char * key )
{
	confItem * c = conf_Find( key );
	if( !c ) return NULL;
	return c->value;
}

int conf_GetInt( char * key )
{
	char * v = conf_Get( key );
	if( !v ) return 0;
	return( atoi( v ));
}


void conf_Set( char * key, char * val )
{
	confItem * c = conf_Find( key );
	if( !c ) return;
	strncpy( c->value, val, kMaxBuf);
	conf_Save();
}

void conf_SetInt( char * key, int val )
{
	confItem * c = conf_Find( key );
	if( !c ) return;
	snprintf( c->value, val, "%d", val );
	conf_Save();
}

