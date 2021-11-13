

print "    Value  Degree >  Sin  sign\n",
      "    -----  ------ >  ---  ----\n";

open my $fp, '>', 'TRIG';
print $fp pack 'xx';      # initial 2 bytes

for(my $value=0; $value<256+64+1; ++$value)
{
   my $theta    = 1.40625 * $value;         # degrees
   my $radians  = $theta / 57.295779513;
   my $sin      = int(100 * sin($radians));
   my $sign     = 0;

   $sign = 1 if $sin < 0;
   $sin = abs($sin);

   printf("     %3d     %3d     %3d    %-d\n",
	$value,
        $theta,
        $sin,
        $sign
	);

   my $outbyte = $sin;        # abs
   $outbyte |= 128 if $sign;  # flip bit 8 if < 0

   print $fp pack 'C', $outbyte;
}

close $fp;