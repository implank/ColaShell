#include "lib.h"
void umain() { 
	u_int me = syscall_getenvid(); 
	while (syscall_try_acquire_console() != 0) { 
		syscall_yield();
	} 
	writef("%d\n",syscall_try_acquire_console());
	writef("I'm %x\n", me); 
	syscall_release_console();
	syscall_release_console();
	while(1);
}
