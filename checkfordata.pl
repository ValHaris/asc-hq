#! /usr/bin/perl

foreach  (@ARGV) {
   if ( ! -e $_  ) {
      $a .= "$_ ";
   }
}
if ( $a ne "" ) {
   print $a;
   exit 1;
}
