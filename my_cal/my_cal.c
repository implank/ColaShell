void my_cal(){
	long long num=0;
	char c;
	while(1){
		c=_my_getchar();
		if('0'<=c&&c<='9');
		else break;
		num=num*10+c-'0';
	}
	int i=0,f=0;
	for(i=31;i>=0;--i){
		if((num>>i)&1){
			_my_putchar('1');
			f=1;
		}
		else if(f)
			_my_putchar('0');
	}
		return ;
}
