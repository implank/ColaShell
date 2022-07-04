#include "lib.h"

int get_num()
{
	int num = 0;
	char ch;
	while (read(0, &ch, sizeof(char)) == sizeof(char))
	{
		if (ch == '\n' || ch == '\r')
			break;
		num = (num << 1) + (num << 3) + ch - '0';
	}
	return num;
}

void umain(int argc, char **argv)
{
	u_int max = get_num(); // positive number
	max=20000000;
	int sum = 0;
	for (; max > 0; max--)
		sum += max;
	fwritef(1, "%d\n", sum);
}