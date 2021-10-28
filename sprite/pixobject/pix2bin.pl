use strict;
#
#  Convert pixobjects to CX16 sprites
#  USAGE: perl pix2bin.pl <input file>
#
#  The pixobject file format is as follows:
#
#  A header line, containing the object's name, its bits per pixel, and its dimensions.
#  Data lines following.  Hexadecimal digits are interpreted properly.  
#
#  Values from 0 up to 61 are handled as 0-9, A-Z, and a-z.
#  The full 8 bit range of values are not handled correctly.
#
#  Example:
=pod
=house1 4bpp 16x8


     ******    
   **********        
 **************
   **********
   *** ** ***   
   ****** ***   
=cut
#
#
#

my $infile = shift || die "SYNOPSIS: $0 pixobj\n";

open my $in, '<', $infile;
my $header = <$in>;
my @data = <$in>;
close $in;

chomp @data;
my ($name, $bpp, $width, $height) = $header =~ /=(\w+) (4bpp|8bpp)\s+(\d\d?)x(\d\d?)/;
die "Cannot read header\n" unless $bpp && $width && $height;

my $dimensions = $3 . 'x' . $4;

my $outfile = sprintf("%s_%s_%s.bin", $bpp, $name, $dimensions);
print "===> $outfile <=== $bpp, $width x $height\n";

open my $out, '>', $outfile;
print $out pack 'xx';             # initial two null bytes

foreach my $line (@data)
{
   my @bits = split '', $line;
   my @line = ();

   my $odd   = 1;    # This is how we'll handle 4 bits per pixel.
   my $value = 0;

   for (0..$width-1) 
   {
       my $charval = getCharval($bits[$_]);

       if ($bpp =~ /4bpp/i && $odd)
       {
           $value = $charval << 4; # initial value = upper nybble
       }
       else                        # 8 bits OR we're even
       {
           $value += $charval;
           print $out pack 'C', $value;
           $value = 0;             # and reset
       }
       $odd = 1 - $odd;            # toggle for nybbles
   }
}
close $out;

sub getCharval
{
    my $c = shift;
    return ord($c)-48 if $c =~ /\d/;        # 0-9
    return ord($c)-55 if $c =~ /[A-Z]/;     # 10-35
    return ord($c)-61 if $c =~ /[a-z]/;     # 35-61 
    return 1 if $c eq '*';
    return 0; # everything else is a zero
}
