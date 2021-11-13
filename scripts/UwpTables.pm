package UwpTables;

# ----------------------------------------------------------------------
#
#  Starport + Base Codes index
#
#  In case you ever want to compress Starport + Base Codes
#
# ----------------------------------------------------------------------
my %spb =
(
   'A'     => 0, 'AN'    => 1, 'AS'    => 2, 'ANS'   => 3,
   'AD'    => 4, 'ADS'   => 5, 'AW'    => 6, 'ANW'   => 7,
   'B'     => 8, 'BN'    => 9, 'BS'    => 10, 'BNS'  => 11,
   'BD'    => 12, 'BDS'  => 13, 'BW'   => 14, 'BNW'  => 15,
   'C'     => 16, 'CS'   => 17, 'D'    => 18, 'DS'   => 19,
   'E'     => 20, 'X'    => 21
);


# ----------------------------------------------------------------------
#
#  Trade System index
#
#  WHY THIS IS IMPORTANT:
#
#  This catalog is a mapping between ALL GENERATABLE COMBINATIONS
#  of trade codes WHICH ARE USED BY THE TRADE SYSTEM.
#
#  This therefore has value BEYOND simply cataloging trade remarks
#  that apply to a world.
#
# ----------------------------------------------------------------------
my @codes = (' ',
                'Ag',
                'Ag Ni',
                'Ag Ni Ri',
                'Ag Ri',
                'As Ba Va',
                'As Hi In Va',
                'As Lo Va',
                'As Ni Va',
                'As Va',
                'Ba',
                'Ba De Po',
                'Ba Fl',
                'Ba Po',
                'Ba Va' ,
                'De Hi In Po',
                'De Hi Po'  ,
                'De Lo Po' ,
                'De Ni'   ,
                'De Ni Po'   ,
                'De Ni Ri',
                'De Po'     ,
                'De Ri',
                'Fl'          ,
                'Fl Hi In'   ,
                'Fl Lo'     ,
                'Fl Ni'    ,
                'Hi'      ,
                'Hi In'     ,
                'Hi In Po' ,
                'Hi In Va' ,
                'Hi Po'   ,
                'Lo'      ,
                'Lo Po'  ,
                'Lo Va' ,
                'Ni'   ,
                'Ni Po'      ,
                'Ni Ri'     ,
                'Ni Va'    ,
                'Po'      ,
                'Ri'     ,
                'Va'
);
my $i = 0;
my %codes = ();
for (@codes)
{
  $codes{ $_ } = sprintf "%-2s", $i;
  $i++;
}

sub getTCIndexForCodeString
{
   my $codeString = shift;
   return $codes{ $codeString };
}

1;

