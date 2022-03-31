#include<asm/regdef.h>
#include<asm/cp0regdef.h>
#include<asm/asm.h>

LEAF(_my_getchar)

loop_begin:
	lui s1,0xb000
	lb t1,0x00(s1)
	beq t1,zero,loop_begin
	nop

addi v0,t1,0
sb t1,0x00(s1)
jal ra

END(_my_getchar)

LEAF(_my_putchar)

lui s1,0xb000
addi t1,a0,0
sb t1,0x00(s1)
jal ra

END(_my_putchar)

LEAF(_my_exit)
lui s1,0xb000
	sb zero,0x10(s1)
END(_my_exit)
