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


INCLUDE BASEMACR.INC

PUBLIC _scratch
_scratch      dd    ?

PUBLIC _datalength
_datalength   dd    ?

PUBLIC _spacelength
_spacelength  dw    ?

PUBLIC _planenum
_planenum     dw    ?

PUBLIC _scanlineret
_ScanLineRet  dd 4 dup  (?)

PUBLIC _dataread
_DataRead     Dd    ?

PUBLIC _datatoread
_DataToRead   dd    ?

PUBLIC _linelength
_linelength dd ?

  linepos      dw ?
  videoindex   dd ?
  repeatcount  db ?
  planenum2    dw ?
  actplane     dw ?

  RLEcount          EQU ah


.CODE

PUBLIC resetdata_
resetdata_ proc near

    mov ebx,_ActiveGraphModeParameters.LinearAddress

    mov edx,_linelength
    mov linepos,dx

    mov actplane,0

    mov ebx, dword ptr _ScanLineRet
    add eax,ebx
    add eax,_ActiveGraphModeParameters.LinearAddress
    mov videoindex,eax
    mov repeatcount,0
    mov ax, word ptr _planenum
    shl ax,2
    mov planenum2,ax

    ret

resetdata_ endp


; works now with Truecolor too
PUBLIC decode_pcx256_
decode_pcx256_  PROC near
   LOCALS
   push ebp

   cmp _ActiveGraphModeParameters.windowstatus,100
   jz @@LinearFrameBuffer

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; 64 kb Video Window ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


   mov dx, linepos

   mov edi, videoindex

   mov RLEcount, repeatcount               ; count in CL *)
   movzx ebp, ActPlane

   mov esi, dword ptr _scratch             ; pointer to buffer *)
   cld                                     ; clear DF *)
   mov ecx, dword ptr _DataRead            ; number of bytes to process


   ; --------------------- Loop through input buffer ----------------------- *)


  @getbyte:
  @l3:
   dec _Datalength                ; end of inputBuffer ?
   jz @Exit                       ; yes, quit *)

   lodsb                          ; get byte into AL, increment SI *)
   cmp RLEcount, 0                ; was prev. byte a count? *)
   jg @multi_data                 ; yes, this is data *)
   cmp al, 192                    ; no, test high bits *)
   jb @one_data                   ; not set, not a count *)

   ; It's a count byte *)
   XOR al, 192                    ; get count from 6 low bits *)
   mov RLEcount, al               ; store repeat count *)
   jmp @getbyte                   ; go get data byte *)

  @one_data:
   mov RLEcount,1

  @multi_data:
   mov [edi],al
   add di, word ptr _ActiveGraphModeParameters.BytePerPix
   jc @SetNVPage1
  @wtr1:
   sub dx,1                         ; decrease pixnum of scanline
   jnz @@www3

   mov edx, _Linelength
   add ebp,4                          ; increase active plane
   cmp bp,planenum2                  ; number of planes reached ?
   je @NextLine

   sub di,word ptr [_ScanLineRet+ebp]          ; go back to startposition of line
   jc @SetVPPage1

  @NoBankBack:
   jmp @@www3

  @NextLine:
   add di,word ptr  [_ScanLineRet+ebp]
   jc @SetNVpage2

  @NoBankBack2:
   mov ebp,0





   @@www3:
   dec ecx                        ; picture finished ?
   jz @exit                       ; leave procedure
   dec RLEcount
   jnz @multi_data


   jmp @getbyte

   ; ------------------------- Finished with buffer ------------------------ *)

  @Exit:
   mov videoindex, edi               ; save status for next run thru buffer *)
   mov repeatcount, RLEcount
   mov linepos,dx
   mov actplane,bp
   mov dword ptr _DataRead,ecx

   pop ebp
   ret

   @SetNVPage1:
   SetNextVirtualPage2
   jmp @wtr1

   @SetNVPage2:
   SetNextVirtualPage2
   jmp @NoBankBack2

   @SetVPPage1:
   SetPrivVirtualPage2
   jmp @NoBankBack

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@LinearFrameBuffer:

   mov dx, linepos

   mov edi, videoindex

   mov RLEcount, repeatcount               ; count in CL *)
   movzx ebp, ActPlane

   mov esi, dword ptr _scratch             ; pointer to buffer *)
   cld                                     ; clear DF *)
   mov ecx, dword ptr _DataRead            ; number of bytes to process


   ; --------------------- Loop through input buffer ----------------------- *)


 @@Lingetbyte:
  @@Linl3:
   dec _Datalength                ; end of inputBuffer ?
   jz @@LinExit                       ; yes, quit *)

   lodsb                          ; get byte into AL, increment SI *)
   cmp RLEcount, 0                ; was prev. byte a count? *)
   jg @@Linmulti_data                 ; yes, this is data *)
   cmp al, 192                    ; no, test high bits *)
   jb @@Linone_data                   ; not set, not a count *)

   ; It's a count byte *)
   XOR al, 192                    ; get count from 6 low bits *)
   mov RLEcount, al               ; store repeat count *)
   jmp @@Lingetbyte                   ; go get data byte *)

  @@Linone_data:
   mov RLEcount,1

  @@Linmulti_data:
   mov [edi],al
   add edi,_ActiveGraphModeParameters.BytePerPix
   sub dx,1                         ; decrease pixnum of scanline
   jnz @@linnoplanechange




   mov edx, _Linelength
   add ebp,4                          ; increase active plane
   cmp bp,planenum2                  ; number of planes reached ?
   je @@LinNextLine

   sub edi,dword ptr [_ScanLineRet+ebp]          ; go back to startposition of line

   jmp @@linnoplanechange

  @@LinNextLine:
   add edi,dword ptr  [_ScanLineRet+ebp]

   mov ebp,0





  @@linnoplanechange:

   dec ecx                        ; picture finished ?
   jz @@Linexit                       ; leave procedure
   dec RLEcount
   jnz @@Linmulti_data


   jmp @@Lingetbyte

   ; ------------------------- Finished with buffer ------------------------ *)

  @@LinExit:
   mov videoindex, edi               ; save status for next run thru buffer *)
   mov repeatcount, RLEcount
   mov linepos,dx
   mov actplane,bp
   mov dword ptr _DataRead,ecx

   pop ebp
   ret



decode_pcx256_ endp




END
