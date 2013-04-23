model-t-shell
=============

![Screenshot](http://umlautllama.com/rand/model-t-shell-004.png "Screenshot")

This is a shell that simulates an interface heavily based on/inspired 
by the Tandy 100/102/200 "Model T" portable computer.

I have no real goals for this, other than to see if I could make it, 
and what kinds of things could be done to modernize the M100 experience.

# Host Platforms

This should build on OS X, linux and eventually for MinGW, by simply typing 
"make".  The prereqs for this are GCC, GnuMake, NCurses/PDCurses.

# Implemented functionality

When you run model-t-shell, it should bring up an M100-like interface, showing
the date and time in the top left, your username in the top right and an 
input line at the bottom.

Arrow keys will move around the program and file list.

Currently, nothing really does very much of anything. I'm in the process of 
making it more flexible/configurable.

Previously, only "TEXT" and "EXIT" are functional.  "TEXT" uses pico 
as the default editor.  This was hardcoded. 

Selecting a file with an extension: .txt, .doc, .do, .bas, .ba, .c,
.h, and a few others will cause that file to open up in Pico as well.

# Future

Perhaps break out the everything-list into commands/commandables
and data files. At that point, it gets close to "Directory Opus"
in style.  Separating these two out may make sense for modern 
computing.  TAB could be used to switch between them?  I'm not 
sure...

We'll see...

