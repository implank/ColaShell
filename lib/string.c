// String routines.  Not hardware optimized, but not shabby.

#include <mmu.h>
#include <error.h>
#include <env.h>
#include <kerelf.h>
#include <sched.h>
#include <pmap.h>
#include <printf.h>

int
strlen(const char *s)
{
	int n;

	for (n = 0; *s; s++) {
		n++;
	}

	return n;
}

char *
strcpy(char *dst, const char *src)
{
	char *ret;

	ret = dst;

	while ((*dst++ = *src++) != 0)
		;

	return ret;
}

const char *
strchr(const char *s, char c)
{
	for (; *s; s++)
		if (*s == c) {
			return s;
		}

	return 0;
}

char *strcat(char *dst, const char *src)
{
	char *ret = dst;
	while (*ret)
		ret++;
	while ((*ret++ = *src++) != 0)
		;
	return dst;
}

int
strcmp(const char *p, const char *q)
{
	while (*p && *p == *q) {
		p++, q++;
	}

	if ((u_int)*p < (u_int)*q) {
		return -1;
	}

	if ((u_int)*p > (u_int)*q) {
		return 1;
	}

	return 0;
}

