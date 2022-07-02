#include "lib.h"
void umain(int argc, char **argv){
	if(argc!=1){
		fwritef(1,"usage: history\n");
		return;
	}
	int his=open(".history",O_RDONLY|O_CREAT);
	char ch;
	fwritef(1,"1\t");
	int cnt=1;
	int f=0;
	while(read(his,&ch,1)==1){
		if(f){
			fwritef(1,"%d\t",cnt);
			f=0;
		}
		fwritef(1,"%c",ch);
		if(ch=='\n'){
			cnt++;
			f=1;
		}
	}
}