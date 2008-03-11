VERSION EQU 7
REVISION EQU 4
DATE MACRO
    dc.b '5.1.2008'
    ENDM
VERS MACRO
    dc.b 'openurl.library 7.4'
    ENDM
VSTRING MACRO
    dc.b 'openurl.library 7.4 (5.1.2008)',13,10,0
    ENDM
VERSTAG MACRO
    dc.b 0,'$VER: openurl.library 7.4 (5.1.2008)',0
    ENDM
