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

 red   dd ?
 green dd ?
 blue  dd ?
 count dd ?
 actcol dd ?
 actdiff dd ?
 
 
PUBLIC _palette16  
_Palette16     dd 1024 DUP (?)

INCLUDE BASEVESA.INC

.CODE

PUBLIC setvirtualpagepos_
SetVirtualPagePos_ proc near
  pusha

  mov _hardwaregraphmodeparameters.pagetoset, eax

  SetVirtualPage

  popa
  ret
setvirtualpagepos_ endp


Public setdisplaystart_
setdisplaystart_ proc near
   mov ax,4F07h
   mov bx,0

   Int  10h
   ret
setdisplaystart_ endp


PUBLIC setscanlinelength_
setscanlinelength_ proc near
   mov ax,4F06h
   mov bx,0
   int 10h
   movzx eax,bx
   ret
setscanlinelength_ endp

PUBLIC getscanlinelength_
getscanlinelength_ proc near
   mov ax,4F06h
   mov bx,1
   int 10h
   movzx eax,bx
   ret
getscanlinelength_ endp




PUBLIC settextmode_
settextmode_ proc near
     mov ah,0
     int 10h
     ret
settextmode_ endp

; ========================================================
; --------------------------------------------------------
;                       B A R
; --------------------------------------------------------
; ========================================================


PUBLIC bar_
bar_ Proc near color:byte
     LOCAL Linelength   :dword
     LOCAL SpaceLength  :dword
     LOCAL LineNum      :dword
     LOCAL LeftR        :word
     LOCALS

     ; X1        Y1        X2        Y2
     ; eax       ebx       ecx       edx

     inc ecx
     inc edx

     mov linelength,ecx
     sub linelength,eax

     push edx
     push ebx
     push eax

     mov eax, _ActiveGraphModeParameters.ScanLineLength

     mov edx, eax
     sub edx, Linelength
     mov SpaceLength, edx

     cmp _ActiveGraphModeParameters.Windowstatus, 100
     jz @@LinearNbar

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; 64 kb Video Window ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

     mul bx
     pop ebx      ; x1

     add ax,bx
     adc dx,0


     mov edi, eax
     add edi, _ActiveGraphModeParameters.LinearAddress

     pop ebx      ; y1
     pop ecx      ; y2
     sub ecx, ebx

     mov LineNum, ecx  ; Zahl der Zeilen


     mov bx, ax        ; bx ist ab jetzt der ZÑhler innerhalb des 64kB Videofesters


     cmp dl,byte ptr _hardwaregraphmodeparameters.actsetpage

     jz @@keinbswtsch

     mov byte ptr _hardwaregraphmodeparameters.pagetoset,dl
     push ebx
     push edx
     SetVirtualPage
     pop edx
     pop ebx

@@keinbswtsch:

     mov edx, LineNum

     mov al,color
     mov ah,al

@@Zeilestart:

     mov ecx,linelength

     ProcessBytesLW bx setBytes  edi

     add edi, Spacelength
     add bx, word ptr Spacelength
     jnc @@KeinBW


     SetNextVirtualPage edi

   @@KeinBW:
     dec edx
     jnz @@ZeileStart
@@ende:

     leave
     ret 4


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@Linearnbar:
     mul ebx
     pop ebx      ; x1

     add eax,ebx

     mov edi, eax
     add edi, _ActiveGraphModeParameters.LinearAddress

     pop ebx      ; y1
     pop edx      ; y2
     sub edx, ebx ; Zahl der Zeilen

     mov al,color
     mov ah,al

@@ZeilestartL:

     mov ecx,linelength

     setBytes

     add edi, Spacelength

     dec edx
     jnz @@ZeileStartL
@@endeL:

     leave
     ret  4


bar_ endp




; ========================================================
; --------------------------------------------------------
;                   R O T A  B A R
; --------------------------------------------------------
; ========================================================


PUBLIC rotabar_
rotabar_ Proc near color:byte
     LOCAL Linelength   :dword
     LOCAL SpaceLength  :dword
     LOCAL LineNum      :dword
     LOCAL LeftR        :word
     LOCALS

     ; X1        Y1        X2        Y2
     ; eax       ebx       ecx       edx

     inc ecx
     inc edx

     mov linelength,ecx
     sub linelength,eax

     push edx
     push ebx
     push eax

     mov eax, _ActiveGraphModeParameters.ScanLineLength

     mov edx, eax
     sub edx, Linelength
     mov SpaceLength, edx

     cmp _ActiveGraphModeParameters.Windowstatus, 100
     jz @@LinearNbar

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; 64 kb Video Window ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

     mul bx
     pop ebx      ; x1

     add ax,bx
     adc dx,0


     mov edi, eax
     add edi, _ActiveGraphModeParameters.LinearAddress

     pop ebx      ; y1
     pop ecx      ; y2
     sub ecx, ebx

     mov LineNum, ecx  ; Zahl der Zeilen


     mov bx, ax        ; bx ist ab jetzt der ZÑhler innerhalb des 64kB Videofesters


     cmp dl,byte ptr _hardwaregraphmodeparameters.actsetpage

     jz @@keinbswtsch

     mov byte ptr _hardwaregraphmodeparameters.pagetoset,dl
     push ebx
     push edx
     SetVirtualPage
     pop edx
     pop ebx

@@keinbswtsch:

     mov edx, LineNum

     mov al,color

@@Zeilestart:

     mov ecx,linelength

   @@pixstart:
     add [edi], al
     add di, 1
     jnc @@ksw1
     setnextvirtualpage2
   @@ksw1:
     dec ecx
     jnz @@pixstart

     add di, word ptr Spacelength
     jnc @@KeinBW

     SetNextVirtualPage2

   @@KeinBW:
     dec edx
     jnz @@ZeileStart
@@ende:

     ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@Linearnbar:
     mul ebx
     pop ebx      ; x1

     add eax,ebx

     mov edi, eax
     add edi, _ActiveGraphModeParameters.LinearAddress

     pop ebx      ; y1
     pop edx      ; y2
     sub edx, ebx ; Zahl der Zeilen

     mov al,color

@@ZeilestartL:

     mov ecx,linelength

   @@pixlwstrt:
     add [edi], al
     inc edi
     dec ecx
     jnz @@pixlwstrt

     add edi, Spacelength

     dec edx
     jnz @@ZeileStartL
@@endeL:

     ret


rotabar_ endp





; ========================================================
; --------------------------------------------------------
;                   R O T A  B A R  2
; --------------------------------------------------------
; ========================================================


PUBLIC rotabar2_
rotabar2_ Proc near color:byte, min: byte, max: byte
     LOCAL Linelength   :dword
     LOCAL SpaceLength  :dword
     LOCAL LineNum      :dword
     LOCAL LeftR        :word
     LOCALS

     ; X1        Y1        X2        Y2
     ; eax       ebx       ecx       edx

     inc ecx
     inc edx

     mov linelength,ecx
     sub linelength,eax

     push edx
     push ebx
     push eax

     mov eax, _ActiveGraphModeParameters.ScanLineLength

     mov edx, eax
     sub edx, Linelength
     mov SpaceLength, edx

     cmp _ActiveGraphModeParameters.Windowstatus, 100
     jz @@LinearNbar

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; 64 kb Video Window ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

     mul bx
     pop ebx      ; x1

     add ax,bx
     adc dx,0


     mov edi, eax
     add edi, _ActiveGraphModeParameters.LinearAddress

     pop ebx      ; y1
     pop ecx      ; y2
     sub ecx, ebx

     mov LineNum, ecx  ; Zahl der Zeilen


     mov bx, ax        ; bx ist ab jetzt der ZÑhler innerhalb des 64kB Videofesters


     cmp dl,byte ptr _hardwaregraphmodeparameters.actsetpage

     jz @@keinbswtsch

     mov byte ptr _hardwaregraphmodeparameters.pagetoset,dl
     push ebx
     push edx
     SetVirtualPage
     pop edx
     pop ebx

@@keinbswtsch:

     mov edx, LineNum

     mov al,color

@@Zeilestart:

     mov ecx,linelength

   @@pixstart:
     mov ah, [edi]
     add ah, al

     cmp al,0
     jg @@positiv
     cmp ah, min
     jnb @@weiter
     mov ah, min
     jmp @@weiter

   @@positiv:
     cmp ah, max
     jna @@weiter
     mov ah, max

   @@weiter:
     mov [edi], ah

     add di, 1
     jnc @@ksw1
     setnextvirtualpage2
   @@ksw1:
     dec ecx
     jnz @@pixstart

     add di, word ptr Spacelength
     jnc @@KeinBW

     SetNextVirtualPage2

   @@KeinBW:
     dec edx
     jnz @@ZeileStart
@@ende:

     ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@Linearnbar:
     mul ebx
     pop ebx      ; x1

     add eax,ebx

     mov edi, eax
     add edi, _ActiveGraphModeParameters.LinearAddress

     pop ebx      ; y1
     pop edx      ; y2
     sub edx, ebx ; Zahl der Zeilen

     mov al,color

@@ZeilestartL:

     mov ecx,linelength

   @@pixlwstrt:
     mov ah, [edi]
     add ah, al

     cmp al,0
     jg @@lpositiv
     cmp ah, min
     jnb @@lweiter
     mov ah, min
     jmp @@lweiter

   @@lpositiv:
     cmp ah, max
     jna @@lweiter
     mov ah, max

   @@lweiter:
     mov [edi], ah


     inc edi
     dec ecx
     jnz @@pixlwstrt

     add edi, Spacelength

     dec edx
     jnz @@ZeileStartL
@@endeL:

     ret


rotabar2_ endp



; ========================================================
; --------------------------------------------------------
;                    G E T  I M A G E
; --------------------------------------------------------
; ========================================================


PUBLIC  getimage_
getimage_ proc near
     LOCAL LineLength: dword
     LOCAL SpaceLength: dword
     LOCAL LineNum: dword
     LOCAL LeftR: word
     LOCAL Temp: word
     LOCALS


     ; X1        Y1        X2        Y2       BUFFER
     ; eax       ebx       ecx       edx      edi

     inc ecx
     inc edx

     push ecx
     sub ecx, eax
     mov linelength, ecx

     dec ecx
     mov [edi],cx
     add edi,2
     pop ecx


     push edx
     push ebx
     push eax

     mov eax, _ActiveGraphModeParameters.ScanLineLength

     mov edx, eax
     sub edx, Linelength
     mov SpaceLength, edx

     cmp _ActiveGraphModeParameters.Windowstatus, 100
     jz @@LinearGetImage

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; 64 kb Video Window ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

     mul bx
     pop ebx      ; x1

     add ax,bx
     adc dx,0


     mov esi, eax
     add esi, _ActiveGraphModeParameters.LinearAddress

     pop ebx      ; y1
     pop ecx      ; y2
     sub ecx, ebx


     mov LineNum, ecx  ; Zahl der Zeilen
     dec ecx
     mov [edi],cx
     add edi,2


     cmp dl,byte ptr _hardwaregraphmodeparameters.actsetpage

     jz @@keinbswtsch

     mov byte ptr _hardwaregraphmodeparameters.pagetoset,dl
     SetVirtualPage

@@keinbswtsch:

     mov edx, LineNum

@@ZeileStart:


     mov ecx,linelength

     ProcessBytesLW2s copyBytes 

     add si, word ptr Spacelength
     jnc @@KeinBW


     SetNextVirtualPage2

   @@KeinBW:
     dec edx
     jnz @@ZeileStart

     ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@LinearGetImage:
     mul ebx
     pop ebx      ; x1

     add eax,ebx

     mov esi, eax
     add esi, _ActiveGraphModeParameters.LinearAddress

     pop ebx      ; y1
     pop edx      ; y2
     sub edx, ebx ; Zahl der Zeilen

     mov [edi],dx
     dec word ptr [edi]
     add edi,2


@@ZeilestartL:

     mov ecx,linelength

     copyBytes

     add esi, Spacelength

     dec edx
     jnz @@ZeileStartL

     ret
getimage_ endp



; ========================================================
; --------------------------------------------------------
;             P U T  I M A G E  INIT-Makros
; --------------------------------------------------------
; ========================================================


InitPutImage MACRO


     ; X1        Y1        BUFFER
     ; ebx       ecx       eax



     cmp eax,0
     jz @@ende

     mov esi, eax

     lodssw
     cmp ax,16973
     jnz @@NoCompression
     mov compression, 1

     lodssw
 ;   mov Pictsize,ax             uncommend if pictsize will be used

     lodssb
     mov RleByte, al

     lodssw
     jmp @@LLL1

   @@NoCompression:
     mov compression,0


   @@LLL1:
     inc ax
     and eax, 0FFFFh
     mov lineLength,eax

     mov edx,_ActiveGraphModeParameters.ScanLineLength
     sub edx,eax
     mov spaceLength,edx

     lodssw
     inc ax
     mov LineNum, eax

     cmp _ActiveGraphModeParameters.windowstatus, 100

     jz @@LinearPutImage


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; 64 kb Video Window ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


     mov eax,_ActiveGraphModeParameters.ScanLineLength
     mul cx

     add ax,bx
     adc dx,0

     cmp dl,byte ptr _hardwaregraphmodeparameters.actsetpage
     jz @@keinbswtsch

     mov byte ptr _hardwaregraphmodeparameters.pagetoset,dl
     push eax
     SetVirtualPage
     pop eax

  @@keinbswtsch:

     mov edi, eax
     add edi,_ActiveGraphModeParameters.LinearAddress
                          ; Jetzt befindet sich in
                          ; eax : Startoffset im Bildschirm

     mov bx, ax        ; bx ist ab jetzt der ZÑhler innerhalb des 64kB Videofesters

     cmp compression,1
     jz @@WinMitCompression

     mov edx,LineNum


ENDM


; ========================================================
; --------------------------------------------------------
;                    P U T  F I L T E R
; --------------------------------------------------------
; ========================================================


PUBLIC  putfilter_
putfilter_ proc near
     LOCAL LineLength: dword
     LOCAL SpaceLength: dword
     LOCAL LineNum: dword
     LOCAL LeftR: word
     LOCAL compression: byte
     LOCAL rlebyte: byte
     LOCALS


     InitPutImage


@@ZeileStart:

     mov ecx,linelength

     ProcessBytesLW bx FilterBytes  edi

     add edi, Spacelength
     add bx, word ptr Spacelength
     jnc @@KeinBW

     SetNextVirtualPage edi

   @@KeinBW:
     dec edx
     jnz @@ZeileStart

   @@ende:
     ret

; ___________________                     __________________
; ___________________ Mit RLE-Kompression __________________


@@WinMitCompression:

  @@CZeilestart:
     mov edx, LineLength

  @@CByteStart:
     lodssb
     cmp al, RleByte
     jnz @@NoComp

     lodssb
     movzx ecx,al

     lodssb
     sub edx,ecx
     ProcessBytesLW bx FilterBytes2  edi

     jmp @@LLL3

   @@NoComp:
     add [edi],al
     inc edi

     dec edx
     add bx,1
     jc @@Seitenwechsel1

   @@LLL3:
     cmp edx,0
     jg  @@CByteStart


     dec LineNum
     jz @@LLL19
     Add edi,SpaceLength
     add bx,word ptr Spacelength
     jnc @@CZeileStart

     SetNextVirtualPage edi
     jmp @@CZeileStart

   @@LLL19:
     ret


   @@Seitenwechsel1:
      SetNextVirtualPage edi
     jmp @@LLL3


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@LinearPutImage:

     mov eax,_ActiveGraphModeParameters.ScanLineLength
     mul ecx

     add eax,ebx


     mov edi, eax
     add edi,_ActiveGraphModeParameters.LinearAddress
                          ; Jetzt befindet sich in
                          ; eax : Startoffset im Bildschirm

     cmp compression,1
     jz @@LinMitCompression

     mov edx,LineNum

@@LinZeileStart:

     mov ecx,linelength

     FilterBytes

     add edi, Spacelength

     dec edx
     jnz @@LinZeileStart

     ret

; ___________________                     __________________
; ___________________ Mit RLE-Kompression __________________


@@LinMitCompression:

     mov ebx, LineNum
  @@LinCZeilestart:
     mov edx, LineLength

  @@LinCByteStart:
     lodssb
     cmp al, RleByte
     jnz @@LinNoComp

     lodssb
     movzx ecx,al

     lodssb
     sub edx,ecx
     FilterBytes2

     jmp @@LinLLL3

   @@LinNoComp:
     add [edi],al
     inc edi
     dec edx

   @@LinLLL3:
     cmp edx,0
     jg @@LinCByteStart


     dec ebx
     jz @@LinLL19
     Add edi,SpaceLength

     jmp @@LinCZeileStart

   @@LinLL19:
     ret


putfilter_ endp




; ========================================================
; --------------------------------------------------------
;                 P U T  X L A T  F I L T E R
; --------------------------------------------------------
; ========================================================


PUBLIC  putxlatfilter_
putxlatfilter_ proc near
     LOCAL LineLength: dword
     LOCAL SpaceLength: dword
     LOCAL LineNum: dword
     LOCALS





     ; X1        Y1        BUFFER     XLATTabellen
     ; ebx       ecx       esi        edx




     cmp esi,0
     jz @@ende

     push edx

     lodssw

     inc ax
     and eax, 0FFFFh
     mov lineLength,eax

     mov edx,_ActiveGraphModeParameters.ScanLineLength
     sub edx,eax
     mov spaceLength,edx

     lodssw
     inc ax
     mov LineNum, eax

     mov eax,_ActiveGraphModeParameters.ScanLineLength
     mul ecx

     add eax,ebx


     mov edi, eax
     add edi,_ActiveGraphModeParameters.LinearAddress

     mov eax,0
     pop ebx

@@LinZeileStart:

     mov ecx,linelength

@@PixStart:
     mov edx, 0
     mov dh, [esi]
     add edx, ebx

     mov al, [ edi ]
     mov al, [ eax + edx ]

     mov [edi], al

     inc edi
     inc esi
     dec ecx
     jnz @@PixStart

     add edi, Spacelength

     dec linenum
     jnz @@LinZeileStart
   @@ende:
     ret

endp



; ========================================================
; --------------------------------------------------------
;                    P U T  I M A G E
; --------------------------------------------------------
; ========================================================


PUBLIC  putimage_
putimage_ proc near
     LOCAL LineLength: dword
     LOCAL SpaceLength: dword
     LOCAL LineNum: dword
     LOCAL LeftR: word
     LOCAL compression: byte
     LOCAL rlebyte: byte
     LOCALS


     InitPutImage


@@ZeileStart:

     mov ecx,linelength

     ProcessBytesLW bx copyBytes  edi

     add edi, Spacelength
     add bx, word ptr Spacelength
     jnc @@KeinBW

     SetNextVirtualPage edi

   @@KeinBW:
     dec edx
     jnz @@ZeileStart

   @@ende:
     ret

; ___________________                     __________________
; ___________________ Mit RLE-Kompression __________________


@@WinMitCompression:

  @@CZeilestart:
     mov edx, LineLength

  @@CByteStart:
     lodssb
     cmp al, RleByte
     jnz @@NoComp

     lodssb
     movzx ecx,al

     lodssb
     sub edx,ecx
     mov ah,al
     ProcessBytesLW bx SetBytes  edi

     jmp @@LLL3

   @@NoComp:
     stossb
     dec edx
     add bx,1
     jc @@Seitenwechsel1

   @@LLL3:
     cmp edx,0
     jg  @@CByteStart


     dec LineNum
     jz @@LLL19
     Add edi,SpaceLength
     add bx,word ptr Spacelength
     jnc @@CZeileStart

     SetNextVirtualPage edi
     jmp @@CZeileStart

   @@LLL19:
     ret


   @@Seitenwechsel1:
      SetNextVirtualPage edi
     jmp @@LLL3


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@LinearPutImage:

     mov eax,_ActiveGraphModeParameters.ScanLineLength
     mul ecx

     add eax,ebx


     mov edi, eax
     add edi,_ActiveGraphModeParameters.LinearAddress
                          ; Jetzt befindet sich in
                          ; eax : Startoffset im Bildschirm

     cmp compression,1
     jz @@LinMitCompression

     mov edx,LineNum

@@LinZeileStart:

     mov ecx,linelength

     copyBytes

     add edi, Spacelength

     dec edx
     jnz @@LinZeileStart

     ret

; ___________________                     __________________
; ___________________ Mit RLE-Kompression __________________


@@LinMitCompression:

     mov ebx, LineNum
  @@LinCZeilestart:
     mov edx, LineLength

  @@LinCByteStart:
     lodssb
     cmp al, RleByte
     jnz @@LinNoComp

     lodssb
     movzx ecx,al

     lodssb
     sub edx,ecx
     mov ah,al
     SetBytes

     jmp @@LinLLL3

   @@LinNoComp:
     stossb
     dec edx

   @@LinLLL3:
     cmp edx,0
     jg @@LinCByteStart


     dec ebx
     jz @@LinLL19
     Add edi,SpaceLength

     jmp @@LinCZeileStart

   @@LinLL19:
     ret


putimage_ endp


; ========================================================
; --------------------------------------------------------
;              P U T  S P R I T E  I M A G E
; --------------------------------------------------------
; ========================================================


PUBLIC  putspriteimage_
putspriteimage_ proc near
     LOCAL LineLength: dword
     LOCAL SpaceLength: dword
     LOCAL LineNum: dword
     LOCAL LeftR: word
     LOCAL compression: byte
     LOCAL rlebyte: byte
     LOCALS


     Initputimage


@@ZeileStart:

     mov ecx,linelength

     ProcessBytesLW bx copySpriteBytes  edi

     add edi, Spacelength
     add bx, word ptr Spacelength
     jnc @@KeinBW

     SetNextVirtualPage edi

   @@KeinBW:
     dec edx
     jnz @@ZeileStart

   @@ende:
     ret

; ___________________                     __________________
; ___________________ Mit RLE-Kompression __________________


@@WinMitCompression:

  @@CZeilestart:
     mov edx, LineLength

  @@CByteStart:
     lodssb
     cmp al, RleByte
     jnz @@NoComp

     lodssb
     movzx ecx,al

     lodssb
     sub edx,ecx
     cmp al,255
     jz @@SpriteTransparent
     mov ah,al
     ProcessBytesLW bx SetBytes  edi

     jmp @@LLL3

   @@SpriteTransparent:
     add edi,ecx
     add bx,cx
     jc @@Seitenwechsel1
     jmp @@LLL3


   @@NoComp:
     cmp al,255
     jz @@TransByte
     mov [edi],al
   @@TransByte:
     inc edi

     dec edx
     add bx,1
     jc @@Seitenwechsel1

   @@LLL3:
     cmp edx,0
     jg  @@CByteStart


     dec LineNum
     jz @@LLL19
     Add edi,SpaceLength
     add bx,word ptr Spacelength
     jnc @@CZeileStart

     SetNextVirtualPage edi
     jmp @@CZeileStart

   @@LLL19:
     ret


   @@Seitenwechsel1:
      SetNextVirtualPage edi
     jmp @@LLL3


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@Linearputimage:

     mov eax,_ActiveGraphModeParameters.ScanLineLength
     mul ecx

     add eax,ebx


     mov edi, eax
     add edi,_ActiveGraphModeParameters.LinearAddress
                          ; Jetzt befindet sich in
                          ; eax : Startoffset im Bildschirm

     cmp compression,1
     jz @@LinMitCompression

     mov edx,LineNum

@@LinZeileStart:

     mov ecx,linelength

     copySpriteBytes

     add edi, Spacelength

     dec edx
     jnz @@LinZeileStart

     ret

; ___________________                     __________________
; ___________________ Mit RLE-Kompression __________________


@@LinMitCompression:

     mov ebx, LineNum
  @@LinCZeilestart:
     mov edx, LineLength

  @@LinCByteStart:
     lodssb
     cmp al, RleByte
     jnz @@LinNoComp

     lodssb
     movzx ecx,al

     lodssb
     sub edx,ecx

     cmp al,255
     jz @@LinSpriteTransparent
     mov ah,al
     SetBytes

     jmp @@LinLLL3

   @@LinSpriteTransparent:
     add edi,ecx
     jmp @@LinLLL3


   @@LinNoComp:
     cmp al,255
     jz @@SingleTransparentByte
     mov [edi],al

   @@SingleTransparentByte:
     inc edi
     dec edx

   @@LinLLL3:
     cmp edx,0
     jg @@LinCByteStart


     dec ebx
     jz @@LinLL19
     Add edi,SpaceLength

     jmp @@LinCZeileStart

   @@LinLL19:
     ret


putspriteimage_ endp



comment ˝

; ========================================================
; --------------------------------------------------------
;         P U T  S P R I T E  I M A G E  H A L F
; --------------------------------------------------------
; ========================================================


PUBLIC  putspriteimagehalf_
putspriteimagehalf_ proc near
     LOCAL LineLength: dword
     LOCAL SpaceLength: dword
     LOCAL LineNum: dword
     LOCAL LeftR: word
     LOCAL compression: byte
     LOCAL rlebyte: byte
     LOCALS


     Initputimage


@@ZeileStart:

     mov ecx,linelength
     shr ecx,1

     ProcessBytesLW bx copySpriteBytes  edi

     add edi, Spacelength
     add bx, word ptr Spacelength
     jnc @@KeinBW

     SetNextVirtualPage edi

   @@KeinBW:
     dec edx
     jnz @@ZeileStart

   @@ende:
     ret

; ___________________                     __________________
; ___________________ Mit RLE-Kompression __________________


@@WinMitCompression:

  @@CZeilestart:
     mov edx, LineLength

  @@CByteStart:
     lodssb
     cmp al, RleByte
     jnz @@NoComp

     lodssb
     movzx ecx,al

     lodssb
     sub edx,ecx
     cmp al,255
     jz @@SpriteTransparent
     mov ah,al
     ProcessBytesLW bx SetBytes  edi

     jmp @@LLL3

   @@SpriteTransparent:
     add edi,ecx
     add bx,cx
     jc @@Seitenwechsel1
     jmp @@LLL3


   @@NoComp:
     cmp al,255
     jz @@TransByte
     mov [edi],al
   @@TransByte:
     inc edi

     dec edx
     add bx,1
     jc @@Seitenwechsel1

   @@LLL3:
     cmp edx,0
     jg  @@CByteStart


     dec LineNum
     jz @@LLL19
     Add edi,SpaceLength
     add bx,word ptr Spacelength
     jnc @@CZeileStart

     SetNextVirtualPage edi
     jmp @@CZeileStart

   @@LLL19:
     ret


   @@Seitenwechsel1:
      SetNextVirtualPage edi
     jmp @@LLL3


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@Linearputimage:

     mov eax,_ActiveGraphModeParameters.ScanLineLength
     mul ecx

     add eax,ebx


     mov edi, eax
     add edi,_ActiveGraphModeParameters.LinearAddress
                          ; Jetzt befindet sich in
                          ; eax : Startoffset im Bildschirm

     cmp compression,1
     jz @@LinMitCompression

     mov edx,LineNum

@@LinZeileStart:

     mov ecx,linelength

     copySpriteBytes

     add edi, Spacelength

     dec edx
     jnz @@LinZeileStart

     ret

; ___________________                     __________________
; ___________________ Mit RLE-Kompression __________________


@@LinMitCompression:

     mov ebx, LineNum
  @@LinCZeilestart:
     mov edx, LineLength

  @@LinCByteStart:
     lodssb
     cmp al, RleByte
     jnz @@LinNoComp

     lodssb
     movzx ecx,al

     lodssb
     sub edx,ecx

     cmp al,255
     jz @@LinSpriteTransparent
     mov ah,al
     SetBytes

     jmp @@LinLLL3

   @@LinSpriteTransparent:
     add edi,ecx
     jmp @@LinLLL3


   @@LinNoComp:
     cmp al,255
     jz @@SingleTransparentByte
     mov [edi],al

   @@SingleTransparentByte:
     inc edi
     dec edx

   @@LinLLL3:
     cmp edx,0
     jg @@LinCByteStart


     dec ebx
     jz @@LinLL19
     Add edi,SpaceLength

     jmp @@LinCZeileStart

   @@LinLL19:
     ret


putspriteimagehalf_ endp

˝

; ========================================================
; --------------------------------------------------------
;          P U T  R O T  S P R I T E  I M A G E
; --------------------------------------------------------
; ========================================================


PUBLIC  putrotspriteimage_
putrotspriteimage_ proc near
     LOCAL LineLength: dword
     LOCAL SpaceLength: dword
     LOCAL LineNum: dword
     LOCAL LeftR: word
     LOCAL compression: byte
     LOCAL rlebyte: byte
     LOCAL rotationValue: byte
     LOCALS

     ; X1        Y1        BUFFER   rotationvalue
     ; ebx       ecx       eax           edx

     mov rotationValue,dl

     Initputimage


@@ZeileStart:

     mov ecx,linelength

     ProcessBytesLW bx copyRotSpriteBytes  edi

     add edi, Spacelength
     add bx, word ptr Spacelength
     jnc @@KeinBW

     SetNextVirtualPage edi

   @@KeinBW:
     dec edx
     jnz @@ZeileStart

   @@ende:
     ret

; ___________________                     __________________
; ___________________ Mit RLE-Kompression __________________


@@WinMitCompression:

  @@CZeilestart:
     mov edx, LineLength

  @@CByteStart:
     lodssb
     cmp al, RleByte
     jnz @@NoComp

     lodssb
     movzx ecx,al

     lodssb
     sub edx,ecx
     cmp al,255
     jz @@SpriteTransparent
     RotateByte
     mov ah,al
     ProcessBytesLW bx SetBytes  edi

     jmp @@LLL3

   @@SpriteTransparent:
     add edi,ecx
     add bx,cx
     jc @@Seitenwechsel1
     jmp @@LLL3


   @@NoComp:
     cmp al,255
     jz @@TransByte
     rotatebyte
     mov [edi],al

   @@TransByte:
     inc edi

     dec edx
     add bx,1
     jc @@Seitenwechsel1

   @@LLL3:
     cmp edx,0
     jg  @@CByteStart


     dec LineNum
     jz @@LLL19
     Add edi,SpaceLength
     add bx,word ptr Spacelength
     jnc @@CZeileStart

     SetNextVirtualPage edi
     jmp @@CZeileStart

   @@LLL19:
     ret


   @@Seitenwechsel1:
      SetNextVirtualPage edi
     jmp @@LLL3


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@Linearputimage:

     mov eax,_ActiveGraphModeParameters.ScanLineLength
     mul ecx

     add eax,ebx


     mov edi, eax
     add edi,_ActiveGraphModeParameters.LinearAddress
                          ; Jetzt befindet sich in
                          ; eax : Startoffset im Bildschirm

     cmp compression,1
     jz @@LinMitCompression

     mov edx,LineNum

@@LinZeileStart:

     mov ecx,linelength

     copyRotSpriteBytes

     add edi, Spacelength

     dec edx
     jnz @@LinZeileStart

     ret

; ___________________                     __________________
; ___________________ Mit RLE-Kompression __________________


@@LinMitCompression:

     mov ebx, LineNum
  @@LinCZeilestart:
     mov edx, LineLength

  @@LinCByteStart:
     lodssb
     cmp al, RleByte
     jnz @@LinNoComp

     lodssb
     movzx ecx,al

     lodssb
     sub edx,ecx

     cmp al,255
     jz @@LinSpriteTransparent

     rotateByte
     mov ah,al
     SetBytes

     jmp @@LinLLL3

   @@LinSpriteTransparent:
     add edi,ecx
     jmp @@LinLLL3


   @@LinNoComp:
     cmp al,255
     jz @@transparentSingleByte
     rotatebyte
     mov [edi],al

   @@transparentSingleByte:
     inc edi
     dec edx

   @@LinLLL3:
     cmp edx,0
     jg @@LinCByteStart


     dec ebx
     jz @@LinLL19
     Add edi,SpaceLength

     jmp @@LinCZeileStart

   @@LinLL19:
     ret


putrotspriteimage_ endp



; ========================================================
; --------------------------------------------------------
;        P U T  R O T  S P R I T E  I M A G E  9 0
; --------------------------------------------------------
; ========================================================

PUBLIC putrotspriteimage90_
putrotspriteimage90_ PROC near
     LOCAL LineLength: dword
     LOCAL SpaceLength: dword
     LOCAL LineNum: dword
     LOCAL LeftR: word
     LOCAL rotationValue: byte
     LOCAL sourceOffsd: dword
     LOCALS

     ; X1        Y1        BUFFER   rotationvalue
     ; ebx       ecx       eax           edx

     mov rotationValue,dl

     cmp eax,0
     jz @@ende

     mov esi, eax

     lodssw
     cmp ax,16973
     jz @@Ende

     inc ax
     and eax, 0FFFFh
     mov lineNum,eax

     lodssw
     inc ax
     mov LineLength, eax

     mov edx,_ActiveGraphModeParameters.ScanLineLength
     sub edx,eax
     mov spaceLength,edx


     cmp _ActiveGraphModeParameters.windowstatus, 100

     jz @@LinearFrameBuffer


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; 64 kb Video Window ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


     mov eax,_ActiveGraphModeParameters.ScanLineLength
     mul cx

     add ax,bx
     adc dx,0

     cmp dl,byte ptr _hardwaregraphmodeparameters.actsetpage
     jz @@keinbswtsch

     mov byte ptr _hardwaregraphmodeparameters.pagetoset,dl
     push eax
     SetVirtualPage
     pop eax

  @@keinbswtsch:

     mov edi, eax
     add edi,_ActiveGraphModeParameters.LinearAddress
                          ; Jetzt befindet sich in
                          ; eax : Startoffset im Bildschirm

     mov bx, ax        ; bx ist ab jetzt der ZÑhler innerhalb des 64kB Videofesters



     mov eax,LineLength
     mul LineNum
     dec eax
     mov sourceOffsd,eax

     mov edx,LineNum

  @@Zeilestart:        ; ---------------
     add esi,SourceOffsd
     inc esi

     mov ecx,LineLength
   @@LLL6:
     sub esi,lineNum
     ;dec si

     mov al,[esi]
     cmp al,255
     jz @@LLL7

     RotateByte
     mov [edi],al

   @@LLL7:
     inc edi
     add bx,1
     jc @@Seitenwechsel2
     @@SWF2:

     dec cx
     jnz @@LLL6

     add edi,spacelength
     add bx,word ptr spacelength
     jc @@Seitenwechsel3
     @@SWF3:

     inc esi
     dec edx
     jnz @@Zeilestart

@@Ende:
     ret

   @@Seitenwechsel2:
      SetNextVirtualPage edi
     jmp @@SWF2
   @@Seitenwechsel3:
      SetNextVirtualPage edi
     jmp @@SWF3

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@LinearFrameBuffer:
     mov eax,_ActiveGraphModeParameters.ScanLineLength
     mul ecx

     add eax,ebx
     add eax,_ActiveGraphModeParameters.LinearAddress

     mov edi, eax

     mov eax,LineLength
     mul LineNum
     dec eax
     mov sourceOffsd,eax

     mov edx,LineNum


  @@LZeilestart:
     add esi,SourceOffsd
     inc esi

     mov ecx,LineLength
   @@LLLL6:
     sub esi,lineNum
     ;dec si

     mov al,[esi]
     cmp al,255
     jz @@LLLL7

     RotateByte
     mov [edi],al

   @@LLLL7:
     inc edi

     dec cx
     jnz @@LLLL6

     add edi,spacelength

     inc esi
     dec edx
     jnz @@LZeilestart

@@LEnde:
     ret


putrotspriteimage90_ endp



; ========================================================
; --------------------------------------------------------
;       P U T  R O T  S P R I T E  I M A G E  2 7 0
; --------------------------------------------------------
; ========================================================


PUBLIC putrotspriteimage270_
putrotspriteimage270_ PROC near
     LOCAL LineLength: dword
     LOCAL SpaceLength: dword
     LOCAL LineNum: dword
     LOCAL LeftR: word
     LOCAL rotationValue: byte
     LOCAL sourceOffsd: dword
     LOCALS

     ; X1        Y1        BUFFER   rotationvalue
     ; ebx       ecx       eax           edx

     mov rotationValue,dl

     cmp eax,0
     jz @@ende

     mov esi, eax

     lodssw
     cmp ax,16973
     jz @@Ende

     inc ax
     and eax, 0FFFFh
     mov lineNum,eax

     lodssw
     inc ax
     mov LineLength, eax

     mov edx,_ActiveGraphModeParameters.ScanLineLength
     sub edx,eax
     mov spaceLength,edx

     cmp _ActiveGraphModeParameters.windowstatus, 100

     jz @@LinearFrameBuffer


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; 64 kb Video Window ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


     mov eax,_ActiveGraphModeParameters.ScanLineLength
     mul cx

     add ax,bx
     adc dx,0

     cmp dl,byte ptr _hardwaregraphmodeparameters.actsetpage
     jz @@keinbswtsch

     mov byte ptr _hardwaregraphmodeparameters.pagetoset,dl
     push eax
     SetVirtualPage
     pop eax

  @@keinbswtsch:

     mov edi, eax
     add edi,_ActiveGraphModeParameters.LinearAddress
                          ; Jetzt befindet sich in
                          ; eax : Startoffset im Bildschirm

     mov bx, ax        ; bx ist ab jetzt der ZÑhler innerhalb des 64kB Videofesters



     mov eax,LineLength
     mul LineNum

     inc eax
     mov sourceOffsd,eax

     mov edx,LineNum

     dec esi


  @@Zeilestart:       ; -------------------------

     mov ecx,LineLength
   @@LLL6:
     add esi,lineNum

     mov al,[esi]
     cmp al,255
     jz @@LLL7
     RotateByte

     mov [edi],al
   @@LLL7:
     inc edi
     add bx,1
     jc @@Seitenwechsel2
     @@SWF2:

     dec cx
     jnz @@LLL6


     add edi,spacelength
     add bx, word ptr spacelength
     jc @@Seitenwechsel3
     @@SWF3:

     sub esi,sourceoffsd
     dec edx
     jnz @@Zeilestart

@@Ende:
     ret


   @@Seitenwechsel2:
      SetNextVirtualPage edi
     jmp @@SWF2

   @@Seitenwechsel3:
      SetNextVirtualPage edi
     jmp @@SWF3



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@LinearFrameBuffer:

     mov eax,_ActiveGraphModeParameters.ScanLineLength
     mul ecx

     add eax,ebx
     add eax,_ActiveGraphModeParameters.LinearAddress

     mov edi, eax


     mov eax,LineLength
     mul LineNum
     inc eax
     mov sourceOffsd,eax

     mov edx,LineNum

     dec esi


  @@LZeilestart:       ; -------------------------

     mov ecx,LineLength
   @@LLLL6:
     add esi,lineNum

     mov al,[esi]
     cmp al,255
     jz @@LLLL7
     RotateByte

     mov [edi],al
   @@LLLL7:
     inc edi

     dec cx
     jnz @@LLLL6


     add edi,spacelength

     sub esi,sourceoffsd
     dec edx
     jnz @@LZeilestart

@@LEnde:
     ret

PutRotSpriteImage270_ endp




; ========================================================
; --------------------------------------------------------
;       P U T  R O T  S P R I T E  I M A G E  1 8 0
; --------------------------------------------------------
; ========================================================


PUBLIC  putrotspriteimage180_
putrotspriteimage180_ proc near
     LOCAL LineLength: dword
     LOCAL SpaceLength: dword
     LOCAL LineNum: dword
     LOCAL LeftR: word
     LOCAL compression: byte
     LOCAL rlebyte: byte
     LOCAL rotationValue: byte
     LOCALS

     ; X1        Y1        BUFFER   rotationvalue
     ; ebx       ecx       eax           edx

     mov rotationValue,dl

     Initputimage


     mov eax,linelength
     mul linenum
     add esi,eax

     mov edx, LineNum

@@ZeileStart:

     mov ecx,linelength

   @@ByteStart:
     dec esi
     mov al,[esi]

     cmp al,255
     jz @@TransByte

     RotateByte

     mov [edi],al
   @@TransByte:
     inc edi
     add bx,1
     jnc @@Nobswtch
     SetNextVirtualPage edi
   @@Nobswtch:

     dec ecx
     jnz @@ByteStart

     add edi, Spacelength
     add bx, word ptr Spacelength
     jnc @@KeinBW

     SetNextVirtualPage edi

   @@KeinBW:
     dec edx
     jnz @@ZeileStart

 @@WinMitCompression:
   @@ende:
     ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@Linearputimage:

     mov eax,_ActiveGraphModeParameters.ScanLineLength
     mul ecx

     add eax,ebx


     mov edi, eax
     add edi,_ActiveGraphModeParameters.LinearAddress
                          ; Jetzt befindet sich in
                          ; eax : Startoffset im Bildschirm

     cmp compression,1
     jz @@LinMitCompression


     mov eax,linelength
     mul linenum
     add esi,eax

     mov edx, LineNum

@@LZeileStart:

     mov ecx,linelength

  @@LbyteStart:
     dec esi
     mov al,[esi]

     cmp al,255
     jz @@LTransByte

     RotateByte

     mov [edi],al
   @@LTransByte:
     inc edi

     dec ecx
     jnz @@LByteStart

     add edi, Spacelength

     dec edx
     jnz @@LZeileStart

 @@LinMitCompression:
     ret


putrotspriteimage180_ endp





; ========================================================
; --------------------------------------------------------
;                  P U T  T E X T U R E 
; --------------------------------------------------------
; ========================================================


PUBLIC  puttexture_
puttexture_ proc near
     LOCAL LineLength: dword
     LOCAL SpaceLength: dword
     LOCAL LineNum: dword
     LOCAL LeftR: word
     LOCAL compression: byte
     LOCAL rlebyte: byte
     LOCAL temp: word


     ; X1        Y1       X2      Y2       BUFFER
     ; eax       ebx      ecx     edx      esi

     push es
     push ds
     pop es


     inc ecx
     inc edx
     

     push ecx
     sub ecx, eax
     mov linelength, ecx

     pop ecx


     push edx
     push ebx
     push eax

     mov eax, _ActiveGraphModeParameters.ScanLineLength

     mov edx, eax
     sub edx, Linelength
     mov SpaceLength, edx

     cmp _ActiveGraphModeParameters.Windowstatus, 100
     jz @@LinearGetImage

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; 64 kb Video Window ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

     mul ebx
     pop ebx      ; x1

     add eax,ebx
     
     add esi,eax   
     


     pop ebx      ; y1
     pop ecx      ; y2
     sub ecx, ebx


     mov LineNum, ecx  ; Zahl der Zeilen

     mov ebx,0
     mov bx,ax
     
     mov edi, ebx
     add edi, _ActiveGraphModeParameters.LinearAddress
     
     shr eax,16
     mov dl,al

     cmp dl,byte ptr _hardwaregraphmodeparameters.actsetpage

     jz @@keinbswtsch

     mov byte ptr _hardwaregraphmodeparameters.pagetoset,dl
     SetVirtualPage

@@keinbswtsch:

     mov edx, LineNum

@@ZeileStart:


     mov ecx,linelength

     ProcessBytesLW2 copyBytes

     add esi, spacelength 
     add di, word ptr Spacelength
     jnc @@KeinBW


     SetNextVirtualPage2

   @@KeinBW:
     dec edx
     jnz @@ZeileStart

     pop es	
     ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@LinearGetImage:
     mul ebx
     pop ebx      ; x1

     add eax,ebx
    
     add esi, eax
     mov edi, eax
     add edi, _ActiveGraphModeParameters.LinearAddress

     pop ebx      ; y1
     pop edx      ; y2
     sub edx, ebx ; Zahl der Zeilen


@@ZeilestartL:

     mov ecx,linelength

     copyBytes

     add esi, Spacelength
     add edi, spacelength

     dec edx
     jnz @@ZeileStartL

     pop es	
     ret
endp







; ========================================================
; --------------------------------------------------------
;             P U T  S P R I T E  T E X T U R E
; --------------------------------------------------------
; ========================================================


PUBLIC  putspritetexture_
putspritetexture_ proc near
     LOCAL LineLength: dword
     LOCAL SpaceLength: dword
     LOCAL LineNum: dword
     LOCAL LeftR: word
     LOCAL compression: byte
     LOCAL rlebyte: byte
     LOCAL temp: word


     ; X1        Y1       X2      Y2       BUFFER
     ; eax       ebx      ecx     edx      esi

     push es
     push ds
     pop es


     inc ecx
     inc edx


     push ecx
     sub ecx, eax
     mov linelength, ecx

     pop ecx


     push edx
     push ebx
     push eax

     mov eax, _ActiveGraphModeParameters.ScanLineLength

     mov edx, eax
     sub edx, Linelength
     mov SpaceLength, edx

     cmp _ActiveGraphModeParameters.Windowstatus, 100
     jz @@LinearGetImage

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; 64 kb Video Window ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

     mul ebx
     pop ebx      ; x1

     add eax,ebx
     
     add esi,eax   
     


     pop ebx      ; y1
     pop ecx      ; y2
     sub ecx, ebx


     mov LineNum, ecx  ; Zahl der Zeilen

     mov ebx,0
     mov bx,ax

     mov edi, ebx
     add edi, _ActiveGraphModeParameters.LinearAddress

     shr eax,16
     mov dl,al

     cmp dl,byte ptr _hardwaregraphmodeparameters.actsetpage

     jz @@keinbswtsch

     mov byte ptr _hardwaregraphmodeparameters.pagetoset,dl
     SetVirtualPage

@@keinbswtsch:

     mov edx, LineNum

@@ZeileStart:


     mov ecx,linelength

     ProcessBytesLW2 copySpriteBytes

     add esi, spacelength
     add di, word ptr Spacelength
     jnc @@KeinBW


     SetNextVirtualPage2

   @@KeinBW:
     dec edx
     jnz @@ZeileStart

     pop es
     ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@LinearGetImage:
     mul ebx
     pop ebx      ; x1

     add eax,ebx

     add esi, eax
     mov edi, eax
     add edi, _ActiveGraphModeParameters.LinearAddress

     pop ebx      ; y1
     pop edx      ; y2
     sub edx, ebx ; Zahl der Zeilen


@@ZeilestartL:

     mov ecx,linelength

     copySpriteBytes

     add esi, Spacelength
     add edi, spacelength

     dec edx
     jnz @@ZeileStartL

     pop es
     ret
endp








; ========================================================
; --------------------------------------------------------
;                    P U T  P I X E L
; --------------------------------------------------------
; ========================================================


PUBLIC putpixel8_
putpixel8_ proc near
     LOCALS

     ;    X           Y         color
     ;   ebx         eax         edx

     push edx

     mov ecx, _ActiveGraphModeParameters.ScanLineLength

     cmp _ActiveGraphModeParameters.WindowStatus, 100
     jz @@LinearPutPixel

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; 64 kb Video Window ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

     mul cx
     add ax,bx
     adc dx,0
     cmp dl, byte ptr _hardwaregraphmodeparameters.actsetpage

     push eax
     jz @@keinbswtsch

     mov byte ptr _hardwaregraphmodeparameters.pagetoset,dl
     SetVirtualPage

@@keinbswtsch:
     pop ebx
     add ebx,_ActiveGraphModeParameters.LinearAddress
     pop eax
     mov [ebx],al

     ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@LinearPutPixel:
     imul ecx
     add eax,ebx
     add eax,_ActiveGraphModeParameters.LinearAddress
     mov ebx,eax

     pop eax
     mov [ebx],al

     ret

endp



; ========================================================
; --------------------------------------------------------
;                    G E T  P I X E L
; --------------------------------------------------------
; ========================================================


PUBLIC getpixel8_
getpixel8_ proc near
      LOCALS
     ;    X           Y
     ;   ebx         eax

     mov ecx, _ActiveGraphModeParameters.ScanLineLength

     cmp _ActiveGraphModeParameters.WindowStatus, 100
     jz @@LinearPutPixel

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; 64 kb Video Window ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

     mul cx
     add ax,bx
     adc dx,0
     cmp dl, byte ptr _hardwaregraphmodeparameters.actsetpage

     push eax
     jz @@keinbswtsch

     mov byte ptr _hardwaregraphmodeparameters.pagetoset,dl
     SetVirtualPage

@@keinbswtsch:
     pop ebx
     add ebx,_ActiveGraphModeParameters.LinearAddress
     movzx eax, byte ptr [ebx]

     ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@LinearPutPixel:
     mul ecx
     add eax,ebx
     add eax,_ActiveGraphModeParameters.LinearAddress
     mov ebx,eax

     movzx eax, byte ptr [ebx]

     ret

endp


PUBLIC copybuf2displaymemory_
copybuf2displaymemory_ proc near
   push es
   
   mov esi, eax
   mov edi, _Activegraphmodeparameters.linearaddress

   cmp _hardwaregraphmodeparameters.windowstatus,100
   jz @@linearframebuf
   

   cmp _hardwaregraphmodeparameters.actsetpage,0
   jz @@nobswt

   mov _hardwaregraphmodeparameters.pagetoset,0
   push ebx

   SetVirtualPage

   pop ebx


 @@nobswt:
   mov ax, ds
   mov es,ax

 @@loopstart:
   cmp ebx,0FFFFh
   ja @@c64kb

   mov ecx,ebx
   shr ecx,2
   rep movsd
   and ebx,3
   mov ecx,ebx
   rep movsb

   jmp @@ende



 @@c64kb:
   sub ebx, 10000h
   mov ecx,  4000h
   rep movsd
   setnextvirtualpage2
   sub edi, 10000h
   jmp @@loopstart

 @@linearFramebuf:
   mov ecx,ebx
   shr ecx,2
   rep movsd
   and ebx,3
   mov ecx,ebx
   rep movsb
 

 @@ende:
   pop es
   ret
endp


PUBLIC set_vgapalette256_
set_vgapalette256_ proc near
  mov esi, eax
  mov ecx,768
  mov ebx,0
  mov edx,3C8h
  mov al,0
  out dx,al
  inc edx
@@loopstart:
  mov al,[esi]
  inc esi
  cmp al,255
  jz @@DontSetNew
  mov [_ActivePalette + ebx],al
  jmp @@SetVal
@@DontSetNew:
  mov al, [_ActivePalette + ebx]
@@SetVal:
  out dx, al
  inc ebx
  dec ecx
  jnz @@loopstart
  ret
endp

public waitretrace_
waitretrace_ proc near
                MOV DX,3DAH
 @@VS1:
                IN AL,DX
                TEST AL,8
                JZ @@VS1
 @@VS2:
                IN AL,DX
                TEST AL,8
                JNZ @@VS2
                ret
endp

public controldpms_
controldpms_ proc near
        local tmpvar : byte;
        mov ax, 4F10h
        mov bl, 1
        mov tmpvar, bh
        int 10h
        cmp ax, 4Fh
        jnz @@ende
        movzx ebx, tmpvar
        mov _actdpmsmode, ebx
     @@ende:
        ret
endp

public getdpmscapabilities_
getdpmscapabilities_ proc near
        mov ax, 4F10h
        mov bx, 0
        mov es, bx
        mov di, bx

        int 10h
        cmp ax, 4Fh
        jnz @@ende
        and ebx, 0FFFFh
        mov _dpmscapabilities, ebx
     @@ende:
        ret
endp



PUBLIC xlatpict_
XlatPict_ Proc near
  locals

  mov eax, _XlatBuffer
  mov edi, eax

  mov ax, [esi]
  stossw
  add esi,2

  cmp ax, 16973

  jnz @Norle

  mov ax, [esi]  ; Size
  stossw
  mov cx, ax
  add esi,2

  mov al, [esi]  ; RLE-Byte
  inc esi
  stossb
  mov dh, al

  mov eax, [esi]  ; X-Size, Y-Size
  stosd
  add esi,4


@rleLoopStart:

  mov al, [esi]
  inc esi
  cmp al,dh
  jz @SetRleByte
  xlatb
  stossb
  jmp @loopdec

@setrleByte:
  stossb
  mov al, [esi]    ; Count-Byte
  inc esi
  dec cx
  stossb

@loopdec:
  dec cx
  jnz @rleLoopStart

  jmp @@ende

; ////////////////// NORLE \\\\\\\\\\\\\\\\\\

@Norle:
  inc ax
  mov cx,ax

  mov ax, [esi]
  stossw
  add esi,2
  inc ax

  mul cx
  mov cx,ax

@NoRleloopStart:
  mov al, [esi]
  inc esi
  xlat
  stossb

  dec cx
  jnz @NoRleloopStart

@@ende:

  mov eax, _XlatBuffer

  ret

endp


PUBLIC loga2_
loga2_ proc near

  bsf eax,ebx
  jz @@ende
  ret
  @@ende:
  mov eax,0
  ret
endp


; ========================================================
; --------------------------------------------------------
;                  P U T  I M A G E  P R T 
; --------------------------------------------------------
; ========================================================


PUBLIC putimageprt_
putimageprt_ proc near imgx1 : dword , imgy1 : dword   

     LOCAL LineLength: dword
     LOCAL SpaceLength: dword
     LOCAL ImgSpaceLength: dword
     LOCAL LineNum: dword
     LOCAL LeftR: word
     LOCAL compression: byte
     LOCAL rlebyte: byte
     LOCAL temp: word
     LOCAL pictheight : word


     ; X1        Y1       X2      Y2       BUFFER
     ; eax       ebx      ecx     edx      esi

     push es
     push ds
     pop es


     push eax
     push ebx
     push ecx
     push edx

     mov ax, [esi+2]
     mov pictheight, ax

     mov ax, [esi]
     inc ax
     and eax, 0FFFFh
     mov imgspacelength, eax   ; die X-Grî·e des Images

     mov ebx, imgy1
     mul ebx

     add eax, imgx1

     add esi, 4
     add esi, eax


     pop edx
     pop ecx
     pop ebx
     pop eax




     inc ecx
     inc edx


     push ecx
     sub ecx, eax
     cmp ecx, imgspacelength
     jb @@wwww01
     mov ecx, imgspacelength

   @@wwww01:
     mov linelength, ecx
     sub imgspacelength, ecx

     pop ecx


     push edx
     push ebx
     push eax

     mov eax, _ActiveGraphModeParameters.ScanLineLength

     mov edx, eax
     sub edx, Linelength
     mov SpaceLength, edx

     cmp _ActiveGraphModeParameters.Windowstatus, 100
     jz @@LinearGetImage

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; 64 kb Video Window ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

     mul ebx
     pop ebx      ; x1

     add eax,ebx




     pop ebx      ; y1
     pop ecx      ; y2
     sub ecx, ebx


     cmp cx, pictheight
     jb @@wwww02
     mov ecx,0
     mov cx,pictheight
   @@wwww02:

     mov LineNum, ecx  ; Zahl der Zeilen

     mov ebx,0
     mov bx,ax

     mov edi, ebx
     add edi, _ActiveGraphModeParameters.LinearAddress

     shr eax,16
     mov dl,al

     cmp dl,byte ptr _hardwaregraphmodeparameters.actsetpage

     jz @@keinbswtsch

     mov byte ptr _hardwaregraphmodeparameters.pagetoset,dl
     SetVirtualPage

@@keinbswtsch:

     mov edx, LineNum

@@ZeileStart:


     mov ecx,linelength

     ProcessBytesLW2 copyBytes

     add esi, Imgspacelength 
     add di, word ptr Spacelength
     jnc @@KeinBW



     SetNextVirtualPage2

   @@KeinBW:
     dec edx
     jnz @@ZeileStart

     pop es
     leave
     ret 8


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@LinearGetImage:
     mul ebx
     pop ebx      ; x1

     add eax,ebx
    
     mov edi, eax
     add edi, _ActiveGraphModeParameters.LinearAddress



     pop ebx      ; y1
     pop edx      ; y2
     sub edx, ebx ; Zahl der Zeilen
     cmp dx, pictheight
     jb @@wwww03
     mov edx,0
     mov dx,pictheight
   @@wwww03:


@@ZeilestartL:

     mov ecx,linelength

     copyBytes

     add esi, imgSpacelength
     add edi, spacelength

     dec edx
     jnz @@ZeileStartL

     pop es
     leave
     ret 8


endp

Public generatemixbuf_
generatemixbuf_ proc near
          ; Parameter:
	  ; ESI: Puffer
     push ebp	  
	  
     mov edi,0
   lpstart:
   
     mov ebx,edi
     shr ebx,4
     mov bh,bl
     mov [esi+ebx],ebx

     mov ebp,edi
     add ebp,16

   lp2start:
     mov eax,[_Palette16 + edi]
     add eax,[_Palette16 + ebp]
     mov red, eax
     
     mov eax,[_Palette16 + edi + 4]
     add eax,[_Palette16 + ebp + 4]
     mov green, eax
     
     mov eax,[_Palette16 + edi + 8]
     add eax,[_Palette16 + ebp + 8]
     mov blue, eax

     mov ebx,0
     mov eax,0FFFFFFFFh
     		       
   colstart:  
     mov ecx,[_Palette16 + ebx]
     shl ecx,1
     sub ecx, red
     imul ecx,ecx
     mov edx, ecx
     
     mov ecx,[_Palette16 + ebx + 4]
     shl ecx,1
     sub ecx, green
     imul ecx,ecx
     add edx, ecx
     
     mov ecx,[_Palette16 + ebx + 8]
     shl ecx,1
     sub ecx, blue
     imul ecx,ecx
     add edx, ecx
     jz direccolfound
     
     cmp edx, eax
     ja kt
     mov eax, edx
     mov actcol, ebx
     
   kt:
     add ebx,16
     cmp ebx, 4096
     jb  colstart

   colfound:  
     mov ebx,edi
     shr ebx,4
     mov ecx, ebp
     shl ecx, 4
     add ebx, ecx
     
     mov eax, actcol
     shr eax,4
     mov [esi+ebx],al
     xchg bh,bl
     mov [esi+ebx],al

     add ebp, 16
     cmp ebp, 4096
     jb  lp2start

     add edi,16
     cmp edi, 4096
     jb  lpstart
     
     pop ebp
     ret
     
direccolfound:
     mov actcol, ebx
     jmp colfound
     
     
endp     

comment #
Public mix3colors_
mix3colors_ proc near
          ; Parameter:
	  ; ESI: Puffer
	  
     shl edi,4
     shl ebx,4
     shl esi,4
     
     mov eax,[_Palette16 + edi]
     add eax,[_Palette16 + ebx]
     add eax,[_Palette16 + esi]
     mov red, eax
     
     mov eax,[_Palette16 + edi + 4]
     add eax,[_Palette16 + ebx + 4]
     add eax,[_Palette16 + esi + 4]
     mov green, eax
     
     mov eax,[_Palette16 + edi + 8]
     add eax,[_Palette16 + ebx + 8]
     add eax,[_Palette16 + esi + 8]
     mov blue, eax

     mov ebx,0      ; Farbindex - zÑhler
     mov eax,0FFFFFFFFh
     		       
   @@colstart:  
     mov ecx,[_Palette16 + ebx]
     imul ecx,3
     sub ecx, red
     imul ecx,ecx
     mov edx, ecx
     
     mov ecx,[_Palette16 + ebx + 4]
     imul ecx,3
     sub ecx, green
     imul ecx,ecx
     add edx, ecx
     
     mov ecx,[_Palette16 + ebx + 8]
     imul ecx,3
     sub ecx, blue
     imul ecx,ecx
     add edx, ecx
     ;jz @@direccolfound
     
     cmp edx, eax
     ja @@kt
     mov eax, edx
     mov actcol, ebx
     
   @@kt:
     add ebx,16
     cmp ebx, 4096
     jb  @@colstart

   @@colfound:  
     mov eax, actcol
     shr eax,4
     
     ret
     
@@direccolfound:
     mov actcol, ebx
     jmp @@colfound
     
     
endp     
#	  
	  











PUBLIC float2int_
float2int_ proc near
   fld dword ptr [edi]
   fistp dword ptr [esi]
   ret
endp









comment #












;  Zeichnet eine Gerade von X,Y nach XE,YE mit Farbe
;  BerÅcksichtigt XorMode.
;
;  Aufruf: PutLine (X,Y,XE,YE,Farbe:WORD);
;---------------
X		EQU	WORD PTR [BP+14]          EAX
Y		EQU	WORD PTR [BP+12]          EBX
XE		EQU	WORD PTR [BP+10]          ECX
YE		EQU	WORD PTR [BP+8]           EDX
Farbe		EQU	BYTE PTR [BP+6]           EDI
;----------------
PutLine		PROC	NEAR
;

;  Berechne SI := DLTX
;
                MOV     esi,[XE]         ; XE
                SUB     esi,[X]          ; XE - X
;
;  Wenn DLTX < 0 , dann tausche X <-> XE und Y <-> YE
;
		JNS	PutLine_2
		MOV	eax,[XE]
		XCHG	eax,[X]
		MOV	[XE],eax
		MOV	eax,[YE]
		XCHG	eax,[Y]
		MOV	[YE],eax
		NEG	esi		; DLTX:= -DLTX
PutLine_2:
;
;  Berechne edie Adresse des 1. Byte im Video-Mem
;  edxedi = (Y * BytesPerScanLine) + X ;		ES:= [WinASegment]
;
;		GetScreenAddr
;               GetScreenAddr	MACRO
		MOVZX	Eeax,[BytesPerScanline]
		MOVZX	Eedx,[Y]		; Eedx = Y
		MUL	Eedx		; EedxEeax := BytesPerScanLine * Y
;
		MOVZX	Eebx,[X]			; Eebx := X
		ADD	Eeax,Eebx
;
		MOVZX	Eebx,[WinGranularity]	; [kByte]
		SHL	Eebx,10			; [Byte]
		ediV	Eebx			; Eedx := Offset   Eeax := Segm
		MOV	edi,edx			; edi := Offset
		MOV	edx,eax			; edx := Segm
		MOV	ES,[WinASegment]	; ES:edi-> Video-Mem
;		ENDM
;
;		SetBanks		; Setze beide Windows := edx
;               SetBanks	MACRO
		PUSH	edx
		SUB	ebx,ebx		; BH:=0 Window setzen;	BL:=0 Window A
		CALL	[WinFuncPtr]
		POP	edx
		MOV	BL,1		; Window B
		CALL	[WinFuncPtr]
;		ENDM
;
		MOV	AL,[Farbe]	; AL := Farbe
;
;
;  Wir haben MovMode.  Setze das 1. Pixel
;
		MOV	ES:[edi],AL
;
;  Berechne ebx := DLTY
;
		MOV	ebx,[YE]
		SUB	ebx,[Y]
;
;  Wenn DLTY > 0, dann poesitive Steigung (Scanlineoffset = +BytePerLine)
;
		JNS	PutLine_8
		NEG	ebx		; DLTY := -DLTY
;
;  Wenn DLTX > DLTY, dann berechne Y = f(X)
;
                CMP     esi,ebx
		JG	SHORT PutLine_20
;...............
;  Wir berechnen X = f(Y) mit Scanlineoffset = -BytePerLine
;
		MOV	ecx,ebx		; Loopcount = DLTY
		JecxZ	PutLine_0	; Loopcount = 0 ?
                MOV     ebp,ebx
		NEG	ebp		; SUM = - DLTY
		ADD	esi,esi		; 2 * DLTX
		ADD	ebx,ebx		; 2 * DLTY
		ALIGN	4
PutLine_12:
		SUB	edi,[BytesPerScanline]; Y := Y - 1
		JC	SHORT PutLine_13	; Achte auf Bankswitch
PutLine_14:
		ADD	ebp,esi			; SUM := SUM + 2 DLTX
		JS	SHORT PutLine_16	; Skip, wenn SUM < 0
		SUB	ebp,ebx			; SUM = SUM - 2 DLTY
		INC	edi			; X := X + 1
		JZ	SHORT PutLine_15	; Achte auf Bankswitch
PutLine_16:
		MOV	ES:[edi],AL		; Setze Pixel
		DEC	ecx
		JNZ	SHORT PutLine_12	; Loop mit DLTY
		JMP	SHORT PutLine_0
;...............
PutLine_13:	CALL	DecWrBank
		JMP	SHORT PutLine_14
;...............
PutLine_15:	CALL	IncWrBank
		JMP	SHORT PutLine_16
;............................
;  Wir berechnen Y = f(X) mit Scanlineoffset = -BytePerLine
;
PutLine_20:	MOV	ecx,esi		; Loopcount = DLTX
		JecxZ	PutLine_0	; Loopcount = 0 ?
		MOV	ebp,esi
		NEG	ebp		; SUM = - DLTX
		ADD	esi,esi		; 2 * DLTX
		ADD	ebx,ebx		; 2 * DLTY
		ALIGN	4
PutLine_22:
		INC	edi			; X := X + 1
		JZ	SHORT PutLine_23	; Achte auf Bankswitch
PutLine_24:
		ADD	ebp,ebx			; SUM := SUM + 2 DLTY
		JS	SHORT PutLine_26	; Skip, wenn SUM < 0
		SUB	ebp,esi			; SUM := SUM - 2DLTX
		SUB	edi,[BytesPerScanline]	; Y := Y - 1
		JC	SHORT PutLine_25	; Achte auf Bankswitch
PutLine_26:
		MOV	ES:[edi],AL		; Setze Pixel
		DEC	ecx
		JNZ	SHORT PutLine_22	; Loop mit DLTX
PutLine_0:
		POP	ebp
		RET	10			; Entferne 5 Words
;...............
PutLine_23:	CALL	IncWrBank
		JMP	SHORT PutLine_24
;...............
PutLine_25:	CALL	DecWrBank
		JMP	SHORT PutLine_26
;---------------
;  Wenn DLTX > DLTY, dann berechne Y = f(X)
;
PutLine_8:	CMP	esi,ebx
		JG	SHORT PutLine_40
;............................
;  Wir berechnen X = f(Y) mit Scanlineoffset = +BytePerLine
;
		MOV	ecx,ebx		; Loopcount = DLTY
		JecxZ	PutLine_0	; Loopcount = 0 ?
		MOV	ebp,ebx
		NEG	ebp		; SUM = - DLTY
		ADD	esi,esi		; 2 * DLTX
		ADD	ebx,ebx		; 2 * DLTY
		ALIGN	4
PutLine_32:
		ADD	edi,[BytesPerScanline]; Y := Y + 1
		JC	SHORT PutLine_33	; Achte auf Bankswitch
PutLine_34:
		ADD	ebp,esi			; SUM := SUM + 2 DLTX
		JS	SHORT PutLine_36	; Skip, wenn SUM < 0
		SUB	ebp,ebx			; SUM := SUM - 2 DLTY
		INC	edi			; X := X + 1
		JZ	SHORT PutLine_35	; Achte auf Bankswitch
PutLine_36:
		MOV	ES:[edi],AL		; Setze Pixel
		DEC	ecx
		JNZ	SHORT PutLine_32	; Loop mit DLTY
		JMP	SHORT PutLine_0
;...............
PutLine_33:	CALL	IncWrBank
		JMP	SHORT PutLine_34
;...............
PutLine_35:	CALL	IncWrBank
		JMP	SHORT PutLine_36
;...............
;  Wir berechnen Y = f(X) mit Scanlineoffset = +BytePerLine
;
PutLine_40:	MOV	ecx,esi		; Loopcount = DLTX
		JecxZ	PutLine_0	; Loopcount = 0 ?
		MOV	ebp,esi
		NEG	ebp		; SUM = - DLTX
		ADD	esi,esi		; 2 * DLTX
		ADD	ebx,ebx		; 2 * DLTY
		ALIGN	4
PutLine_42:
		INC	edi			; X := X + 1
		JZ	SHORT PutLine_43	; Achte auf Bankswitch
PutLine_44:
		ADD	ebp,ebx			; SUM := SUM + 2 DLTY
		JS	SHORT PutLine_46	; Skip, wenn SUM < 0
		SUB	ebp,esi			; SUM := SUM - 2 DLTX
		ADD	edi,[BytesPerScanline]; Y := Y + 1
		JC	SHORT PutLine_45	; Achte auf Bankswitch
PutLine_46:
		MOV	ES:[edi],AL		; Setze Pixel
		DEC	ecx
		JNZ	SHORT PutLine_42	; Loop mit DLTX
		JMP	PutLine_0
;...............
PutLine_43:	CALL	IncWrBank
		JMP	SHORT PutLine_44
;...............
PutLine_45:	CALL	IncWrBank
		JMP	SHORT PutLine_46
;...............



PutLine 	ENDP












PUBLIC mix2cols_ 
mix2cols_ near proc
    ; col 1

     lea esi, _ActivePalette
     shl eax,4

#
END
