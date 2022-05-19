#include "lib.h"


void umain()
{
	int a = 0;
	int id = 0;

	if ((id = fork()) == 0) {
		if ((id = fork()) == 0) {
			a += 3;
			for (;;) {
//				while (syscall_try_acquire_console() != 0) {
  //      syscall_yield();
   // }
				writef("\t\tthis is child2 :a:%d\n", a);
		//		syscall_release_console();
			}
		}
	
		a += 2;
		
		for (;;) {
	//		while (syscall_try_acquire_console() != 0) {
   //     syscall_yield();
   // }
			writef("\tthis is child :a:%d\n", a);
		//	syscall_release_console();
		}
	}

	a++;

	for (;;) {
	//	while (syscall_try_acquire_console() != 0) {
  //      syscall_yield();
  //  }
			writef("this is father: a:%d\n", a);
	//	syscall_release_console();
	}
}
