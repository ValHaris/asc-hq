; asm

TActiveGraphModeparameters STRUC
  ResolutionX      dd ?
  ResolutionY      dd ?
  ActSetPage       dd ?
  WindowStatus     dd ?
  Granularity      dd ?
  ScanLineLength   dd ?
  ScanLineNumber   dd ?
  BytePerScanLine  dd ?
  Mode             dd ?
  VidoMemory       dd ?
  BytePerPix       dd ?
  LinearAddress    dd ?
  PageToSet        dd ?
TActiveGraphModeparameters ENDS

TFontSettings STRUC
  font             dd ?
  fntcolor            db ?
  fntbackground       db ?
  fntmarkcolor        db ?
  fntcolorfont        db ?
  fntLength        dw ?
  fntDirection        db ?
  fntJustify          db ?
  fntitalic           db ?
  fntheight           db ?
  fntcompmode         db ?
  fntmarkfont         dd ?
ENDS

TMouseSettings STRUC
  mouse_x             dd ?
  mouse_y             dd ?
  mouse_x1            dd ?
  mouse_y1            dd ?
  mouse_altx          dd ?
  mouse_alty          dd ?
  mouse_background    dd ?
  mouse_pointer       dd ?
  mouse_sizex         dd ?
  mouse_sizey         dd ?
  mouse_taste         db ?
  mouse_status        db ?
  mouse_offx1         dd ?
  mouse_offy1         dd ?
  mouse_offx2         dd ?
  mouse_offy2         dd ?
  mouse_hotspotx      dd ?
  mouse_hotspoty      dd ?
  mouse_backgroundsize dd ?
ends


SetNextVirtualPage MACRO p

    IFB <p>
      'Parameter fehlt'
    ENDIF

    Push eax
    push ebx
    push edx
    push ecx
    inc _hardwaregraphmodeparameters.pagetoset

    SetVirtualPage

    pop ecx
    pop edx
    pop ebx
    pop eax
    sub p, 10000h
ENDM

SetPrivVirtualPage MACRO p

    IFB <p>
      'Parameter fehlt'
    ENDIF

    Push eax
    push ebx
    push edx
    push ecx
    dec _hardwaregraphmodeparameters.pagetoset

    SetVirtualPage

    pop ecx
    pop edx
    pop ebx
    pop eax
    add p, 10000h
ENDM

SetNextVirtualPage2 MACRO  parameters

    IFB <parameters>
      gmp equ _HardwaregraphModeParameters
    ELSE
      gmp equ parameters
    ENDIF


    Push eax
    push ebx
    push edx
    push ecx
    inc gmp.pagetoset

    SetVirtualPage parameters

    pop ecx
    pop edx
    pop ebx
    pop eax
ENDM

SetPrivVirtualPage2 MACRO

    Push eax
    push ebx
    push edx
    push ecx
    dec _hardwaregraphmodeparameters.pagetoset

    SetVirtualPage

    pop ecx
    pop edx
    pop ebx
    pop eax
ENDM


SetVirtualPage macro parameters
    LOCAL SingleWindow
    IFB <parameters>
      agmp equ _HardwareGraphModeParameters
    ELSE
      agmp equ parameters
    ENDIF

    mov _handleractive,1
    cli

    cmp byte ptr agmp.WindowStatus,0
    jz SingleWindow

    mov eax, 4F05h
    mov ebx, 1
    mov edx, agmp.pagetoset
    mov ecx, agmp.Granularity
    shl edx, cl
    int 10h

   SingleWindow:
    mov eax, 4F05h
    mov ebx, 0

    mov edx,agmp.pagetoset
    mov agmp.actsetpage, edx
    mov ecx, agmp.Granularity
    shl edx, cl
    int 10h

    sti
    mov _handleractive,0

ENDM



stossb macro
  stosb
endm

stossw macro
  stosw
endm

stossd macro
  stosd
endm

lodssb macro
  lodsb
endm

lodssw macro
  lodsw
endm

lodssd macro
  lodsd
endm


;------------------------------------
;-- Setbytes
;------------------------------------

setBytes Macro ID1
      LOCAL ID2,ID3

      test edi,1
      jz ID2
      stossb
      dec ecx
      IFB <ID1>
        jz ID3
      ELSE
        jz ID1
      ENDIF

      ID2:
      shr ecx,1
      rep stossw

      IFB <ID1>
        jnc ID3
      ELSE
        jnc ID1
      ENDIF

      stossb

      ID3:
endm


;------------------------------------
;-- Addbytes
;------------------------------------

AddBytes Macro
      LOCAL ID2

    ID2:
      add [edi], al
      inc edi
      dec ecx
      jnz ID2

endm



;------------------------------------
;-- Copybytes
;------------------------------------

copyBytes Macro ID1
      LOCAL ID2,ID3

      test edi,1
      jz ID2
      movsb
      dec ecx
      IFB <ID1>
        jz ID3
      ELSE
        jz ID1
      ENDIF

      ID2:
      shr ecx,1
      rep movsw

      IFB <ID1>
        jnc ID3
      ELSE
        jnc ID1
      ENDIF

      movsb

      ID3:
endm


;------------------------------------
;-- CopySpritebytes
;------------------------------------

copySpriteBytes Macro
      LOCAL ID2,ID3,ID4

    ID4:
      lodssb
      cmp al,255
      jz ID2
      stossb
      dec ecx
      jnz ID4
      jmp ID3

    ID2:
      inc edi
      dec ecx
      jnz ID4
    ID3:
endm

;------------------------------------
;-- Rotatebytes
;------------------------------------
RotateByte Macro
    LOCAL @@keineRot
     cmp al,16
     jb @@keinerot
     cmp al,23
     ja @@keinerot
     add al,rotationValue

   @@keineRot:
endm


CopyRotSpriteBytes Macro
   LOCAL ID1,SetNoByte,Ende
 ID1:
   lodssb
   cmp al,255
   jz SetNoByte
   rotatebyte
   stossb
   dec ecx
   jnz ID1
   jmp ende
 SetNoByte:
   inc edi
   dec ecx
   jnz ID1
 ende:
endm


;------------------------------------
;-- Filterbytes
;------------------------------------

FilterBytes Macro
      LOCAL ID4

    ID4:
      lodssb
      add [edi],al
      inc edi
      dec ecx
      jnz ID4
endm

FilterBytes2 Macro
      LOCAL ID4

    ID4:
      add [edi],al
      inc edi
      dec ecx
      jnz ID4
endm



ProcessBytesLW Macro  counter, func, vidptr, funcparm1, funcparm2
     LOCAL LB1,LB2,LB5

     IFB <counter>
      'Parameter fehlt'
     ENDIF

     IFB <func>
      'Parameter fehlt'
     ENDIF

     IFB <vidptr>
      'Parameter fehlt'
     ENDIF

      push ax
      mov ax,counter
      mov leftr,ax
      pop ax

      add counter, cx

      jc LB5   ; change virtual page

      func funcparm1 funcparm2
      jmp LB1

      LB5:

      xor ecx,ecx
      sub cx, leftr

      func funcparm1 funcparm2

      SetNextVirtualPage vidptr
      mov vidptr, _ActiveGraphModeParameters.LinearAddress

      movzx ecx, counter
      cmp ecx,0
      jz lb1
      func funcparm1 funcparm2

      LB1:
endm

ProcessBytesLW2 Macro  func,  funcparm1, funcparm2
     LOCAL LB1,LB2,LB5

     IFB <func>
      'Parameter fehlt'
     ENDIF

      mov temp, di
      add temp, cx
      
      jc LB5   ; change virtual page

      func funcparm1 funcparm2
      jmp LB1

      LB5:

      sub cx, temp

      func funcparm1 funcparm2

      SetNextVirtualPage2 _hardwaregraphmodeparameters
      mov edi, _hardwaregraphmodeparameters.linearaddress

      mov cx, temp
      cmp cx,0
      jz lb1
      func funcparm1 funcparm2

      LB1:
endm

ProcessBytesLW2s Macro  func,  funcparm1, funcparm2
     LOCAL LB1,LB2,LB5

     IFB <func>
      'Parameter fehlt'
     ENDIF

      mov temp, si
      add temp, cx
      
      jc LB5   ; change virtual page

      func funcparm1 funcparm2
      jmp LB1

      LB5:

      sub cx, temp

      func funcparm1 funcparm2

      SetNextVirtualPage2 _hardwaregraphmodeparameters
      mov esi, _hardwaregraphmodeparameters.linearaddress

      mov cx, temp
      cmp cx,0
      jz lb1
      func funcparm1 funcparm2

      LB1:
endm
