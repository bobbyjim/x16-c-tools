#
#
#	add-code.pl
#	2025 jan 07
#	rje
#
#	INSTRUCTIONS: Place this in the root of your project directory.
#
#	Adds support code to a project. Really just a glorified copy:
#   * asks if you want to add sprite code
#   * asks if you want to add PSG code
#   * asks if you want to add timer code
#   * asks if you want to add utext code.
#
#   Alternately, if you pass in a string parameter, it will switch to auto-install mode:
#   * s for sprite code
#   * p for PSG code
#   * t for timer code
#   * u for utext code
#  
#	You'll have to update your Makefile accordingly.
#	
#	* This code assumes you're in your project.
#	* This code assumes the x16-c-tools project is in the same git directory.
#
use strict;
use File::Copy;

die "/src does not exist: $!" unless -d "./src";
die "Cannot find x16-c-tools project: $!" unless -d "../x16-c-tools";

my $sprite = 0;
my $psg    = 0;
my $timer  = 0;
my $utext  = 0;

my $packages = shift || undef;

if ($packages) {
   $sprite = 'no';  
   $sprite = 'yes' if $packages =~ /s/;

   $psg    = 'no';
   $psg    = 'yes' if $packages =~ /p/;

   $timer  = 'no';
   $timer  = 'yes' if $packages =~ /t/;

   $utext  = 'no';
   $utext  = 'yes' if $packages =~ /u/;
}

askFor("sprite", $sprite);
askFor("PSG",    $psg);
askFor("timer",  $timer);
askFor("utext",  $utext);

sub askFor {
	my $base = shift;
	my $install = shift;

	return if $install eq 'no';

    if ($install ne 'yes') {
		print "Do you want <$base> installed?";
		my $response = <STDIN>;
		return unless $response =~ /^y/i;
	}

	copy("../x16-c-tools/$base.c", "./src/$base.c" ) or die "Copy failed: $!";
	copy("../x16-c-tools/$base.h", "./src/$base.h" ) or die "Copy failed: $!";

	print " - Added $base.c and $base.h\n";
}
