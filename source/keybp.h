//     $Id: keybp.h,v 1.8 2000-01-01 19:04:18 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.7  1999/12/30 21:04:46  mbickel
//      Restored DOS compatibility again.
//
//     Revision 1.6  1999/12/30 20:30:35  mbickel
//      Improved Linux port again.
//
//     Revision 1.5  1999/12/28 22:04:26  mbickel
//      Had to make some changes again to compile it for DOS...
//
//     Revision 1.4  1999/12/28 21:03:00  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.3  1999/12/27 12:51:38  mbickel
//      Cleanup for Linux port
//
//     Revision 1.2  1999/11/16 03:41:53  tmwilson
//     	Added CVS keywords to most of the files.
//     	Started porting the code to Linux (ifdef'ing the DOS specific stuff)
//     	Wrote replacement routines for kbhit/getch for Linux
//     	Cleaned up parts of the code that gcc barfed on (char vs unsigned char)
//     	Added autoconf/automake capabilities
//     	Added files used by 'automake --gnu'
//
//
/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING. If not, write to the 
    Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
    Boston, MA  02111-1307  USA
*/

#ifdef _DOS_

#ifndef NEWKEYB 
   #define ct_enterk  ct_enter
   #define ct_altp     256 
   #define ct_space   32 
   #define ct_bspace 8 
   #define ct_a  'a' 
   #define ct_b  'b' 
   #define ct_c  'c' 
   #define ct_d  'd' 
   #define ct_e  'e' 
   #define ct_f  'f' 
   #define ct_g  'g' 
   #define ct_h  'h' 
   #define ct_i  'i' 
   #define ct_j  'j' 
   #define ct_k  'k' 
   #define ct_l  'l' 
   #define ct_m  'm' 
   #define ct_n  'n' 
   #define ct_o  'o' 
   #define ct_p  'p' 
   #define ct_q  'q' 
   #define ct_r  'r' 
   #define ct_s  's' 
   #define ct_t  't' 
   #define ct_u  'u' 
   #define ct_v  'v' 
   #define ct_w  'w' 
   #define ct_x  'x' 
   #define ct_y  'y' 
   #define ct_z  'z' 
   #define ct_oe 'î' 
   #define ct_ae 'Ñ' 
   #define ct_ue 'Å' 
   #define ct_0  '0' 
   #define ct_1  '1' 
   #define ct_2  '2' 
   #define ct_3  '3' 
   #define ct_4  '4' 
   #define ct_5  '5' 
   #define ct_6  '6' 
   #define ct_7  '7' 
   #define ct_8  '8' 
   #define ct_9  '9' 

   #define ct_tab    9 
   #define ct_esc    27 
   #define ct_invvalue   -1
   #define ct_invvalue2  -2
   #define ct_left   75+256 
   #define ct_right  77+256 
   #define ct_up     72+256 
   #define ct_down   80+256 
   #define ct_enter  13 
   #define ct_entf   83+256 
   #define ct_einf   82+256 
   #define ct_pos1   71+256 
   #define ct_ende   79+256 
   #define ct_pdown  81+256 
   #define ct_pup    73+256 
   #define ct_f1     59+256 
   #define ct_f2     60+256 
   #define ct_f3     61+256 
   #define ct_f4     62+256 
   #define ct_f5     63+256 
   #define ct_f6     64+256 
   #define ct_f7     65+256 
   #define ct_f8     66+256 
   #define ct_f9     67+256 
   #define ct_f10    68+256 
   #define ct_f11    254 
   #define ct_f12    253 
   #define ct_minus  '-' 
   #define ct_plus   '+' 
   #define ct_point  '.' 
   #define ct_komma  ',' 
   #define ct_ap  0 
   #define ct_8k  ct_up 
   #define ct_4k  ct_left 
   #define ct_6k  ct_right 
   #define ct_2k  ct_down 
   #define ct_1k  ct_ende 
   #define ct_3k  ct_pdown 
   #define ct_7k  ct_pos1 
   #define ct_9k  ct_pup 
   #define ct_5k   513 
   #define ct_minusk  ct_minus
   #define ct_plusk  ct_plus
   #define ct_lalt  0 
   #define ct_ralt  0 
   #define ct_stp  -96
   #define ct_shp  -32
   #define ct_shift_tab 271
#endif

#ifdef NEWKEYB
   #define ct_invvalue   -1
   #define ct_shp  2048 
   #define ct_stp  512 
   #define ct_altp  1024

   #define ct_space   57 
   #define ct_bspace  14 
   #define ct_lshift  42 
   #define ct_rshift  54 
   #define ct_lstrg  29 
   #define ct_rstrg  129 
   #define ct_lalt  56 
   #define ct_ralt  156 

   #define ct_roll  70 
   #define ct_pause  130 
   #define ct_druck  155 

   #define ct_esc    1 
   #define ct_tab    15 

   #define ct_shift_tab    (ct_tab | ct_shp)

   #define ct_capslck  58 
   #define ct_sb  86 
   #define ct_gitt  43 
   #define ct_enter  28 
   #define ct_minus  53 
   #define ct_plus   27 
   #define ct_punkt  52 
   #define ct_point ct_punkt
   #define ct_komma  51 

   #define ct_num  69 
   #define ct_divk  153 
   #define ct_malk  55 
   #define ct_minusk  74 
   #define ct_plusk  78 
   #define ct_enterk  128 
   #define ct_entk  83 
   #define ct_0k  82 
   #define ct_1k  79 
   #define ct_2k  80 
   #define ct_3k  81 
   #define ct_4k  75 
   #define ct_5k  76 
   #define ct_6k  77 
   #define ct_7k  71 
   #define ct_8k  72 
   #define ct_9k  73 

   #define ct_a  30 
   #define ct_b  48 
   #define ct_c  46 
   #define ct_d  32 
   #define ct_e  18 
   #define ct_f  33 
   #define ct_g  34 
   #define ct_h  35 
   #define ct_i  23 
   #define ct_j  36 
   #define ct_k  37 
   #define ct_l  38 
   #define ct_m  50 
   #define ct_n  49 
   #define ct_o  24 
   #define ct_p  25 
   #define ct_q  16 
   #define ct_r  19 
   #define ct_s  31 
   #define ct_t  20 
   #define ct_u  22 
   #define ct_v  47 
   #define ct_w  17 
   #define ct_x  45 
   #define ct_y  44 
   #define ct_z  21 
   #define ct_oe 39 
   #define ct_ae 40 
   #define ct_ue 26 

   #define ct_circ  41 
   #define ct_0  11 
   #define ct_1  2 
   #define ct_2  3 
   #define ct_3  4 
   #define ct_4  5 
   #define ct_5  6 
   #define ct_6  7 
   #define ct_7  8 
   #define ct_8  9 
   #define ct_9  10 
   #define ct_ez  12 
   #define ct_apos  13 

   #define ct_left   175 
   #define ct_right  177 
   #define ct_up     172 
   #define ct_down   180 

   #define ct_entf   183 
   #define ct_einf   182 
   #define ct_pos1   171 
   #define ct_ende   179 
   #define ct_pdown  181 
   #define ct_pup    173 

   #define ct_f1     59 
   #define ct_f2     60 
   #define ct_f3     61 
   #define ct_f4     62 
   #define ct_f5     63 
   #define ct_f6     64 
   #define ct_f7     65 
   #define ct_f8     66 
   #define ct_f9     67 
   #define ct_f10    68 
   #define ct_f11    87 
   #define ct_f12    88 
#endif

   #define cto_stp  -96
   #define cto_entf   83+256 
   #define cto_space   32 
   #define cto_bspace  8 
   #define cto_a  'a' 
   #define cto_b  'b' 
   #define cto_c  'c' 
   #define cto_d  'd' 
   #define cto_e  'e' 
   #define cto_f  'f' 
   #define cto_g  'g' 
   #define cto_h  'h' 
   #define cto_i  'i' 
   #define cto_j  'j' 
   #define cto_k  'k' 
   #define cto_l  'l' 
   #define cto_m  'm' 
   #define cto_n  'n' 
   #define cto_o  'o' 
   #define cto_p  'p' 
   #define cto_q  'q' 
   #define cto_r  'r' 
   #define cto_s  's' 
   #define cto_t  't' 
   #define cto_u  'u' 
   #define cto_v  'v' 
   #define cto_w  'w' 
   #define cto_x  'x' 
   #define cto_y  'y' 
   #define cto_z  'z' 
   #define cto_oe 'î' 
   #define cto_ae 'Ñ' 
   #define cto_ue 'Å' 
   #define cto_0  '0' 
   #define cto_1  '1' 
   #define cto_2  '2' 
   #define cto_3  '3' 
   #define cto_4  '4' 
   #define cto_5  '5' 
   #define cto_6  '6' 
   #define cto_7  '7' 
   #define cto_8  '8' 
   #define cto_9  '9' 

   #define cto_tab    9 
   #define cto_esc    27 
   #define cto_invvalue   -1
   #define cto_invvalue2  -2
   #define cto_left   75+256 
   #define cto_right  77+256 
   #define cto_up     72+256 
   #define cto_down   80+256 
   #define cto_enter  13 
   #define cto_pos1   71+256 
   #define cto_ende   79+256 
   #define cto_pdown  81+256 
   #define cto_pup    73+256 
   #define cto_einf   82+256 
   #define cto_f1     59+256 
   #define cto_f2     60+256 
   #define cto_f3     61+256 
   #define cto_f4     62+256 
   #define cto_f5     63+256 
   #define cto_f6     64+256 
   #define cto_f7     65+256 
   #define cto_f8     66+256 
   #define cto_f9     67+256 
   #define cto_f10    68+256 
   #define cto_f11    254 
   #define cto_f12    253 
   #define cto_minus  '-' 
   #define cto_plus   '+' 
   #define cto_point  '.' 
   #define cto_komma  ',' 

#else //  _DOS_

   #include "SDL.h"

   #define ct_invvalue   -1
   #define ct_shp  2048
   #define ct_stp  512
   #define ct_altp  1024

   #define ct_space   SDLK_SPACE
   #define ct_bspace  SDLK_BACKSPACE
   #define ct_lshift  SDLK_LSHIFT
   #define ct_rshift  SDLK_RSHIFT
   #define ct_lstrg   SDLK_LCTRL
   #define ct_rstrg   SDLK_RCTRL
   #define ct_lalt    SDLK_LALT
   #define ct_ralt    SDLK_RALT

   #define ct_roll    SDLK_SCROLLOCK
   #define ct_pause   SDLK_PAUSE
   #define ct_druck   SDLK_PRINT

   #define ct_esc     SDLK_ESCAPE
   #define ct_tab     SDLK_TAB

   #define ct_shift_tab    (ct_tab | ct_shp)

   #define ct_capslck SDLK_CAPSLOCK
   #define ct_sb      SDLK_LESS 
   #define ct_gitt    SDLK_HASH
   #define ct_enter   SDLK_RETURN
   #define ct_minus   SDLK_MINUS
   #define ct_plus    SDLK_PLUS
   #define ct_punkt   SDLK_PERIOD
   #define ct_point ct_punkt
   #define ct_komma   SDLK_COMMA

   #define ct_num     SDLK_NUMLOCK
   #define ct_divk    SDLK_KP_DIVIDE
   #define ct_malk    SDLK_KP_MULTIPLY
   #define ct_minusk  SDLK_KP_MINUS
   #define ct_plusk   SDLK_KP_PLUS
   #define ct_enterk  SDLK_KP_ENTER
   #define ct_entk    SDLK_KP_PERIOD
   #define ct_0k      SDLK_KP0
   #define ct_1k      SDLK_KP1
   #define ct_2k      SDLK_KP2
   #define ct_3k      SDLK_KP3
   #define ct_4k      SDLK_KP4
   #define ct_5k      SDLK_KP5
   #define ct_6k      SDLK_KP6
   #define ct_7k      SDLK_KP7
   #define ct_8k      SDLK_KP8
   #define ct_9k      SDLK_KP9

   #define ct_a  SDLK_a
   #define ct_b  SDLK_b
   #define ct_c  SDLK_c
   #define ct_d  SDLK_d
   #define ct_e  SDLK_e
   #define ct_f  SDLK_f
   #define ct_g  SDLK_g
   #define ct_h  SDLK_h
   #define ct_i  SDLK_i
   #define ct_j  SDLK_j
   #define ct_k  SDLK_k
   #define ct_l  SDLK_l
   #define ct_m  SDLK_m
   #define ct_n  SDLK_n
   #define ct_o  SDLK_o
   #define ct_p  SDLK_p
   #define ct_q  SDLK_q
   #define ct_r  SDLK_r
   #define ct_s  SDLK_s
   #define ct_t  SDLK_t
   #define ct_u  SDLK_u
   #define ct_v  SDLK_v
   #define ct_w  SDLK_w
   #define ct_x  SDLK_x
   #define ct_y  SDLK_y
   #define ct_z  SDLK_z
//   #define ct_oe 39
//   #define ct_ae 40
//   #define ct_ue 26

   #define ct_circ  SDLK_CARET
   #define ct_0  SDLK_0
   #define ct_1  SDLK_1
   #define ct_2  SDLK_2
   #define ct_3  SDLK_3
   #define ct_4  SDLK_4
   #define ct_5  SDLK_5
   #define ct_6  SDLK_6
   #define ct_7  SDLK_7
   #define ct_8  SDLK_8
   #define ct_9  SDLK_9
//   #define ct_ez  12
//   #define ct_apos  13

   #define ct_left   SDLK_LEFT
   #define ct_right  SDLK_RIGHT
   #define ct_up     SDLK_UP
   #define ct_down   SDLK_DOWN

   #define ct_entf   SDLK_DELETE
   #define ct_einf   SDLK_INSERT
   #define ct_pos1   SDLK_HOME
   #define ct_ende   SDLK_END
   #define ct_pdown  SDLK_PAGEDOWN
   #define ct_pup    SDLK_PAGEUP

   #define ct_f1     SDLK_F1
   #define ct_f2     SDLK_F2
   #define ct_f3     SDLK_F3
   #define ct_f4     SDLK_F4
   #define ct_f5     SDLK_F5
   #define ct_f6     SDLK_F6
   #define ct_f7     SDLK_F7
   #define ct_f8     SDLK_F8
   #define ct_f9     SDLK_F9
   #define ct_f10    SDLK_F10
   #define ct_f11    SDLK_F11
   #define ct_f12    SDLK_F12

   #define cto_invvalue   -1
   #define cto_shp  2048
   #define cto_stp  512
   #define cto_altp  1024

   #define cto_space   SDLK_SPACE
   #define cto_bspace  SDLK_BACKSPACE
   #define cto_lshift  SDLK_LSHIFT
   #define cto_rshift  SDLK_RSHIFT
   #define cto_lstrg   SDLK_LCTRL
   #define cto_rstrg   SDLK_RCTRL
   #define cto_lalt    SDLK_LALT
   #define cto_ralt    SDLK_RALT

   #define cto_roll    SDLK_SCROLLOCK
   #define cto_pause   SDLK_PAUSE
   #define cto_druck   SDLK_PRINT

   #define cto_esc     SDLK_ESCAPE
   #define cto_tab     SDLK_TAB

   #define cto_shift_tab    (cto_tab | cto_shp)

   #define cto_capslck SDLK_CAPSLOCK
   #define cto_sb      SDLK_LESS
   #define cto_gitt    SDLK_HASH
   #define cto_enter   SDLK_RETURN
   #define cto_minus   SDLK_MINUS
   #define cto_plus    SDLK_PLUS
   #define cto_punkt   SDLK_PERIOD
   #define cto_point cto_punkt
   #define cto_komma   SDLK_COMMA

   #define cto_num     SDLK_NUMLOCK
   #define cto_divk    SDLK_KP_DIVIDE
   #define cto_malk    SDLK_KP_MULTIPLY
   #define cto_minusk  SDLK_KP_MINUS
   #define cto_plusk   SDLK_KP_PLUS
   #define cto_enterk  SDLK_KP_ENTER
   #define cto_entk    SDLK_KP_PERIOD
   #define cto_0k      SDLK_KP0
   #define cto_1k      SDLK_KP1
   #define cto_2k      SDLK_KP2
   #define cto_3k      SDLK_KP3
   #define cto_4k      SDLK_KP4
   #define cto_5k      SDLK_KP5
   #define cto_6k      SDLK_KP6
   #define cto_7k      SDLK_KP7
   #define cto_8k      SDLK_KP8
   #define cto_9k      SDLK_KP9

   #define cto_a  SDLK_a
   #define cto_b  SDLK_b
   #define cto_c  SDLK_c
   #define cto_d  SDLK_d
   #define cto_e  SDLK_e
   #define cto_f  SDLK_f
   #define cto_g  SDLK_g
   #define cto_h  SDLK_h
   #define cto_i  SDLK_i
   #define cto_j  SDLK_j
   #define cto_k  SDLK_k
   #define cto_l  SDLK_l
   #define cto_m  SDLK_m
   #define cto_n  SDLK_n
   #define cto_o  SDLK_o
   #define cto_p  SDLK_p
   #define cto_q  SDLK_q
   #define cto_r  SDLK_r
   #define cto_s  SDLK_s
   #define cto_t  SDLK_t
   #define cto_u  SDLK_u
   #define cto_v  SDLK_v
   #define cto_w  SDLK_w
   #define cto_x  SDLK_x
   #define cto_y  SDLK_y
   #define cto_z  SDLK_z
//   #define cto_oe 39
//   #define cto_ae 40
//   #define cto_ue 26

   #define cto_circ  SDLK_CARET
   #define cto_0  SDLK_0
   #define cto_1  SDLK_1
   #define cto_2  SDLK_2
   #define cto_3  SDLK_3
   #define cto_4  SDLK_4
   #define cto_5  SDLK_5
   #define cto_6  SDLK_6
   #define cto_7  SDLK_7
   #define cto_8  SDLK_8
   #define cto_9  SDLK_9
//   #define cto_ez  12
//   #define cto_apos  13

   #define cto_left   SDLK_LEFT
   #define cto_right  SDLK_RIGHT
   #define cto_up     SDLK_UP
   #define cto_down   SDLK_DOWN

   #define cto_entf   SDLK_DELETE
   #define cto_einf   SDLK_INSERT
   #define cto_pos1   SDLK_HOME
   #define cto_ende   SDLK_END
   #define cto_pdown  SDLK_PAGEDOWN
   #define cto_pup    SDLK_PAGEUP

   #define cto_f1     SDLK_F1
   #define cto_f2     SDLK_F2
   #define cto_f3     SDLK_F3
   #define cto_f4     SDLK_F4
   #define cto_f5     SDLK_F5
   #define cto_f6     SDLK_F6
   #define cto_f7     SDLK_F7
   #define cto_f8     SDLK_F8
   #define cto_f9     SDLK_F9
   #define cto_f10    SDLK_F10
   #define cto_f11    SDLK_F11
   #define cto_f12    SDLK_F12


#endif //_DOS_

    typedef int tkey;

    extern char  skeypress( tkey keynr);
    extern char *get_key(tkey keynr);
    extern tkey r_key(void);
    extern int  rp_key(void);
    extern int keypress(void);
    extern void initkeyb(void);
    extern void closekeyb(void);
    extern void wait(void);
    extern tkey char2key (int ch);
    extern void getkeysyms ( tkey* keysym, int* keyprnt );

#ifndef _DOS_
extern int getch(void);
extern int kbhit(void);
#endif

