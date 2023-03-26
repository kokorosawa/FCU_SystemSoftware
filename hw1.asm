INLOOP  TD    INDEV   
        JEQ   INLOOP  
	JSUB  READ
        LDA   COUNT
        SUB   ONE
        STA   COUNT
	JSUB  WRITE
.
.
READ	LDX   ZREO
RLOOP	RD    INDEV
        STCH  DATA, X 	
	LDA   COUNT
	ADD   ONE
        STA   COUNT
        LDCH  DATA, X
        COMP  ENDSS
        STX   INDEX
        LDA   INDEX
        ADD   ONE
        STA   INDEX
        LDX   INDEX
        JGT   RLOOP
        RSUB
.
.
WRITE	LDX  ZZ
WLOOP   LDCH DATA, X
	COMP LOWER
	JGT  W
        COMP NUM
        JLT  W
	ADD  UTL
W	WD   OUTDEV
	TIX  COUNT
	JLT  WLOOP
	RSUB
.
.
.
.
NUM    WORD 58
INDEV  BYTE X'F1'  
OUTDEV BYTE X'F2'   
DATA   RESB 100
ZREO   WORD 0
FIVE   WORD 5
LOWER  WORD 96
UTL    WORD 32
COUNT  WORD 0
ONE    WORD 1
ENDSS  WORD 36
INDEX  WORD 0
ZZ     WORD 0
