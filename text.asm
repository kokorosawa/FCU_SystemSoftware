INLOOP  TD    INDEV   // 測試輸入裝置
        JEQ   INLOOP  // 迴圈直到輸入裝置準備好
		JSUB  READ
		JSUB  WRITE		// 讀一個字元進入A暫存器
.
.
READ		LDX   ZREO
RLOOP	RD    INDEV
        STCH  DATA, X 	// 把A暫存器的字元存入DATA
		TIX   FIVE
		JLT   RLOOP
		RSUB
.
.
WRITE	LDX   ZREO
WLOOP   LDCH DATA, X
		COMP LOWER
		JGT  W
		ADD  UTL
W		WD   OUTDEV
		TIX  FIVE
		JLT  WLOOP
		RSUB
.
.
.
.
INDEV  BYTE X'F1'   // 輸入裝置代號F1
OUTDEV BYTE X'05'   // 輸出裝置代號05
DATA   RESB  10
ZREO   WORD 0
FIVE   WORD 5
LOWER  WORD 96
UTL    WORD 32