#include<asm/regdef.h>
#include<asm/cp0regdef.h>
#include<asm/asm.h>

LEAF(_my_exit)
lui s1,0xb000
my_exit:
	sb zero,0x10(s1)
END(_my_exit)
