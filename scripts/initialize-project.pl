#
#	initialize-project.pl
#	2025 jan 06
#	rje
#
#	INSTRUCTIONS: Place this in the root of your project directory.
#
#	Minimal setup for a cc65 project:
#	* creates /src
#   * creates /src/.obj
#   * creates BOOT.BAS (which loads MAIN)
#	* creates Makefile (which builds MAIN)
#	* creates run (which calls ../../x16 emulator)
#	* creates graphics.h and graphics.c wrapper files.
#	* creates main.c with a stub init() and a main loop.
#
use strict;

die "Project already created\n" if -e 'src/main.c' 
								|| -e 'src/BOOT.BAS'
             				    || -e 'src/Makefile' 
								|| -e 'src/run'
								|| -e 'src/graphics.c'
								|| -e 'src/graphics.h';

mkdir 'src' or die "Cannot create \/src\n";
chdir 'src';
mkdir '.obj';

createBootfile();
createRunscript();
createMakefile();
createMain();
createGraphics_H();
createGraphics_C();

sub createBootfile {
	open my $fh, '>', 'BOOT.BAS' or die "Cannot create BOOT.BAS: $!";
	print $fh <<'ENDBOOT';
10 COLOR 2,0
20 ? CHR$($93)
30 ? "SPLASH SCREEN"
100 COLOR 8
110 FOR X = 1 TO 80:L$=L$+CHR$(164):NEXT
120 ?:?:?:?:? L$;
130 ?:?:? SPC(27);"PRESS <RETURN> TO BEGIN"
140 GET A$:IF A$="" GOTO 140
150 A = RND(-TI)
200 REM LOAD RESOURCES HERE
210 REM ...
220 REM ...
998 REM NOW LOAD MAIN PROGRAM
999 LOAD "MAIN",8,1
ENDBOOT
   close $fh;
   print " - Created BOOT.BAS\n";
}

sub createRunscript {
	open my $fh, '>', 'run' or die "Cannot create run script: $!";
	print $fh <<'ENDRUN';
../../x16 -bas BOOT.BAS -run
ENDRUN
	close $fh;
	chmod 0755, 'run';
	print " - Created run script and made it executable\n";
}

sub createMakefile {
	open my $fh, '>', 'Makefile' or die "Cannot createMakefile: $!";
	print $fh <<'ENDMAKEFILE';
SOURCES = main.c graphics.c 

PROGRAM = MAIN

CC65_TARGET = cx16

CC	= cl65 
CFLAGS 	= --cpu 65c02 -t $(CC65_TARGET) --create-dep $(<:.c=.d) -Ors 
LDFLAGS	= -t $(CC65_TARGET) -m $(PROGRAM).map
OBJDIR  = .obj

#############################################
OBJECTS = $(SOURCES:%.c=$(OBJDIR)/%.o)

.SUFFIXES: 
all: $(PROGRAM)

ifneq (($MAKECMDGOALS),clean)
-include $(SOURCES:.c=.d)
endif

clean:
	$(RM) $(OBJECTS) $(SOURCES:.c=.d) $(PROGRAM) $(PROGRAM).map

.PHONY: all clean

$(PROGRAM): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJECTS): $(OBJDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<
ENDMAKEFILE
	close $fh;
	print " - Created Makefile with a target of MAIN\n";
}

sub createMain {
	open my $fh, '>', 'main.c' or die "Cannot create main.c: $!";
	print $fh <<'ENDMAIN';
/*


*/
#include <6502.h>
#include <stdlib.h>

#include "graphics.h"

void setPETFont()
{
   // set PET font
   struct regs fontregs;
   fontregs.a = 4; // PET-like
   fontregs.pc = 0xff62;
   _sys(&fontregs);
}

void init() {
	// initialization code goes here
	setPETFont();
	_randomize();
}

void main() {
	init();
	graphics_sayhello();
	for(;;) {
		// main loop code should go here
	}
}
ENDMAIN
	close $fh;
	print " - Created main.c with stubbed out init() and run loop.\n";
}

sub createGraphics_H {
	open my $fh, '>', 'graphics.h' or die "Cannot create graphics.h: $!";
	print $fh <<'ENDGRAPHICS_H';
#ifndef GRAPHICS_H
#define GRAPHICS_H

void graphics_sayhello();

#endif
ENDGRAPHICS_H
	close $fh;
	print " - Created graphics.h\n";
}

sub createGraphics_C {
	open my $fh, '>', 'graphics.c' or die "Cannot create grahphics.c: $!";
	print $fh <<'ENDGRAPHICS_C';
/*

	This file isolates hardware-specific graphics code, in order to allow
	runtime debugging on a non-CX16 platform.

*/
#include "graphics.h"
#ifdef __CX16__
#include <conio.h>
#else
#include <stdio.h>
#endif

void graphics_sayhello() {
#ifdef __CX16__
	cprintf("hello cx16\r\n");
#else
	printf("hello unix\n");
#endif
}

ENDGRAPHICS_C
	close $fh;
	print " - Created graphics.c\n";
}
