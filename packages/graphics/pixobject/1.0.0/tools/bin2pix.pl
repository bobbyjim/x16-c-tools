#!/usr/bin/env perl
use strict;
#
#  Convert CX16 sprites to pixobjects
#  USAGE: perl bin2pix.pl <4bpp | 8bpp> <width> <sprite file>
#
=pod
  The VERA sprite format has two varieties:

  4 bits per pixel
  8 bits per pixel

  This script is not smart enough to know the color depth;
  therefore, the command line has to specify "4bpp" or "8bpp".

  Similarly, the script is not smart enough to know the image width.

  The output is a pixobject with this format:

=NAME 4bpp|8bpp WIDTHxHEIGHT

  "NAME" is the filename minus the extension.
  WIDTH and HEIGHT are in pixels.

  What follows is the lines of data.  These values are supported:

    0         1111111111222222222233  
    01234567890123456789012345678901  
     !"#$%&'()*+,-./0123456789:;<=>?  

    0           1111111111  2222222222  3333333333  4444444444  5555555555  6666666666  7777777777  8888888888  9999999999  0
    0123456789  0123456789  0123456789  0123456789  0123456789  0123456789  0123456789  0123456789  0123456789  0123456789  0123456789
    @ABCDEFGHI  JKLMNOPQRS  TUVWXYZ[\]  ^_`abcdefg  hijklmnopq  rstuvwxyz{  |}~¡¢£¤¥¦§  ¨©ª«¬®¯°±²  ³´µ¶·¸¹º»¼  ½¾¿ÀÁÂÃÄÅÆ  ÇÈÉÊËÌÍÎÏÐ
=cut
my $bpp    = shift || die "USAGE: $0 <4bpp | 8bpp> <width> <sprite file>\n";
my $width  = shift || die "USAGE: $0 $bpp <width> <sprite file>\n";
my $infile = shift || die "USAGE: $0 $bpp $width <sprite file>\n";

my @primaryMap   = split '', ' !"#$%&\'()*+,-./0123456789:;<=>?';
my @secondaryMap = split '', '@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~¡¢£¤¥¦§¨©ª«¬®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐ'; 

my ($name) = $infile =~ /^(\w+)\./;
my $line = '';
my @data = ();

open my $in, '<:raw', $infile || die "ERROR: cannot open sprite file [$infile]\n";
read $in, my $nullbytes, 2;

while (read($in, my $c, 1) != 0)
{
    my $v = unpack 'C', $c;
    if ($bpp eq '4bpp')  # two chars per byte
    {
       $line .= decodeByte($v >> 4);
       $line .= decodeByte($v & 0x0f);
    }
    else                 # one byte per byte
    {
       $line .= decodeByte($v); 
    }

    if (length($line) == $width)
    {
        push @data, $line;
        $line = '';
    }
}
close $in;

my $height = scalar @data;
my $dimensions = $width . 'x' . $height;
my $data = join "\n", @data;

print<<EOPIXOBJ;
=$name $bpp $dimensions
$data
EOPIXOBJ

sub decodeByte
{
   my $c = shift;

   return '.' if $c == 0;
   return '*' if $c == 1;
   return chr($c+48) if $c < 10;  # '0' .. '9'
   return chr($c+55) if $c < 36;  # 'A' .. 'Z'
   return chr($c+61) if $c < 62;  # 'a' .. 'z'
   return '.'; # everything else is a zero
}


sub decodeByteNew
{
    my $val = shift;
    #
    #  Here's how we're going to do it.
    #  Everything from 32 to 255 is on a 7-shade scale.
    #  We compress that to two shades and we have our mapping.
    #
    return $primaryMap[$val] if $val < 32;

    #  It's on a shade scale.
    my $divisor = 224 / (scalar @secondaryMap);

    $val -= 32;                   # translate to zero
    $val = int($val / $divisor);  # find our index
    return $secondaryMap[$val];
}
