; asm

INCLUDE SETPAGES.INC

_activegraphmodeparameters TActiveGraphModeparameters ?
dummy db 200 DUP (?)
_hardwaregraphmodeparameters TActiveGraphModeparameters ?
dummy4 db 200 DUP (?)
_activefontsettings TFontSettings ?
dummy2 db 200 dup (?)
_mouseparams TMouseSettings ?
dummy3 db 100 dup (?)

_xlatbuffer       dd ?
_dpmscapabilities dd ?
_actdpmsmode      dd ?
_handleractive    dd ?

_activepalette db 768 DUP (?)

PUBLIC _dpmscapabilities
PUBLIC _actdpmsmode
PUBLIC _activegraphmodeparameters
PUBLIC _hardwaregraphmodeparameters
PUBLIC _activefontsettings
PUBLIC _activepalette
PUBLIC _mouseparams
PUBLIC _xlatbuffer
PUBLIC _handleractive


