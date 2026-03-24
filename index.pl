#
# index.pl - check the package.yaml files for errors.
#
use strict;
use warnings;
use v5.10;
use YAML::Tiny;

my %registry;

my $root = shift or die "Usage: $0 <directory>\n";
$root =~ s/\/$//; # remove trailing slash

my @files = `find "$root" -name package.yaml`;
chomp @files;

foreach my $file (@files) {
	# Check if file is empty
	if (-z $file) {
		say "ERROR: $file is empty";
		next;
	}

	my $yaml = YAML::Tiny->read($file);
	# Check if YAML was parsed successfully
	unless (defined $yaml) {
		say "ERROR: Failed to parse $file: " . YAML::Tiny->errstr;
		next;
	}

	# Check top-level structure
	my $data = $yaml->[0];
	unless ($data && ref $data eq 'HASH') {
		say "ERROR: $file does not contain a valid YAML object";
		next;
	}

	# Check required fields
	unless (exists $data->{name} && exists $data->{version}) {
		say "ERROR: $file is missing name or version fields";
		next;
	}

	my $name = lc $data->{name};
	# Check name is lowercase
	if ($name ne $data->{name}) {
		say "ERROR: $file name must be lowercase";
		next;
	}
	
	# Sanity check version format
	unless ($data->{version} =~ /^\d+\.\d+\.\d+$/) {
		say "ERROR: $file has invalid version format";
		next;
	}

	# Check path matches name and version
	my $path = $name;
	$path =~ s/\./\//g;
	$path = $root . '/' . $path . '/' . $data->{version} . '/package.yaml';
	unless ($file eq $path) {
		say "ERROR: $file path does not match name and version:\n       $path";
		next;
	}

	my $deps = $data->{deps} || [];
	$deps = [$deps] unless ref $deps eq 'ARRAY';

	# Check for duplicate package name
	if (exists $registry{$name}) {
		say "ERROR: Duplicate package name $name ($file)";
		next;
	}

	$registry{$name} = {
		version => $data->{version},
		file => $file,
		deps => $deps,
		description => $data->{description} || '',
	};
}

my @errors = ();

say "Package Name            Ver. Dependencies         Description";
say "--------------------------- -------------------- ------------------------------------------------------";
foreach my $name (sort keys %registry) {
	my $entry = $registry{$name};

    my ($version_trunc) = $entry->{version} =~ /^(\d+\.\d+)/;
	my $name_version =sprintf("%-21s %5s", $name, $version_trunc);
	my $description_trunc = length($entry->{description} || '') > 50 ? substr($entry->{description}, 0, 47) . '...' : $entry->{description} || '';
	my $deps_str = @{$entry->{deps} || []} ? join(', ', @{$entry->{deps}}) : '-';
	printf "%-20s %-20s %s\n", $name_version, $deps_str, $description_trunc;

	for my $dep (@{$registry{$name}->{deps} || []}) {
		unless (exists $registry{$dep}) {
			push @errors, "ERROR: $name depends on missing $dep";
		}
	}
}

if (@errors) {
	say "\nErrors found:";
	say $_ for @errors;
	exit 1;
} else {
	say "\n";
	say " *******************************";
	say " *                             *";
	say " *   All packages are valid!   *";
	say " *                             *";
	say " *******************************\n\n";

	exit 0;
}
