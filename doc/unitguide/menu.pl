#!/usr/bin/perl

#***************************************************************************
#*      MenuGen 0.3                                                        *
#*                                                                         *
#*      copyright (C) 2003 by Martin Bickel (mail@martin-bickel.de         *
#***************************************************************************
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License as published by  *
#*   the Free Software Foundation; either version 2 of the License, or     *
#*   (at your option) any later version.                                   *
#*                                                                         *
#***************************************************************************



# Some terminology. Consider this menu
#  
#  groupname  
#    link1
#    link2
#  another group  
#    link3
#
#
#  The gropus open a new page in the menu frame which contain the links of that group 
#  The links themselfs open a page in the main frame showing the content 
#  Both groups and links are entries.



# counts the number of leading dots 
sub countdot {
  my $cnt = 0;
  while ($_[0] =~ /^\./ ) {
     $_[0] =~ s/^\.//;
     ++$cnt;
  }
  return $cnt;  
}


# parses the menu file
# parameters: 
#    0 filename
#    1 initial depth
sub parsefile {
   my ( @data );
   $depth_ = $_[1];
   my $fn = $_[0];
   open $fn, $fn || die "cannot open $_[0] !\n";
   
   while( <$fn> ) {
     if ( !/^;/ ) {
	if ( /^\.*#/ ) {
           $filename = $_;
           $depth_ = countdot($_);
	   $depth_ += $_[1];
           $filename =~ s/\.*#(\w*)/$1/;
           &parsefile ($filename, $depth_);
	} else {
           ($name_,$style_,$link_)=split(/;/,$_);
           if ( /;/ ) {
              $ddepth_ = countdot($name_) + $_[1];
	      $link_ =~ s/&/&amp;/g;
              push (@name,$name_); 
              push (@depth,$ddepth_); 
              push (@style,$style_); 
              push (@link,$link_); 
           }
	}
     }
   }
   close INP;
}


# outputs a single menu line
# parameters:
#        0           : menu item no. 
#        1 (optional): if '1' then group entries don't have links. This used for the noframes menu
sub printLine {
   my $i = $_[0];
   my $suppressMenuLinks = $_[1];
   $returnvalue = "<tr>";
   
   for ( $einr = 0; $einr < $depth[$i]; $einr++ ) {
      $returnvalue .= "<td style=\"width:10px\"></td>";
   }
   $width= $maxdepth - $depth[$i] + 1;
   $returnvalue .= "<td colspan=\"$width\" class=\"menu\"> ";
   
   if ( $link[$i] =~ /^-$/ ) {  
      # this is a group entry
      if ( $suppressMenuLinks == 1 ) {
         $returnvalue .= "<b>$name[$i]</b>"; 
      } else {
         if ( $menuopen==1 ) {
	    $u = $tree[$actdepth-1];
            $returnvalue .= "<b><A HREF=\"menu$u.html\" class=\"navi\">$name[$i] <IMG SRC=\"pfeil_zu.gif\" alt=\"[-]\"></A></b>"; 
	 } else {
            $returnvalue .= "<b><A HREF=\"menu$i.html\" class=\"navi\">$name[$i] <IMG SRC=\"pfeil_auf.gif\" alt=\"[+]\"></A></b>"; 
	 }   
      }
      
   } else {  
      # this is a link entry
      $returnvalue .= "<A HREF=$link[$i] class=\"navi\">$name[$i]</A>";
   }
   $returnvalue .= " </td></tr>\n";
   return $returnvalue;
}


# main program
# parameters:
#     0 (optional)           : prefix directory for menu files
#     1 (even more optionaä) : prefix directory for the noframes menu


$argl = @ARGV;

if ( $argl >= 1 ) {
   $prefix = $ARGV[0];
} else {
   $prefix = "";
}   

if ( $argl >= 2 ) {
   $idxprefix = $ARGV[1];
} else {
   $idxprefix = $prefix;
}   

parsefile("entries.txt", 0);



$length = @name;

$maxdepth = 0;
for ( $counter = 0; $counter < $length; $counter++ ) {
   if ( $depth[$counter] > $maxdepth ) {
      $maxdepth = $depth[$counter];
   }
}      

unshift(@link, "-" );
unshift(@depth, -1 );
unshift(@name, "root" );
unshift(@style, $style[0] );
++$length;

print "Number of entries: $length \n";

$counter = 0;
for ( $counter = 0; $counter < $length; $counter++ ) {

   if ( $link[$counter] =~ /^-$/ || $counter == 0 ) {
      $d = $depth[$counter];
      open DATEI, ">${prefix}menu$counter.html";
      print DATEI <<END_OF_HEADER;

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<HTML>
<HEAD>
<meta http-equiv="content-type" content="text/html; charset=ISO-8859-1">

<TITLE>Menu</TITLE>
<LINK REL="stylesheet" TYPE="text/css" HREF="$style[$counter]">
<link rel="shortcut icon" href="favicon.ico">
</HEAD>
<BODY class="navi">
<table border="0" cellspacing="0" cellpadding="0">
END_OF_HEADER

      $blockopen = 0;  
      
      # tree contains the entry numbers that are opened groups.
      # A tree of (0,1,4,19) means that the entry #1 is the first open group (level 0), 
      #   entry #4 is the open group that opens level 1 
      #   entry #19 is the open group that opens level 2, which is the currently processed entry
      
      @tree = ($counter);
      # We are building the tree in reverses order, so we are first pushing the current entry
      
      $actdepth = $depth[$counter];
      for ( $i = $counter; $i >= 0; $i-- ) {
         # we are going up from the current entry and look for groups that open the levels of depth
	 if ( $depth[$i] < $actdepth ) {
            unshift ( @tree, $i );
            $actdepth = $depth[$i];
	 }
      }
      push ( @tree, $length+10 );

      $treepos = 0;
      $actdepth = -1;  
      for ( $i = 0; $i < $length; ++$i ) {
	 if ( $tree[$treepos] == $i ) {
            $actdepth++;
            $treepos++;
            $menuopen=1;
	 } else {
            $menuopen=0;
            if ( $depth[$i] < $actdepth ) {
               $actdepth = $depth[$i];
            }
	 }

	 if ( $depth[$i] <= $actdepth && $depth[$i] >= 0 ) {
            print DATEI printLine( $i );
	 }

      }
      print DATEI "</table></BODY></HTML>";
      close DATEI;
   }
}



 
open DATEI, ">${idxprefix}noframes.inc";
for ( $i = 0; $i < $length; ++$i ) {
   $_ = printLine( $i, 1 );
   s/target="\w+"//g;
   print DATEI $_;
}
close DATEI;
