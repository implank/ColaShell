#include<stdio.h>
int main()
{
	  int n;
	scanf("%d",&n);
	int a[10]={0},l=0;
	while(n){
		a[++l]=n%10;
		n/=10;
	}
	int f=1;
	for(int i=1;i<=l;++i){
		if(a[i]!=a[l-i+1])
			f=0;
	}
	if(f){
		printf("Y");
	}else{
		printf("N");
	}
	return 0;
}
