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

  0-9 colors 0-9
  A-Z colors 10-35
  a-z colors 36-61

=cut
my $bpp    = shift || die "USAGE: $0 <4bpp | 8bpp> <width> <sprite file>\n";
my $width  = shift || die "USAGE: $0 $bpp <width> <sprite file>\n";
my $infile = shift || die "USAGE: $0 $bpp $width <sprite file>\n";

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
    my $val = shift;
    return '.' if $val == 0;
    return '*' if $val == 1;
    return chr(48+$val) if $val < 10; # '0'..'9'
    return chr(55+$val) if $val < 36; # 'A'..'Z'
    return chr(61+$val) if $val < 61; # 'a'..'z'
    return '.'; # everything else is a dot
}
