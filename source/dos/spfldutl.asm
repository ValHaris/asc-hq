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

IFNDEF WATCOM
  .386
  MODEL flat,Cpp
ENDIF

.DATA

INCLUDE basemacr.inc

_vfbstartdif DD ?
PUBLIC _vfbstartdif

_scrstartdif DD ?
PUBLIC _scrstartdif

_scrstartpage DD ?
PUBLIC _scrstartpage

.CODE

PUBLIC luftlinie8_
luftlinie8_ PROC NEAR
   LOCAL y1 : DWORD
   LOCAL y2 : DWORD


   mov y1, ebx
   mov y2, edx


   shl eax,1

   and ebx,1
   add eax, ebx


   shl ecx,1
   and edx,1
   add ecx,edx


   sub eax, ecx
   cmp eax,0
   jg @POS1
   neg eax

@POS1:

   mov ecx,y1
   sub ecx,y2
   cmp ecx,0
   jg @POS2
   neg ecx

@POS2:

    cmp eax,ecx
    ja @WEITER

    xchg eax,ecx
@WEITER:

    sub eax,ecx
    shr eax,1
    imul eax,12           ; maxmalq
    shl ecx,3
    add eax,ecx
    ret

ENDP

PUBLIC ROL_
ROL_ PROC NEAR
   rol al,cl
   ret
ENDP

PUBLIC setvisibility_
setvisibility_ PROC NEAR
; parameter
;   eax : ptr to word to modify
;   ebx : value to set
;    cl : spieldaten.actspieler

      shl cl,1


      mov dx,0fffch
      rol dx,cl

      and WORD PTR [eax], dx

      rol bx,cl

      or  WORD PTR [eax],bx

      ret
ENDP


PUBLIC dispspielfeld_
dispspielfeld_ PROC NEAR
     push es
     push ebp

     mov ebp, eax



     mov edi, [ebp]  ; Position des Virtual Frame Buffer
     add ebp,4

     mov ax, ds
     mov es, ax


     mov eax, [ebp]  ; Anzahl Bilder
     add ebp,4

  @@PICTSTART:
     mov edx, [ebp]  ; Anzahl Zeilen
     add ebp, 4

     mov esi, [ebx]  ; Pointer auf DirecPict Daten
     add ebx,4

   @@LINESTART:
     mov ecx, [ebp]  ; AnzahlPixel bis zum n„chsten spacing ; 4er Schritte
     add ebp,4
     rep movsd

     mov ecx, [ebp]
     add ebp,4
     rep movsw

     add edi, [ebp]
     add ebp, 4

     dec edx
     jnz @@LINESTART

     dec eax
     jnz @@PICTSTART


     pop ebp
     pop es

     ret

ENDP






PUBLIC copyvfb2displaymemory_
copyvfb2displaymemory_ PROC NEAR
   LOCAL temp : WORD
   LOCALS
   push es
   mov ax, ds
   mov es,ax

   mov edi, _hardwaregraphmodeparameters.linearaddress
   add edi, _scrstartdif

   mov esi, _Activegraphmodeparameters.linearaddress
   add esi, _vfbstartdif

   cmp _hardwaregraphmodeparameters.windowstatus, 100
   jz @@LINEARFRAMEBUF


   mov ecx, _scrstartpage
   cmp byte ptr _hardwaregraphmodeparameters.actsetpage, cl
   jz @@LOOPSTART

   mov byte ptr _hardwaregraphmodeparameters.pagetoset,cl
   push ebx
   SetVirtualPage _hardwaregraphmodeparameters
   pop ebx

  @@loopstart:
   mov edx,[ebx]
   add ebx,4

  @@LINESTART:

   mov ecx,[ebx]
   add ebx,4

   add esi, ecx
   add di, cx
   jnc @@NB
   setnextvirtualpage2 _hardwaregraphmodeparameters

 @@NB:

   mov ecx,[ebx]
   add ebx,4

   processbytesLW2 copybytes

   dec edx
   jnz @@LINESTART

   jmp @@ENDE


 @@LINEARFRAMEBUF:
   mov edx,[ebx]
   add ebx,4

  @@LLINESTART:

   mov ecx,[ebx]
   add ebx,4

   add esi, ecx
   add edi, ecx

   mov ecx,[ebx]
   add ebx,4

   copybytes

   dec edx
   jnz @@LLINESTART


 @@ENDE:
   pop es
   ret
ENDP


PUBLIC copyvfb2displaymemory_zoom_
copyvfb2displaymemory_zoom_ PROC NEAR
   LOCAL temp : WORD
   LOCALS
   push es
   push ebp

   mov esi, [ebx]
   mov edi, [ebx+4]
   mov edx, [ebx+12]

 @@yloop:
   mov ebp, [ebx+16]
   mov ecx, [ebx+8]
  @@xloop:
   add esi, [ebp]
   movsb
;   lodsb
   add ebp, 4
;   stosb
;   dec esi
   dec ecx
   jnz @@xloop

   add esi, [ebx+20]
   add edi, [ebx+24]

   mov ebp, [ebx+28]
   dec edx
   add esi, [ebp+4*edx]

   cmp edx, 0
   jnz @@yloop

   pop ebp
   pop es
   ret
ENDP


END
