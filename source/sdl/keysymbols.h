/***************************************************************************
                          keysymbols.h  -  description
                             -------------------
    begin                : Sat Oct 14 2000
    copyright            : (C) 2000 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef keysymbols_h_included
 #define keysymbols_h_included


   #include "../global.h"
   #include sdlheader

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

#endif //keysymbols_h_included
