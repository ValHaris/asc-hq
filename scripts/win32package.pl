#!/usr/bin/perl
use strict;

my $version = `perl getversion ../source/strtmesg.cpp`;
chomp $version;


my $exepath = "../source/win32/msvc/Release/bin/";
my $zipname = "asc-$version.zip";
my @files = ("asc2.exe", "mapeditor2.exe");
my @debugfiles = ("asc.pdb", "mapeditor.pdb");

my $ftpclient = "C:/Programme/NcFTP/ncftpput.exe";


print $version . "\n";

open (P, "passwd.txt") || die "could not open password file";
my $password = <P>;
chomp $password;
close P;


chdir($exepath) || die "could not change directory to $exepath";

system("zip -X -D $zipname " . join (" ", @files) );
die "error zipping file" if $?;

my $archivedir = "archive/" . $version;

mkdir $archivedir if ( ! -e $archivedir ) ;

system("cp " . join (" ", @files, @debugfiles) . " $archivedir");
die "error copying file" if $?;




system("$ftpclient -u ftp60885 -p $password -v www.asc-hq.de /www.asc-hq.de $zipname");
die "error uploading file" if $?;
