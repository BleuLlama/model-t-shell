# modeltsh - The Model T Shell
#
#	Scott Lawrence
#	yorgle@gmail.com
#	2012-October

# 
# Copyright (C) 2011 Scott Lawrence
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject
# to the following conditions:
# 
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
# ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
# CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# 


#######################################################################
# Build options

EXENAME := modeltsh

SRCS := src/main.c \
	src/error.c \
	src/conf.c \
	src/items.c \
	src/myString.c \
	src/utils.c

####################
# general build rules
#
# In general, this should auto-configure for OS X/MinGW, but 
# one thing you will need to do is to install pdcurses on MinGW:
# ref: http://comptb.cects.com/1848-adding-pdcurses-to-mingw
#
# In linux(Ubuntu) you will need to install ncurses:
#   sudo apt-get install ncurses-dev

LDFLAGS += 
DEFS += 
CFLAGS += $(DEFS) -Wall -pedantic -std=gnu99
CFLAGS += -I src -g
CC = gcc


export ARCH := $(shell uname)


ifeq ($(ARCH),MINGW32_NT-6.1)
    # Windows 7
    export ARCH := MINGW
endif
ifeq ($(ARCH),MINGW32_NT-5.2)
    # Server 2003
    export ARCH := MINGW
endif
ifeq ($(ARCH),MINGW32_NT-5.1)
    # Win XP
    export ARCH := MINGW
endif


ifeq ($(ARCH),MINGW)
# MinGW/Windows
TARG := $(EXENAME).exe
LDFLAGS += -static
LIBS += -L /lib -lpdcurses
CFLAGS += -I /include
CFLAGS += -DPDC_DLL_BUILD
endif

ifeq ($(ARCH),Darwin)
# OS X
TARG := $(EXENAME)
LIBS += -lncurses 
endif

ifeq ($(ARCH),Linux)
TARG := $(EXENAME)
LIBS += -lcurses -lm
endif

################################################################################

all: $(EXENAME)
.PHONY: all

################################################################################

OBJS := $(SRCS:%.c=%.o)

$(EXENAME): $(OBJS)
	@echo link $@
	@$(CC) $(CFLAGS) $^ $(LDFLAGS) $(LIBS) -o $@


################################################################################
TARGS += pasimple 

pasimple: build/simple.o build/paHelper.o
	@echo link $@
	@$(CC) $(CFLAGS) $(INCS) $^ $(LDFLAGS) $(LIBS) -o $@


TARGS += sdlsimple 

sdlsimple: build/sdlsimple.o
	@echo link $@
	@$(CC) $(CFLAGS) $^ $(LDFLAGS) $(LIBS) -o $@

################################################################################

%.o: %.c
	@echo build -- $(CC) $<	
	@$(CC) $(CFLAGS)  $(DEFS) $(INCS) -c -o $@ $<

clean: 
	@echo removing all build files.
	@rm -rf $(OBJS) $(EXENAME) $(TARGS)

test: $(EXENAME)
	./$(EXENAME)

testm: $(EXENAME)
	
