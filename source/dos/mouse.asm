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

; _mouseproc dd 0
; PUBLIC _mouseproc 

.CODE



; ========================================================
; --------------------------------------------------------
;         P U T  M O U S E  B A C K G R O U N D
; --------------------------------------------------------
; ========================================================


PUBLIC  putmousebackground_
putmousebackground_ proc near
     LOCAL LineLength: dword
     LOCAL SpaceLength: dword
     LOCAL LineNum: dword
     LOCAL orgpage: byte
     LOCALS

     push es
     push ds
     pop es

     mov ebx, [_mouseparams.mouse_altx]
     cmp ebx, -1
     jz @@keinbswtschende
     mov ecx, [_mouseparams.mouse_alty]

     mov eax, _MouseParams.mouse_background
     cmp eax,0
     jz @@keinbswtschende

     mov esi, eax

     lodssw

     inc ax
     and eax, 0FFFFh
     mov lineLength,eax

     mov edx,_hardwareGraphModeParameters.ScanLineLength
     sub edx,eax
     mov spaceLength,edx

     lodssw
     inc ax
     mov LineNum, eax

     cmp _hardwareGraphModeParameters.windowstatus, 100

     jz @@LinearPutImage


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; 64 kb Video Window ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


     mov eax,_hardwareGraphModeParameters.ScanLineLength
     mul word ptr _mouseparams.mouse_alty

     add ax,word ptr _mouseparams.mouse_altx
     adc dx,0

     mov cl,byte ptr _hardwareGraphModeParameters.actsetpage
     mov orgpage, cl

     cmp dl,cl
     jz @@keinbswtsch

     mov byte ptr _hardwareGraphModeParameters.pagetoset,dl
     push eax
     push ebx
     SetVirtualPage _hardwareGraphModeParameters
     pop ebx
     pop eax

  @@keinbswtsch:

     mov edi, eax
     add edi,_hardwareGraphModeParameters.LinearAddress
                          ; Jetzt befindet sich in
                          ; eax : Startoffset im Bildschirm

     mov edx,LineNum

@@ZeileStart:

     mov ecx,linelength

   @@PixStart:
     mov al, [esi]
     mov [edi],al
     inc esi
     add di,1
     jnc @@nobsw
     SetNextVirtualPage2 _hardwareGraphModeParameters

   @@nobsw:
     dec ecx
     jnz @@Pixstart

     add di, word ptr Spacelength
     jnc @@KeinBW

     SetNextVirtualPage2 _hardwareGraphModeParameters

   @@KeinBW:
     dec edx
     jnz @@ZeileStart

   @@ende:

     movzx dx,orgpage

     cmp byte ptr _hardwareGraphModeParameters.actsetpage,dl
     jz @@keinbswtschende

     mov byte ptr _hardwareGraphModeParameters.pagetoset,dl
     SetVirtualPage _hardwareGraphModeParameters

  @@keinbswtschende:


     mov [_mouseparams.mouse_altx], -1
     ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@Linearputimage:

     mov eax,_hardwareGraphModeParameters.ScanLineLength
     mov ecx, _mouseparams.mouse_alty
     mul ecx

     mov ecx, _mouseparams.mouse_altx
     add eax,ecx


     add eax,_hardwareGraphModeParameters.LinearAddress
     mov edi, eax

     mov edx,LineNum

@@LinZeileStart:

     mov ecx,linelength

     rep movsb

     add edi, Spacelength

     dec edx
     jnz @@LinZeileStart

     mov [_mouseparams.mouse_altx], -1
     pop es
     ret

putmousebackground_ endp






; ========================================================
; --------------------------------------------------------
;            P U T  M O U S E  P O I N T E R
; --------------------------------------------------------
; ========================================================


PUBLIC  putmousepointer_
putmousepointer_ proc near
     LOCAL LineLength: dword
     LOCAL SpaceLength: dword
     LOCAL LineNum: dword
     LOCAL orgpage: byte
     LOCALS

     push es
     push ds
     pop es

     mov ebx, [_mouseparams.mouse_x1]
     mov ecx, [_mouseparams.mouse_y1]
     mov [_mouseparams.mouse_altx], ebx
     mov [_mouseparams.mouse_alty], ecx

     mov eax, _MouseParams.mouse_pointer
     cmp eax,0
     jz @@ende

     mov esi, eax

     mov ebx, _MouseParams.mouse_background

     lodssw
     mov [ebx],ax
     add ebx,2

   @@LLL1:
     inc ax
     and eax, 0FFFFh
     mov lineLength,eax

     mov edx,_hardwareGraphModeParameters.ScanLineLength
     sub edx,eax
     mov spaceLength,edx

     lodssw
     mov [ebx],ax
     add ebx,2
     inc ax
     mov LineNum, eax

     cmp _hardwareGraphModeParameters.windowstatus, 100

     jz @@LinearPutImage


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; 64 kb Video Window ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


     mov eax,_hardwareGraphModeParameters.ScanLineLength
     mul word ptr _mouseparams.mouse_y1

     add ax,word ptr _mouseparams.mouse_x1
     adc dx,0

     mov cl,byte ptr _hardwareGraphModeParameters.actsetpage
     mov orgpage, cl

     cmp dl,cl
     jz @@keinbswtsch

     mov byte ptr _hardwareGraphModeParameters.pagetoset,dl
     push eax
     push ebx
     SetVirtualPage _hardwareGraphModeParameters
     pop ebx
     pop eax

  @@keinbswtsch:

     mov edi, eax
     add edi,_hardwareGraphModeParameters.LinearAddress
                          ; Jetzt befindet sich in
                          ; eax : Startoffset im Bildschirm

     mov edx,LineNum

@@ZeileStart:

     mov ecx,linelength
 @@Pixstart:
     mov al, [edi]
     mov [ebx], al
     mov al, [esi]
     cmp al,255
     jz @@Transparent
     mov [edi],al
   @@Transparent:
     inc ebx
     inc esi
     add di,1
     jnc @@nobsw
     SetNextVirtualPage2 _hardwareGraphModeParameters

   @@nobsw:
     dec ecx
     jnz @@Pixstart


     add di, word ptr Spacelength
     jnc @@KeinBW

     SetNextVirtualPage2 _hardwareGraphModeParameters

   @@KeinBW:
     dec edx
     jnz @@ZeileStart

   @@ende:
     movzx dx,orgpage

     cmp byte ptr _hardwareGraphModeParameters.actsetpage,dl
     jz @@keinbswtschende

     mov byte ptr _hardwareGraphModeParameters.pagetoset,dl
     SetVirtualPage _hardwareGraphModeParameters

  @@keinbswtschende:
     ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@Linearputimage:

     mov eax,_hardwareGraphModeParameters.ScanLineLength
     mov ecx, _mouseparams.mouse_y1
     mul ecx

     mov ecx, _mouseparams.mouse_x1
     add eax,ecx


     add eax,_hardwareGraphModeParameters.LinearAddress
     mov edi, eax

     mov edx,LineNum

@@LinZeileStart:

     mov ecx,linelength

   @@LinPixstart:
     mov al, [edi]
     mov [ebx], al
     mov al, [esi]
     cmp al,255
     jz @@LinTransparent
     mov [edi],al
   @@LinTransparent:
     inc ebx
     inc esi
     inc edi
     dec ecx
     jnz @@LinPixstart


     add edi, Spacelength

     dec edx
     jnz @@LinZeileStart

     pop es
     ret

putmousepointer_ endp



comment #




PUBLIC mouseintproc_
mouseintproc_ PROC near

        cli
        pushf
        push eax
        push ebx
        push ecx
        push edx
        push esi
        push edi
        push ebp
        push es

        mov [_mouseparams.mouse_taste],bl
        and ecx,0FFFFh
        and edx,0FFFFh

        mov eax,[_mouseparams.mouse_SizeX]
        add eax,ecx
        cmp [_HardwareGraphModeParameters.resolutionX], eax
        jnbe wtrx
        mov ecx,[_HardwareGraphModeParameters.resolutionX]
        ;dec ecx
        sub ecx,[_mouseparams.mouse_SizeX]

    wtrx:
        mov [_mouseparams.mouse_x],ecx

        mov [_mouseparams.mouse_y],edx

        cmp [_mouseparams.mouse_Status],2
        jb ende

        cmp ecx,[_mouseparams.mouse_altx]
        jnz verschiebung
        cmp edx, [_mouseparams.mouse_AltY]
        jnz verschiebung
        jmp ende

    verschiebung:

        mov ax, ds
        mov es, ax


        cmp [_mouseparams.mouse_altx], -1
        jz noputi

        call putmousebackground_
      noputi:
        mov ecx, [_mouseparams.mouse_x]

        mov edx, [_mouseparams.mouse_y]

        cmp ecx,[_mouseparams.mouse_offx1]
        jnge not_in_ininvisible_rect

        cmp edx,[_mouseparams.mouse_offy1]
        jnge not_in_ininvisible_rect

        cmp ecx,[_mouseparams.mouse_offx2]
        jnle not_in_ininvisible_rect

        cmp edx,[_mouseparams.mouse_offy2]
        jnle not_in_ininvisible_rect

        mov [_mouseparams.mouse_altx], -1
        jmp ende

    not_in_ininvisible_rect:

        call putmousepointer_


   ende:
        cmp _mouseproc, 0
        jz realend
        call _mouseproc
   realend:

        pop es
        pop ebp
        pop edi
        pop esi
        pop edx
        pop ecx
        pop ebx
        pop eax
        popf
        sti

        ret
mouseintproc_ endp

#


PUBLIC mouseintproc2_
mouseintproc2_ PROC near

        cli
        push ebx
        push ecx
        push edx
        push ds
        pop es
        

        mov [_mouseparams.mouse_taste],bl
        and ecx,0ffffh
        mov [_mouseparams.mouse_x1],ecx
        and edx,0ffffh
        mov [_mouseparams.mouse_y1],edx

        pop edx
        pop ecx
        pop ebx

        sti

        ret
mouseintproc2_ endp


end