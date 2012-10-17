
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

SRCS := src/main.c

####################
# general build rules
#
# In general, this should auto-configure for OS X/MinGW, but 
# one thing you will need to do is to install pdcurses on MinGW:
# ref: http://comptb.cects.com/1848-adding-pdcurses-to-mingw



LDFLAGS += 
DEFS += 
CFLAGS += $(DEFS) -Wall -pedantic
INCS += 
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
INCS += -I /include
CFLAGS += -DPDC_DLL_BUILD
endif

ifeq ($(ARCH),Darwin)
# OS X
CFLAGS += -mmacosx-version-min=10.8
TARG := $(EXENAME)
LIBS += -lncurses 
else
endif


################################################################################

all: $(TARG)
.PHONY: all

################################################################################

OBJS := $(SRCS:%.c=%.o)

$(TARG): $(OBJS)
	@echo link $@
	@$(CC) $(CFLAGS) $^ $(LDFLAGS) $(LIBS) -o $@


################################################################################
TARGS += pasimple 

pasimple: build/simple.o build/paHelper.o
	@echo link $@
	@$(CC) $(CFLAGS) $^ $(LDFLAGS) $(LIBS) -o $@


TARGS += sdlsimple 

sdlsimple: build/sdlsimple.o
	@echo link $@
	@$(CC) $(CFLAGS) $^ $(LDFLAGS) $(LIBS) -o $@


################################################################################

build/%.o: %.c
	@echo build -- $(CC) $<	
	@$(CC) $(CFLAGS) $(DEFS) $(INCS) -c -o $@ $<

$(OBJS): build

build:
	@echo Making Build Directiory
	@mkdir build/

clean: 
	@echo removing all build files.
	@rm -rf build $(OBJS) $(TARG) $(TARGS)

test: $(TARG)
	./$(TARG)

testm: $(TARG)
	