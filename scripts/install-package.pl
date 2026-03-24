#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

use Cwd qw(abs_path);
use File::Basename qw(dirname);
use File::Copy qw(copy);
use File::Path qw(make_path remove_tree);
use File::Spec;
use FindBin qw($Bin);
use Getopt::Long qw(GetOptions);
use YAML::Tiny;

my %options = (
	project => '.',
	repo => undef,
	force => 0,
	dry_run => 0,
	include_tests => 0,
	help => 0,
);

GetOptions(
	'project=s' => \$options{project},
	'repo=s' => \$options{repo},
	'force' => \$options{force},
	'dry-run' => \$options{dry_run},
	'include-tests' => \$options{include_tests},
	'help' => \$options{help},
) or die usage();

if ($options{help}) {
	print usage();
	exit 0;
}

my @specs = @ARGV;
die usage() unless @specs;

$options{repo} = abs_path($options{repo} // File::Spec->catdir($Bin, '..'))
	or die "Cannot resolve repository root\n";
$options{project} = abs_path($options{project})
	or die "Cannot resolve project directory '$options{project}'\n";

my $packages_root = File::Spec->catdir($options{repo}, 'packages');
die "Package repository not found at $packages_root\n" unless -d $packages_root;
die "Project directory not found at $options{project}\n" unless -d $options{project};

my $registry = build_registry($packages_root);
my %state = (
	installing => {},
	installed => {},
	order => [],
);

for my $spec (@specs) {
	my $entry = resolve_package($registry, $spec);
	install_package($registry, $entry, \%options, \%state);
}

say '';
if ($options{dry_run}) {
	say 'Dry run complete.';
} else {
	say 'Install complete.';
}

say 'Installed packages:';
say " - $_" for @{$state{order}};

exit 0;

sub usage {
	return <<'END_USAGE';
Usage: install-package.pl [options] package[\@version] ...

Installs one or more packages from this repository into another project's
packages/ directory, preserving the package layout and installing dependencies
first.

Options:
  --project DIR       Target project root. Default: current directory
  --repo DIR          Package repository root. Default: inferred from script
  --force             Reinstall packages even if already present
  --dry-run           Print planned actions without copying files
  --include-tests     Copy each package's test/ directory too
  --help              Show this help text

Examples:
  perl scripts/install-package.pl --project /path/to/game audio.adsr
  perl scripts/install-package.pl --project . graphics.sprite text.utext
  perl scripts/install-package.pl --project . audio.adsr\@1.0.0
END_USAGE
}

sub build_registry {
	my ($root) = @_;
	my %registry;
	my @files = `find "$root" -name package.yaml`;
	chomp @files;

	for my $file (@files) {
		next unless length $file;

		my $yaml = YAML::Tiny->read($file)
			or die "Failed to parse $file: " . YAML::Tiny->errstr . "\n";
		my $data = $yaml->[0];
		die "Invalid package metadata in $file\n"
			unless $data && ref $data eq 'HASH';
		die "Missing name/version in $file\n"
			unless exists $data->{name} && exists $data->{version};

		my $deps = $data->{deps} || [];
		$deps = [$deps] unless ref $deps eq 'ARRAY';

		push @{$registry{$data->{name}}}, {
			name => $data->{name},
			version => $data->{version},
			deps => [@$deps],
			description => $data->{description} || '',
			file => $file,
			dir => dirname($file),
		};
	}

	for my $name (keys %registry) {
		@{$registry{$name}} = sort {
			compare_versions($a->{version}, $b->{version})
		} @{$registry{$name}};
	}

	return \%registry;
}

sub resolve_package {
	my ($registry, $spec) = @_;
	my ($name, $wanted_version) = split /\@/, $spec, 2;
	die "Invalid package spec '$spec'\n" unless defined $name && length $name;

	my $versions = $registry->{$name}
		or die "Unknown package '$name'\n";

	if (defined $wanted_version && length $wanted_version) {
		for my $entry (@$versions) {
			return $entry if $entry->{version} eq $wanted_version;
		}
		die "Package '$name' does not have version '$wanted_version'\n";
	}

	return $versions->[-1];
}

sub install_package {
	my ($registry, $entry, $options, $state) = @_;
	my $package_id = "$entry->{name}\@$entry->{version}";

	return if $state->{installed}{$package_id};
	die "Circular dependency detected while installing $package_id\n"
		if $state->{installing}{$package_id};

	$state->{installing}{$package_id} = 1;

	for my $dep_name (@{$entry->{deps}}) {
		my $dep = resolve_package($registry, $dep_name);
		install_package($registry, $dep, $options, $state);
	}

	copy_package($entry, $options);

	delete $state->{installing}{$package_id};
	$state->{installed}{$package_id} = 1;
	push @{$state->{order}}, $package_id;
}

sub copy_package {
	my ($entry, $options) = @_;
	my $package_path = $entry->{name};
	$package_path =~ s/\./\//g;

	my $destination = File::Spec->catdir(
		$options->{project},
		'packages',
		split(/\//, $package_path),
		$entry->{version},
	);

	if (-e $destination) {
		if ($options->{force}) {
			announce("Replacing $entry->{name}\@$entry->{version} at $destination", $options);
			remove_tree($destination) unless $options->{dry_run};
		} else {
			announce("Skipping $entry->{name}\@$entry->{version}; already present at $destination", $options);
			return;
		}
	}

	announce("Installing $entry->{name}\@$entry->{version} -> $destination", $options);
	make_path($destination) unless $options->{dry_run};

	my @items = qw(package.yaml README.md include src data tools);
	push @items, 'test' if $options->{include_tests};

	for my $item (@items) {
		my $source = File::Spec->catfile($entry->{dir}, $item);
		next unless -e $source;

		my $target = File::Spec->catfile($destination, $item);
		copy_path($source, $target, $options);
	}
}

sub copy_path {
	my ($source, $target, $options) = @_;

	if (-d $source) {
		announce("  mkdir $target", $options) if $options->{dry_run};
		make_path($target) unless $options->{dry_run};

		opendir my $dh, $source or die "Cannot read $source: $!\n";
		for my $child (sort grep { $_ ne '.' && $_ ne '..' } readdir $dh) {
			copy_path(
				File::Spec->catfile($source, $child),
				File::Spec->catfile($target, $child),
				$options,
			);
		}
		closedir $dh;
		return;
	}

	announce("  copy $source -> $target", $options);
	return if $options->{dry_run};

	make_path(dirname($target));
	copy($source, $target) or die "Copy failed: $source -> $target: $!\n";
	my $mode = (stat $source)[2] & 07777;
	chmod $mode, $target;
}

sub announce {
	my ($message, $options) = @_;
	my $prefix = $options->{dry_run} ? '[dry-run] ' : '';
	say $prefix . $message;
}

sub compare_versions {
	my ($left, $right) = @_;
	my @left_parts = split /\./, $left;
	my @right_parts = split /\./, $right;
	my $length = @left_parts > @right_parts ? scalar @left_parts : scalar @right_parts;

	for my $index (0 .. $length - 1) {
		my $left_value = $left_parts[$index] // 0;
		my $right_value = $right_parts[$index] // 0;
		my $cmp = $left_value <=> $right_value;
		return $cmp if $cmp;
	}

	return 0;
}