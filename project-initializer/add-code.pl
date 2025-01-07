#
#
#	add-code.pl
#	2025 jan 07
#	rje
#
#	Adds support code to a project. Really just a glorified copy.
#	You'll have to update your Makefile accordingly.
#	
#	* This code assumes you're in your project.
#	* This code assumes the x16-c-tools project is in the same git directory.
#
use strict;
use File::Copy;

die "/src does not exist: $!" unless -d "./src";
die "Cannot find x16-c-tools project: $!" unless -d "../x16-c-tools";

askFor("sprite");
askFor("PSG");
askFor("timer");
askFor("utext");

sub askFor {
	my $base = shift;

	print "Do you want <$base> installed?";
	my $response = <STDIN>;
	return unless $response =~ /^y/i;

	copy("../x16-c-tools/$base.c", "./src/$base.c" ) or die "Copy failed: $!";
	copy("../x16-c-tools/$base.h", "./src/$base.h" ) or die "Copy failed: $!";
}
