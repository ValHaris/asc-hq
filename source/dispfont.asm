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



fontoffset         DD ?
stringoffset       DD ?

mempos             DD 256 DUP  (?)
stringlen          DB ?             ; in Zeichen
stringpixlen       DD ?             ; in Pixeln
height             DB ?
spacelength        DD ?
charlen            DD 256 DUP (?)   ; LÑnge der einzelnen Buchstaben
abstand            DD 256 DUP (?)
extraheight        DB ?
rightextralength   DW ?
leftextralength    DD ?
bytestoprocess     DW ?
negkernwidth       DW ?
temp1              DW ?
temp2              DW ?

.CODE

charsize           EQU 8  ; grî·e eines Characters
charpos            EQU 8
charmempos         EQU 4
; charlength         EQU 0
fontheight         EQU 6
fontkerning        EQU 2056
; nlengthpos         EQU 307+ 110 *12  ; leerzeichen entspricht LÑnge von n

tfont STRUC
         fontname         DD                ?
         fontcolor        DB                ?
         fontcaps         DB                ?
         fnt_height       DW                ?
         fntcharacter     DB  2048 DUP     (?)
         fntkernchartable DB 65536 DUP     (?)
         fontpalette      DD                ?
         fontgroundline   DB                ?
      ENDS





SETBYTESLW2 MACRO
     LOCAL LB1,lb2,LB5

     mov temp1, di
     mov temp2, di

     add temp1, cx

     jc LB5   ; change virtual page

     setbytes
     jmp LB1

      LB5:

      ;xor ecx,ecx
      sub cx, temp1

      setbytes

      SetNextVirtualPage2
      mov edi, _ActiveGraphModeParameters.LinearAddress

      movzx ecx, temp1
      setbytes

      LB1:
ENDM

SETSPACELENGTH MACRO
   LOCAL NOBSW
   add di,WORD PTR spacelength
   jnc NOBSW
   SetNextVirtualPage2
   NOBSW:
ENDM





PUBLIC showtext2_
showtext2_ PROC NEAR
   LOCALS
   push ebp

;   cmp _activefontsettings.fntcompMode,0
;   jz @@CompatiblityMode

   mov leftextralength, 0
   mov extraheight,0
 @@COMPATIBLITYMODE:
   cmp _ActiveGraphModeParameters.windowstatus,100
   jz @@LINEARFRAMEBUFFERINIT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; 64 kb Video Window ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


   mul WORD PTR _ActiveGraphModeParameters.ScanLineLength
   add ax, bx
   adc dx,0


   cmp dl, byte ptr _hardwaregraphmodeparameters.actsetpage

   jz @@COMMONINIT
   push eax
   push ebx
   push ecx

   mov byte ptr _hardwaregraphmodeparameters.pagetoset,dl
   SetVirtualPage

   pop ecx
   pop ebx
   pop eax

   jmp @@COMMONINIT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@LINEARFRAMEBUFFERINIT:
   movzx edx, WORD PTR _ActiveGraphModeParameters.ScanLineLength
   mul edx
   add eax, ebx


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Common ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@COMMONINIT:
   mov negkernwidth,0

   add eax,_ActiveGraphModeParameters.LinearAddress

   mov edi, eax

   mov ebp, ecx        ; String ab jetzt in EBP
   mov stringoffset, ecx


   mov ebx,  DWORD PTR _activefontsettings

   cmp ebx,0                     ; activefontsettings.font == NULL  ?
   jz @@ENDE


   mov fontoffset, ebx           ; Addresse von activefontsettings.font

   mov dl, [ebx + fontheight]
   mov dh, _activefontsettings.fntheight
   cmp dh,0
   jz @@USENORMALHEIGHT
   cmp dh,dl
   jb @@LESSHEIGHT

   sub dh,dl
   mov extraheight, dh
   jmp @@USENORMALHEIGHT

 @@LESSHEIGHT:
   mov dl,dh

 @@USENORMALHEIGHT:
   mov height, dl



   mov ebx,0
   mov stringlen,0
   mov stringpixlen,0

   ; Berechnen der Speicherpositionen der einzelnen Buchstaben
   ;    in bx Position innerhalb des Strings, in 4er Schritten

   mov ecx,0
   mov edx, fontoffset

 @@CHARSTART:
   movzx eax, word ptr [ebp]    ; Zeichen in al, ÅbernÑchstes in ah

   cmp al,0                     ; Ende des Strings ?
   jz @@LENGTHCALC

   movsx ecx, byte ptr [ edx + eax + fontkerning ]    ; Kerning in ecx

   cmp ah, 0       ; nÑchstes Zeichen Åberhaupt vorhanden ??
   jz @@keinabstand
   add ecx, 2                   ; StandartmÑ·iger Zeichenabstand
@@keinabstand:

   and eax, 0ffh

   lea esi, [ eax * charsize + edx + charpos ]

   cmp WORD PTR [esi+2],0               ; size
   jz @@CharDoesNotExist              ; wenn 0, dann Zeichen nicht vorhanden

   mov eax, [esi+4]                     ; Pointer auf Bilddaten -> EAX

   add eax,2                          ;  im ersten Word: LÑnge  => wird Åbersprungen
   mov [mempos + ebx], eax            ;  im lokalen Array ablegen
   movzx eax, WORD PTR [esi]          ;  LÑnge des Buchstabens

   add stringpixlen, eax
   mov [charlen + ebx], eax

   add stringpixlen, ecx	      ;  Abstand zum nÑchsten Buchstaben draufaddieren
   mov [abstand + ebx], ecx



   inc stringlen
   add ebx,4

@@CharDoesNotExist:
   inc ebp
   jmp @@CHARSTART


@@LENGTHCALC:
   mov bx,_activefontsettings.fntlength
   cmp bx,0
   mov rightextralength,0
   mov leftextralength,0
   jz @@USENORMALLENGTH

   cmp bx,WORD PTR stringpixlen
   jb @@LESSLENGTH

   sub bx,WORD PTR stringpixlen
   inc bx
   cmp _activefontsettings.fntjustify,1
   ja @@GREATERLENGTHRIGHT
   jb @@GREATERLENGTHLEFT

   mov ax,bx
   shr bx,1
   sub ax,bx
   mov rightextralength,bx
   mov WORD PTR leftextralength,ax
   jmp @@LengthCalcFinished


 @@GREATERLENGTHLEFT:
   mov rightextralength, bx
   jmp @@LengthCalcFinished

 @@GREATERLENGTHRIGHT:
   mov WORD PTR leftextralength, bx
   jmp @@LengthCalcFinished


 @@LESSLENGTH:
;   inc bx
   mov WORD PTR stringpixlen,bx

 @@USENORMALLENGTH:
   cmp stringpixlen,0
   jz @@ende

 @@LengthCalcFinished:

   ; Eigentliche Anzeigeroutine
   ;  benutzt die gerade Berechneten Werte fÅr Speicherposition und Kerning

   cmp _ActiveGraphModeParameters.windowstatus,100
   jz @@LINDISPSTART

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; 64 kb Video Window ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


 @@DISPSTART:
    mov dh, _activefontsettings.fntbackground

    mov eax, _ActiveGraphModeParameters.ScanLineLength
    sub eax, stringpixlen
    cmp dh,254
    ja @@DISPSTARTNOBACKGR
    sub ax, rightextralength
    sub ax, WORD PTR leftextralength
    jmp @@DISPSTARTNOBACKGRNOLEFTT
  @@DISPSTARTNOBACKGR:
    add di, WORD PTR leftextralength
    jnc @@DISPSTARTNOBACKGRNOLEFTT
    setnextvirtualpage2
  @@DISPSTARTNOBACKGRNOLEFTT:
    mov spacelength, eax

   mov dl, _activefontsettings.fntcolor



 @@DISPLINESTART:
  cmp dh,255
  jz @@NOLEFTSPACE

  mov ecx,0
  mov cx, WORD PTR leftextralength
  cmp cx,0
  jz @@NOLEFTSPACE

  mov al,dh
  mov ah,dh
  SETBYTESLW2


 @@NOLEFTSPACE:
   mov ebx,0
   mov ax,WORD PTR stringpixlen
   
   cmp ax,0
   jle @@LINEFINISHED2

   mov bytestoprocess,ax      ; in line

   movzx eax, stringlen

 @@DISPCHARSTART:

   mov ebp, [mempos+ebx]

   mov ecx, [charlen+ebx]

 @@CHARLOOP:
   inc negkernwidth
   cmp BYTE PTR [ebp],0
   jz @@SETNOPIX

   mov [edi], dl
   jmp @@CONTINUEPIXSET

 @@SETNOPIX:
   cmp negkernwidth,0
   jl @@CONTINUEPIXSET
   cmp dh,255
   jz @@CONTINUEPIXSET

   mov [edi], dh


 @@CONTINUEPIXSET:
   inc ebp
   add di,1
   jnc @@PIXNOBANKSWITCH
   SetNextVirtualPage2

 @@PIXNOBANKSWITCH:

   dec bytestoprocess
   cmp bytestoprocess,0
   jle @@LINEFINISHED2

   dec cx
   jnz @@CHARLOOP


 ; -------------- Kerning ---------------
   mov ecx,[abstand+ebx]
   cmp ecx,0
   jz @@NAECHSTESZEICHEN
   jl @@NEGATIV

 ; --------- Positives Kerning ----------
   sub bytestoprocess, cx
   jnc @@positiv
   add cx, bytestoprocess
  @@POSITIV:
   cmp dh,255
   jz @@NOBACKGRKERN
   mov [edi],dh
 @@NOBACKGRKERN:
   add di,1
   jnc @@K1NOBANKSWITCH
   setnextvirtualpage2
  @@K1NOBANKSWITCH:

   dec ecx
   jnz @@POSITIV
   jmp @@NAECHSTESZEICHEN

 ; ------------ Negatives Kerning ---------
  @@NEGATIV:
   sub bytestoprocess, cx
   cmp dh,255
   jz @@NOBACKGRNDNEGKERN
   mov negkernwidth,cx
   dec negkernwidth 

 @@NOBACKGRNDNEGKERN:
   add di,cx
   jc @@NAECHSTESZEICHEN
   SetPrivVirtualPage2

 ; ------------ Kerning fertig --------------
   @@NAECHSTESZEICHEN:


   mov [mempos+ebx],ebp
   add ebx,4
   
   cmp bytestoprocess,0
   jle @@LINEFINISHED

   dec eax
   jnz @@DISPCHARSTART
   jmp @@LINEFINISHED

@@LINEFINISHED2:       ; Durch zu kurze LÑnge abgeschnitten
   dec ecx
   add ebp,ecx
   mov [mempos+ebx],ebp


@@LINEFINISHED:
  cmp dh,255
  jz @@EXTRASPACENOBANKSWITCH

  mov ecx,0
  mov cx, rightextralength
  cmp cx,0
  jz @@EXTRASPACENOBANKSWITCH
  mov al,dh
  mov ah,dh
  SETBYTESLW2

@@EXTRASPACENOBANKSWITCH:

   SETSPACELENGTH

   dec height
   jnz @@DISPLINESTART

   cmp extraheight,0
   jz @@ENDE

   cmp dh,255
   jz @@ENDE
   mov bx, WORD PTR stringpixlen
   add bx, rightextralength
   add bx, WORD PTR leftextralength


   mov al,dh
   mov ah,dh

 @@EXTRAHEIGHTLOOPSTART:

   movzx ecx, bx

   SETBYTESLW2

   SETSPACELENGTH
   dec extraheight
   jnz @@EXTRAHEIGHTLOOPSTART


   jmp @@ENDE


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 @@LINDISPSTART:
    mov dh, _activefontsettings.fntbackground

    mov eax, _ActiveGraphModeParameters.ScanLineLength
    sub eax, stringpixlen
    cmp dh,254
    ja @@LINDISPSTARTNOBACKGR
    sub ax, rightextralength
    sub ax, WORD PTR leftextralength
    jmp @@LINDISPSTARTNOBACKGRNOEEXTRALEFT
  @@LINDISPSTARTNOBACKGR:
    add edi, leftextralength
  @@LINDISPSTARTNOBACKGRNOEEXTRALEFT:
    mov spacelength, eax

   mov dl, _activefontsettings.fntcolor



 @@LINDISPLINESTART:
  cmp dh,255
  jz @@LINNOLEFTSPACE

  mov ecx,0
  mov cx, WORD PTR leftextralength
  cmp cx,0
  jz @@LINNOLEFTSPACE

  mov al,dh
  mov ah,dh
  SetBytes


 @@LINNOLEFTSPACE:
   mov ebx,0
   mov ax,WORD PTR stringpixlen
   cmp ax,0
   jle @@LINLINEFINISHED2
   mov bytestoprocess,ax      ; in line

   movzx eax, stringlen

 @@LINDISPCHARSTART:

   mov ebp, [mempos+ebx]

   mov ecx, [charlen+ebx]

 @@LINCHARLOOP:
   inc negkernwidth
   cmp BYTE PTR [ebp],0
   jz @@LINSETNOPIX

   mov [edi], dl
   jmp @@LINCONTINUEPIXSET

 @@LINSETNOPIX:
   cmp negkernwidth,0
   jl @@LINCONTINUEPIXSET
   cmp dh,255
   jz @@LINCONTINUEPIXSET

   mov [edi], dh


 @@LINCONTINUEPIXSET:
   inc ebp
   inc edi

   dec bytestoprocess
   cmp bytestoprocess,0
   jle @@LINLINEFINISHED2

   dec cx
   jnz @@LINCHARLOOP


 ; -------------- Kerning ---------------
   mov ecx,[abstand+ebx]

   cmp ecx,0
   jz @@LINNAECHSTESZEICHEN
   

   jl @@LINNEGATIV

 ; --------- Positives Kerning ----------
   sub bytestoprocess, cx
   jnc @@LINPOSNoOverflow
   add cx, bytestoprocess

  @@LINPOSNoOverflow:
  
   cmp dh,255
   jz @@LinPosKernNochBackGround
   
  @@LINPOSKERNBACK:
   mov [edi],dh
   inc edi
   dec ecx
   jnz @@LINPOSKERNBACK
   jmp @@LINNAECHSTESZEICHEN
   
   @@LinPosKernNochBackGround:
   add edi, ecx
   jmp @@LINNAECHSTESZEICHEN
   
 ; ------------ Negatives Kerning ---------
  @@LINNEGATIV:
   mov negkernwidth,cx
   dec negkernwidth
   sub bytestoprocess, cx
   add edi,ecx
   
 ; ------------ Kerning fertig --------------



   @@LINNAECHSTESZEICHEN:

   mov [mempos+ebx],ebp
   add ebx,4
   
   cmp bytestoprocess,0
   jle @@LINLINEFINISHED

   dec eax
   jnz @@LINDISPCHARSTART
   jmp @@LINLINEFINISHED

@@LINLINEFINISHED2:       ; Durch zu kurze LÑnge abgeschnitten
   dec ecx
   add ebp,ecx
   mov [mempos+ebx],ebp


@@LINLINEFINISHED:
  cmp dh,255
  jz @@LINEXTRASPACENOBANKSWITCH

  mov ecx,0
  mov cx, rightextralength
  cmp ecx,0
  jz @@LINEXTRASPACENOBANKSWITCH
  mov al,dh
  mov ah,dh
  SetBytes

@@LINEXTRASPACENOBANKSWITCH:

   add edi, spacelength

   dec height
   jnz @@LINDISPLINESTART

   cmp extraheight,0
   jz @@ENDE

   cmp dh,255
   jz @@ENDE
   mov bx, WORD PTR stringpixlen
   add bx, rightextralength
   add bx, WORD PTR leftextralength


   mov al,dh
   mov ah,dh

 @@LINEXTRAHEIGHTLOOPSTART:

   movzx ecx, bx

   SetBytes

   add edi, spacelength
   dec extraheight
   jnz @@LINEXTRAHEIGHTLOOPSTART




 @@ENDE:
   pop ebp
   ret

ENDP


PUBLIC showtext2c_
showtext2c_ PROC NEAR
   LOCALS
   push ebp


;   cmp _activefontsettings.fntcompMode,0
;   jz @@CompatiblityMode

   mov leftextralength, 0
   mov extraheight,0
 @@COMPATIBLITYMODE:
   cmp _ActiveGraphModeParameters.windowstatus,100
   jz @@LINEARFRAMEBUFFERINIT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; 64 kb Video Window ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


   mul WORD PTR _ActiveGraphModeParameters.ScanLineLength
   add ax, bx
   adc dx,0


   cmp dl, byte ptr _hardwaregraphmodeparameters.actsetpage

   jz @@COMMONINIT
   push eax
   push ebx
   push ecx

   mov byte ptr _hardwaregraphmodeparameters.pagetoset,dl
   SetVirtualPage

   pop ecx
   pop ebx
   pop eax

   jmp @@COMMONINIT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@LINEARFRAMEBUFFERINIT:
   movzx edx, WORD PTR _ActiveGraphModeParameters.ScanLineLength
   mul edx
   add eax, ebx


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Common ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

@@COMMONINIT:
   mov negkernwidth,0

   add eax,_ActiveGraphModeParameters.LinearAddress

   mov edi, eax

   mov ebp, ecx        ; String ab jetzt in EBP
   mov stringoffset, ecx


   mov ebx,  DWORD PTR _activefontsettings

   cmp ebx,0                     ; activefontsettings.font == NULL  ?
   jz @@ENDE


   mov fontoffset, ebx           ; Addresse von activefontsettings.font

   mov dl, [ebx + fontheight]
   mov dh, _activefontsettings.fntheight
   cmp dh,0
   jz @@USENORMALHEIGHT
   cmp dh,dl
   jb @@LESSHEIGHT

   sub dh,dl
   mov extraheight, dh
   jmp @@USENORMALHEIGHT

 @@LESSHEIGHT:
   mov dl,dh

 @@USENORMALHEIGHT:
   mov height, dl



   mov ebx,0
   mov stringlen,0
   mov stringpixlen,0

   ; Berechnen der Speicherpositionen der einzelnen Buchstaben
   ;    in bx Position innerhalb des Strings, in 4er Schritten

   mov ecx,0
   mov edx, fontoffset

 @@CHARSTART:
   movzx eax, word ptr [ebp]    ; Zeichen in al, ÅbernÑchstes in ah

   cmp al,0                     ; Ende des Strings ?
   jz @@LENGTHCALC

   movsx ecx, byte ptr [ edx + eax + fontkerning ]    ; Kerning in ecx

   cmp ah, 0       ; nÑchstes Zeichen Åberhaupt vorhanden ??
   jz @@keinabstand
   add ecx, 2                   ; StandartmÑ·iger Zeichenabstand
@@keinabstand:

   and eax, 0ffh

   lea esi, [ eax * charsize + edx + charpos ]

   cmp WORD PTR [esi+2],0               ; size
   jz @@CharDoesNotExist              ; wenn 0, dann Zeichen nicht vorhanden

   mov eax, [esi+4]                     ; Pointer auf Bilddaten -> EAX

   add eax,2                          ;  im ersten Word: LÑnge  => wird Åbersprungen
   mov [mempos + ebx], eax            ;  im lokalen Array ablegen
   movzx eax, WORD PTR [esi]          ;  LÑnge des Buchstabens

   add stringpixlen, eax
   mov [charlen + ebx], eax

   add stringpixlen, ecx	      ;  Abstand zum nÑchsten Buchstaben draufaddieren
   mov [abstand + ebx], ecx



   inc stringlen
   add ebx,4

@@CharDoesNotExist:
   inc ebp
   jmp @@CHARSTART


@@LENGTHCALC:
   mov bx,_activefontsettings.fntlength
   cmp bx,0
   mov rightextralength,0
   mov leftextralength,0
   jz @@USENORMALLENGTH

   cmp bx,WORD PTR stringpixlen
   jb @@LESSLENGTH

   sub bx,WORD PTR stringpixlen
   inc bx
   cmp _activefontsettings.fntjustify,1
   ja @@GREATERLENGTHRIGHT
   jb @@GREATERLENGTHLEFT

   mov ax,bx
   shr bx,1
   sub ax,bx
   mov rightextralength,bx
   mov WORD PTR leftextralength,ax
   jmp @@LengthCalcFinished


 @@GREATERLENGTHLEFT:
   mov rightextralength, bx
   jmp @@LengthCalcFinished

 @@GREATERLENGTHRIGHT:
   mov WORD PTR leftextralength, bx
   jmp @@LengthCalcFinished


 @@LESSLENGTH:
;   inc bx
   mov WORD PTR stringpixlen,bx

 @@USENORMALLENGTH:
   cmp stringpixlen,0
   jz @@ende

 @@LengthCalcFinished:

   ; Eigentliche Anzeigeroutine
   ;  benutzt die gerade Berechneten Werte fÅr Speicherposition und Kerning

   cmp _ActiveGraphModeParameters.windowstatus,100
   jz @@LINDISPSTART

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; 64 kb Video Window ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


 @@DISPSTART:
    mov dh, _activefontsettings.fntbackground

    mov eax, _ActiveGraphModeParameters.ScanLineLength
    sub eax, stringpixlen
    cmp dh,254
    ja @@DISPSTARTNOBACKGR
    sub ax, rightextralength
    sub ax, WORD PTR leftextralength
    jmp @@DISPSTARTNOBACKGRNOLEFTT
  @@DISPSTARTNOBACKGR:
    add di, WORD PTR leftextralength
    jnc @@DISPSTARTNOBACKGRNOLEFTT
    setnextvirtualpage2
  @@DISPSTARTNOBACKGRNOLEFTT:
    mov spacelength, eax


 @@DISPLINESTART:
  cmp dh,255
  jz @@NOLEFTSPACE

  mov ecx,0
  mov cx, WORD PTR leftextralength
  cmp cx,0
  jz @@NOLEFTSPACE

  mov al,dh
  mov ah,dh
  SETBYTESLW2


 @@NOLEFTSPACE:
   mov ebx,0
   mov ax,WORD PTR stringpixlen

   cmp ax,0
   jle @@LINEFINISHED2
   
   mov bytestoprocess,ax      ; in line

   movzx eax, stringlen

 @@DISPCHARSTART:

   mov ebp, [mempos+ebx]

   mov ecx, [charlen+ebx]

 @@CHARLOOP:
   inc negkernwidth
   mov dl,BYTE PTR [ebp]
   cmp dl,0
   jz @@SETNOPIX

   mov [edi], dl
   jmp @@CONTINUEPIXSET

 @@SETNOPIX:
   cmp negkernwidth,0
   jl @@CONTINUEPIXSET
   cmp dh,255
   jz @@CONTINUEPIXSET

   mov [edi], dh


 @@CONTINUEPIXSET:
   inc ebp
   add di,1
   jnc @@PIXNOBANKSWITCH
   SetNextVirtualPage2

 @@PIXNOBANKSWITCH:

   dec bytestoprocess
   cmp bytestoprocess,0
   jle @@LINEFINISHED2

   dec cx
   jnz @@CHARLOOP


 ; -------------- Kerning ---------------
   mov ecx,[abstand+ebx]
   cmp ecx,0
   jz @@NAECHSTESZEICHEN
   jl @@NEGATIV

 ; --------- Positives Kerning ----------
   sub bytestoprocess, cx
   jnc @@positiv
   add cx, bytestoprocess
  @@POSITIV:
   cmp dh,255
   jz @@NOBACKGRKERN
   mov [edi],dh
 @@NOBACKGRKERN:
   add di,1
   jnc @@K1NOBANKSWITCH
   setnextvirtualpage2
  @@K1NOBANKSWITCH:

   dec ecx
   jnz @@POSITIV
   jmp @@NAECHSTESZEICHEN

 ; ------------ Negatives Kerning ---------
  @@NEGATIV:
   sub bytestoprocess, cx
   cmp dh,255
   jz @@NOBACKGRNDNEGKERN
   mov negkernwidth,cx
   dec negkernwidth

 @@NOBACKGRNDNEGKERN:
   add di,cx
   jc @@NAECHSTESZEICHEN
   SetPrivVirtualPage2

 ; ------------ Kerning fertig --------------
   @@NAECHSTESZEICHEN:


   mov [mempos+ebx],ebp
   add ebx,4
   
   cmp bytestoprocess,0
   jle @@LINEFINISHED

   dec eax
   jnz @@DISPCHARSTART
   jmp @@LINEFINISHED

@@LINEFINISHED2:       ; Durch zu kurze LÑnge abgeschnitten
   dec ecx
   add ebp,ecx
   mov [mempos+ebx],ebp


@@LINEFINISHED:
  cmp dh,255
  jz @@EXTRASPACENOBANKSWITCH

  mov ecx,0
  mov cx, rightextralength
  cmp cx,0
  jz @@EXTRASPACENOBANKSWITCH
  mov al,dh
  mov ah,dh
  SETBYTESLW2

@@EXTRASPACENOBANKSWITCH:

   SETSPACELENGTH

   dec height
   jnz @@DISPLINESTART

   cmp extraheight,0
   jz @@ENDE

   cmp dh,255
   jz @@ENDE
   mov bx, WORD PTR stringpixlen
   add bx, rightextralength
   add bx, WORD PTR leftextralength


   mov al,dh
   mov ah,dh

 @@EXTRAHEIGHTLOOPSTART:

   movzx ecx, bx

   SETBYTESLW2

   SETSPACELENGTH
   dec extraheight
   jnz @@EXTRAHEIGHTLOOPSTART


   jmp @@ENDE


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; Linear Frame Buffer ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 @@LINDISPSTART:
    mov dh, _activefontsettings.fntbackground

    mov eax, _ActiveGraphModeParameters.ScanLineLength
    sub eax, stringpixlen
    cmp dh,254
    ja @@LINDISPSTARTNOBACKGR
    sub ax, rightextralength
    sub ax, WORD PTR leftextralength
    jmp @@LINDISPSTARTNOBACKGRNOEEXTRALEFT
  @@LINDISPSTARTNOBACKGR:
    add edi, leftextralength
  @@LINDISPSTARTNOBACKGRNOEEXTRALEFT:
    mov spacelength, eax


 @@LINDISPLINESTART:
  cmp dh,255
  jz @@LINNOLEFTSPACE

  mov ecx,0
  mov cx, WORD PTR leftextralength
  cmp cx,0
  jz @@LINNOLEFTSPACE

  mov al,dh
  mov ah,dh
  SetBytes


 @@LINNOLEFTSPACE:
   mov ebx,0
   mov ax,WORD PTR stringpixlen
   cmp ax,0
   jle @@LINLINEFINISHED2
   mov bytestoprocess,ax      ; in line

   movzx eax, stringlen

 @@LINDISPCHARSTART:

   mov ebp, [mempos+ebx]

   mov ecx, [charlen+ebx]

 @@LINCHARLOOP:
   inc negkernwidth
   mov dl,BYTE PTR [ebp]
   cmp dl,0
   jz @@LINSETNOPIX

   mov [edi], dl
   jmp @@LINCONTINUEPIXSET

 @@LINSETNOPIX:
   cmp negkernwidth,0
   jl @@LINCONTINUEPIXSET
   cmp dh,255
   jz @@LINCONTINUEPIXSET

   mov [edi], dh


 @@LINCONTINUEPIXSET:
   inc ebp
   inc edi

   dec bytestoprocess
   cmp bytestoprocess,0
   jle @@LINLINEFINISHED2

   dec cx
   jnz @@LINCHARLOOP


 ; -------------- Kerning ---------------
   mov ecx,[abstand+ebx]

   cmp ecx,0
   jz @@LINNAECHSTESZEICHEN
   

   jl @@LINNEGATIV

 ; --------- Positives Kerning ----------
   sub bytestoprocess, cx
   jnc @@LINPOSNoOverflow
   add cx, bytestoprocess

  @@LINPOSNoOverflow:
  
   cmp dh,255
   jz @@LinPosKernNochBackGround
   
  @@LINPOSKERNBACK:
   mov [edi],dh
   inc edi
   dec ecx
   jnz @@LINPOSKERNBACK
   jmp @@LINNAECHSTESZEICHEN
   
   @@LinPosKernNochBackGround:
   add edi, ecx
   jmp @@LINNAECHSTESZEICHEN
   
 ; ------------ Negatives Kerning ---------
  @@LINNEGATIV:
   mov negkernwidth,cx
   dec negkernwidth
   sub bytestoprocess, cx
   add edi,ecx
   
 ; ------------ Kerning fertig --------------



   @@LINNAECHSTESZEICHEN:

   mov [mempos+ebx],ebp
   add ebx,4
   
   cmp bytestoprocess,0
   jle @@LINLINEFINISHED

   dec eax
   jnz @@LINDISPCHARSTART
   jmp @@LINLINEFINISHED

@@LINLINEFINISHED2:       ; Durch zu kurze LÑnge abgeschnitten
   dec ecx
   add ebp,ecx
   mov [mempos+ebx],ebp


@@LINLINEFINISHED:
  cmp dh,255
  jz @@LINEXTRASPACENOBANKSWITCH

  mov ecx,0
  mov cx, rightextralength
  cmp ecx,0
  jz @@LINEXTRASPACENOBANKSWITCH
  mov al,dh
  mov ah,dh
  SetBytes

@@LINEXTRASPACENOBANKSWITCH:

   add edi, spacelength

   dec height
   jnz @@LINDISPLINESTART

   cmp extraheight,0
   jz @@ENDE

   cmp dh,255
   jz @@ENDE
   mov bx, WORD PTR stringpixlen
   add bx, rightextralength
   add bx, WORD PTR leftextralength


   mov al,dh
   mov ah,dh

 @@LINEXTRAHEIGHTLOOPSTART:

   movzx ecx, bx

   SetBytes

   add edi, spacelength
   dec extraheight
   jnz @@LINEXTRAHEIGHTLOOPSTART




 @@ENDE:
   pop ebp
   ret

ENDP




PUBLIC expand_
expand_ PROC NEAR
  LOCALS
  pusha

  mov esi, eax
  mov edi, ebx

  mov ax, [esi]
  mov [edi],ax
  mov dx,1
  add esi,2
  add edi,2

@STRT:
  mov al,[esi]
  inc esi

;  mov dx,ax
  mov bx,1
  ror bx,1

@SRT2:
  rol bx,1
  test al,bl
  jnz @SRT3

  mov [edi],dh    ; stosb 0
  jmp @SRT4

@SRT3:
  mov [edi],dl   ; stosb 1

@SRT4:
  inc edi

  dec ecx
  jz @@ENDE

  cmp bx,80h
  jnz @SRT2
  jmp @STRT

@@ENDE:

  popa

  ret
ENDP



END
