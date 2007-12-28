VERSION		EQU	7
REVISION	EQU	3

DATE	MACRO
		dc.b '28.12.2007'
		ENDM

VERS	MACRO
		dc.b 'openurl.library 7.3'
		ENDM

VSTRING	MACRO
		dc.b 'openurl.library 7.3 (28.12.2007)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: openurl.library 7.3 (28.12.2007)',0
		ENDM
