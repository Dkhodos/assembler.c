STR: .asciz "aBcd"
MAIN: add $3,5,$9
LOOP: ori 10,-5,$2
 jmp Next
Next: move $20,$4
LIST: .db 6,-9
 bgt $4,$2,END
 la K
 sw $10
 bne $31,$9, LOOP
 jmp $4
 .dh 27056.15
K: .dw 31,16, 33
END: stop
