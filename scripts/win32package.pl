#!/usr/bin/perl
use strict;
use Getopt::Long;

my $testVersion = 0;
my $cvsUpload = 0;
my $pbpEditor = 0;
my $mapedit = 0;

GetOptions ("test" => \$testVersion,
            "cvs" => \$cvsUpload,
            "pbpeditor" => \$pbpEditor,
            "mapeditor" => \$mapedit );  


my $version = `perl getversion ../source/strtmesg.cpp`;
chomp $version;


my $exepath = "../source/win32/msvc/Release/bin/";
my $zipname = "asc-$version.zip";
my @files = ("asc2.exe" ); 
push( @files,  "mapeditor2.exe") if ( $mapedit );
my @debugfiles = ("asc.pdb", "mapeditor.pdb");

print $version . "\n";

open (P, "passwd.txt") || die "could not open password file";
my $password = <P>;
chomp $password;
close P;


chdir($exepath) || die "could not change directory to $exepath";

my @zipfiles;
if ( $testVersion ) {
	system ( "cp asc2.exe asc2-test.exe");
	die "could not rename ASC2.exe" if $?;
	
	push( @zipfiles, "asc2-test.exe" );
	push( @zipfiles,  "mapeditor2.exe") if ( $mapedit );
} else {
   @zipfiles = @files;
}

system("zip -X -D $zipname " . join (" ", @zipfiles) );
die "error zipping file" if $?;

my $archivedir = "archive/" . $version;

mkdir $archivedir if ( ! -e $archivedir ) ;

system("cp " . join (" ", @files, @debugfiles) . " $archivedir");
die "error copying file" if $?;

system("net use z:") unless -e "z:";

sub randomString {
   my $s = "";
   for ( my $i = 0;  $i < 10; $i++ ) {
      my $j = rand 36;
      if ( $j < 26 ) {
         if ( rand(2) < 1 ) {
            $s .= chr($j + 97);
         } else {
            $s .= chr($j + 65);
         }
      } else {
         $s .= chr($j + 48-26);
      }
   }
   return $s;
}


system("cmd /c copy $zipname z:");
die "error uploading file" if $?;

if ( $cvsUpload ) {
    system("Upload.cmd  $version");
}

if ( $pbpEditor ) {
    my $id = randomString();
    my $pbpzip = "pbpeditor-$id.zip";
    unlink($pbpzip) if -e $pbpzip;
    system("zip $pbpzip pbpeditor2.exe");
    system("cmd /c copy $zipname z:");
    die "error uploading file $pbpzip " if $?;
 
}