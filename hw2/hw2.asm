WIDL  LDA   IDLIDX
      ADD   ASCII
      WD    OUTDEV
      LDA   #9
      WD    OUTDEV
      RSUB
.
.
W     LDA   SECIDX
      MUL   FIRIDX
      COMP  #10
      JLT   LESS
      STA   TEMP
      DIV   #10
      STA   TENDG
      LDA   TENDG
      ADD   ASCII
      WD    OUTDEV
      LDA   TENDG
      MUL   #10
      STA   TEN
      LDA   TEMP
      SUB   TEN
      ADD   ASCII
      WD    OUTDEV
      J     TAB
LESS  ADD   ASCII  //less than ten
      WD    OUTDEV
TAB   LDA   #9
      WD    OUTDEV
      RSUB
.
.
MAIN  TD    INDEV   
      JEQ   MAIN
      LDS   #10
      JSUB  IDL
      END   MAIN
.
.
IDL   LDA   #9
      WD    OUTDEV
LOOP  LDX   IDLIDX
      JSUB  WIDL
      TIXR  S
      STX   IDLIDX
      JLT   LOOP
      LDA   #10
      WD    OUTDEV
.
.
FIRLP LDX   FIRIDX
      LDA   FIRIDX
      ADD   ASCII
      WD    OUTDEV
      LDA   #9
      WD    OUTDEV
SECLP LDX   SECIDX //start secloop
      JSUB  W
      TIXR  S
      STX   SECIDX
      JLT   SECLP    //end secloop
      LDA   #1
      STA   SECIDX
      LDA   #10
      WD    OUTDEV
      LDX   FIRIDX
      TIXR  S
      STX   FIRIDX
      JLT   FIRLP
.
.
.
.
TEN    RESW 1
TENDG  RESW 1
TEMP   RESW 1
IDLIDX WORD 1
ASCII  WORD 48
FIRIDX WORD 1
SECIDX WORD 1
INDEV  BYTE X'F1'  
OUTDEV BYTE X'F2'   