/*
 * Copyright (C) 2001 MontaVista Software Inc.
 * Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <printf.h>
#include <pmap.h>
struct mystruct{
	int size;
	char c;
	int array[1000];
}t1,t2;
int main()
{
	printf("main.c:\tmain is start ...\n");
	t1.size=2,t1.c='c',t1.array[0]=10,t1.array[1]=11;
	t2.size=3,t2.c='Q',t2.array[0]=-1,t2.array[1]=-2,t2.array[2]=-3;
	printf("%T\n",&t1);
	printf("%T\n",&t2);
	printf("%04T\n",&t1);
	printf("%04T\n",&t2);
	mips_init();
	panic("main is over is error!");

	return 0;
}
