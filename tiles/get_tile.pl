#!/opt/local/bin/perl -w

use strict;
use GD;

my $ts = new GD::Image("tilesheet.png") or die "can't open tilesheet";

my $im = new GD::Image(20,20);

sub import_tile
{
	my ($dst,$src,$tileidx) = @_;
	my $srcx = ($tileidx&31)*20;
	my $srcy = ($tileidx>>5)*20;
	$dst->copy($src, 0, 0, $srcx, $srcy, 20, 20);
}

import_tile($im, $ts, $ARGV[0]);

open my $png, '>', 'in.png';
print $png $im->png;
close $png;

