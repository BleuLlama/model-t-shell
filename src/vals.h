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
/* the color pairs for the screen */
#define kColorTopBar		(1)
#define kColorBottomBar		(2)
#define kColorText		(3)
#define kColorTextSelected	(4)
#define kColorTextError		(5)
#define kColorTextWarning	(6)
#define kColorTextPath		(7)

/* ********************************************************************** */

/* user input buffer size */
#define kMaxBuf (256)

#define kUnknownPerson "you"	/* couldn't figure out your name, use this */

#define kSkipDotFiles	1	/* show dot files in the file listing? */
