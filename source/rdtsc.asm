;    This file is part of Advanced Strategic Command; http://www.asc-hq.de
;    Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger
;
;    This program is free software; you can redistribute it and/or modify
;    it under the terms of the GNU General Public License as published by
;    the Free Software Foundation; either version 2 of the License, or
;    (at your option) any later version.
;
;    This program is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU General Public License for more details.
;
;    You should have received a copy of the GNU General Public License
;    along with this program; see the file COPYING. If not, write to the 
;    Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
;    Boston, MA  02111-1307  USA

.DATA

 inited   dd  0
 fakecounter dd 0


.CODE


detect proc near
        pushfd                                  ;  ;
        pop     eax                             ;  ;flags in eax
        mov     ebx,eax                         ;  ;flags in ebx also
        xor     eax,40000H                      ;  ;toggle bit 18
        push    eax                             ;  ;
        popfd                                   ;  ;put it back into flags reg
        pushfd                                  ;  ;
        pop     eax                             ;  ;flags back in eax

        cmp eax, ebx
        jnz @@l1
  ;      .if eax == ebx                          ;  ;if the bit got undone
          mov   eax,0                           ;  ;then report a 386
          ret                                   ;  ;
  ;      .endif                                  ;  ;
       @@l1:
        mov     ebx,eax                         ;  ;flags in eax and ebx
        xor     eax,200000H                     ;  ;toggle bit 21
        push    eax                             ;  ;
        popfd                                   ;  ;put it back into flags reg
        pushfd                                  ;  ;
        pop     eax                             ;  ;flags back in eax

        cmp eax, ebx
        jnz @@l2
  ;      .if eax == ebx                          ;  ;if the bit got undone
          mov   eax,0                           ;  ;then report a 486
          ret                                   ;  ;
  ;      .endif                                  ;  ;
       @@l2:
        mov     eax,1                           ;  ;
        cpuid                                   ;  ;CPUID - 0fH, 0a2H
        mov eax, edx
        shr     eax,4                           ;  ;in low 4 bits
        and eax, 1
        ret                                     ;  ;return 5, 6, or later
detect endp 

PUBLIC gettimestampcounter_
gettimestampcounter_ proc near
  @@start:
   cmp inited, 1
   ja @@fake
   jb @@init

   rdtsc
   shrd eax, edx, 20
   ret

   @@init:
    call detect
    cmp eax, 1
    jz @@ok
    mov inited, 2
    jmp @@start

   @@ok:

    mov inited, 1

    rdtsc
    shrd eax, edx, 20
    cmp eax, 70000000h
    jna @@start
    mov inited, 2

    jmp @@start

   @@fake:
    inc fakecounter
    mov eax, fakecounter
    ret

gettimestampcounter_ endp

END
