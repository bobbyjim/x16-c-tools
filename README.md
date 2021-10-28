# x16-c-tools
C programming tools for the X16

# SPRITE TOOLS

main.c demonstrates the use of sprite.h and sprite.c 
for defining and placing sprites.

The SpriteDefinition data structure is the payload used to communicate 
sprite attributes to the functions.  There are some things to be aware of:

	uint16_t block; 

This is expected to be the actual mid- and low-memory address in VERA; 
for example, 0x4000 is typically the right place to start loading sprite
data, so .block = 0x4000 would point to the beginning of sprite data.

	uint8_t  mode;
    uint8_t  layer;
	uint8_t  dimensions;      

Familiarize yourself with the #defined constants for these fields.
    
	int      x : 15;
	int      y : 15;

These fields are large on purpose: to allow incremental movement.
Please refer to the two macros for scaling screen locations to these
fields: SPRITE_X_SCALE() and SPRITE_Y_SCALE().

## Creating Sprite files

There are also two tools for creating sprite files in the /sprite folder:

png2sprite.py is copied from x16-demo, and converts a PNG image to a C style array.
c2bin.pl is even more useful: it converts a PNG image to a directly loadable binary.

## Pixobjects

Pixobject files were used in the 1990s in certain applications. They were simple,
ascii-encoded graphic images with a line of metadata.

Pixobjects are limited, but useful in that they are easy to edit.

The /sprite/pixobject folder contains two utilities: pix2bin.pl, which converts
a pixobject to a directly loadable binary sprite file, and bin2pix.pl, which takes
a sprite binary file and creates a pixobject from it.
