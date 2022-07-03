#include "lib.h"
#include <args.h>

int debug_ = 0;
static u_int sh_id=0;

//
// get the next token from string s
// set *p1 to the beginning of the token and
// *p2 just past the token.
// return:
//	0 for end-of-string
//	> for >
//	| for |
//	w for a word
//
// eventually (once we parse the space where the nul will go),
// words get nul-terminated.
#define WHITESPACE " \t\r\n"
#define SYMBOLS "<|>&;()"

int _gettoken(char *s, char **p1, char **p2){
	int t;
	if (s == 0) {
		//if (debug_ > 1) writef("GETTOKEN NULL\n");
		return 0;
	}
	//if (debug_ > 1) writef("GETTOKEN: %s\n", s);
	*p1 = 0;
	*p2 = 0;
	while(strchr(WHITESPACE, *s))
		*s++ = 0;
	if(*s == 0) {
	//	if (debug_ > 1) writef("EOL\n");
		return 0;
	}
	if (*s == '"'){
		*p1 = s + 1;
		s++;
		while (*s && *s != '"')
			s++;
		if (*s == 0)
		{
			writef("\" does not match\n");
			return -1;
		}
		*s = 0;
		s++;
		while (*s && !strchr(WHITESPACE SYMBOLS, *s))
			s++;
		*p2 = s;
		return 'w';
	}
	if(*s=='$'&&*(s+1)&&!strchr(WHITESPACE,*(s+1))){
		*p1=s+1;
		s++;
		while(*s&&!strchr(WHITESPACE,*s))
			s++;
		*s=0;
		*p2=s+1;
		return '$';
	}
	if(strchr(SYMBOLS, *s)){
		if(*s=='>'&&*(s+1)&&*(s+1)=='>'){
			t=*s;
			*p1=s;
			*s=0;
			s+=2;
			*p2=s;
			return 'a';
		}
		t = *s;
		*p1 = s;
		*s++ = 0;
		*p2 = s;
//		if (debug_ > 1) writef("TOK %c\n", t);
		return t;
	}
	*p1 = s;
	while(*s && !strchr(WHITESPACE SYMBOLS, *s))
		s++;
	*p2 = s;
	if (debug_ > 1) {
		t = **p2;
		**p2 = 0;
//		writef("WORD: %s\n", *p1);
		**p2 = t;
	}
	return 'w';
}
int gettoken(char *s, char **p1){
	static int c, nc;
	static char *np1, *np2;
	if (s) {
		nc = _gettoken(s, &np1, &np2);
		return 0;
	}
	c = nc;
	*p1 = np1;
	nc = _gettoken(np2, &np1, &np2);
	return c;
}
int call_inner_instr(int argc, char **argv){
	int r;
	EnvVar list[EV_MAX]={0};
	if(strcmp(argv[0],"clear")==0){
		writef("\x1b[2J\x1b[H");
		return 1;
	}
	if(strcmp(argv[0],"unset")==0){
		if(argc!=2)fwritef(1,"usage: unset [varname]\n");
		else{
			r=syscall_set_env_var(argv[1],0,0,sh_id);
			if(r<0){
				fwritef(1,"env var [%s] don't exist\n",argv[1]);
			}
		}
		return 1;
	}
	if(strcmp(argv[0],"declare")==0){
		if(argc==1){
			syscall_get_env_var_list(list,sh_id);
			int i=0;
			while(list[i].name[0]){
				fwritef(1,"%3d ",i+1);
				fwritef(1,list[i].mode&EV_RDONLY?"-":"w");
				fwritef(1,list[i].mode&EV_GLOBAL?"x":"-");
				fwritef(1," ");
				fwritef(1,"%s=",list[i].name);
				fwritef(1,"%s",list[i].value);
				fwritef(1,"\n");
				i++; 
			}
			return 1;
		}else {
			int exargs=0;
			int mode=0;
			if(argv[1][0]=='-'){
				exargs=1;
				int i;
				for(i=1;argv[1][i];++i){
					if(argv[1][i]=='r')mode|=EV_RDONLY;
					if(argv[1][i]=='x')mode|=EV_GLOBAL;
				}
			}
			r=syscall_set_env_var(argv[exargs+1],argv[exargs+2],mode,sh_id);
			if(r==-E_EV_RDONLY){
				fwritef(1,"env var [%s] is readonly\n",argv[exargs+1]);
			}
			if(r==-E_EV_FULL){
				fwritef(1,"env var reaches the limit\n");
			}
		}
		return 1;
	}
	return 0;
}
#define MAXARGS 16
void runcmd(char *s){
	char *argv[MAXARGS], *t;
	char buf[MAXARGS][EV_VALUEMAXLEN];
	int argc, c, i, r, p[2], fd, rightpipe;
	int fdnum;
	int buf_len=0;
	int hang=0;
	int pid;
	struct Stat state;
	rightpipe = 0;
	gettoken(s, 0);
again:
	argc = 0;
	for(;;){
		c = gettoken(0, &t);
		switch(c){
		case 0:
			goto runit;
		case '&':
			hang=1;
			break;
		case 'w':
			if(argc == MAXARGS){
				writef("too many arguments\n");
				exit();
			}
			argv[argc++] = t;
			break;
		case '$':
			if(argc==MAXARGS){
				writef("too many arguments\n");
				exit();
			}
			if(syscall_get_env_var(t,buf[buf_len],sh_id)<0)
				argv[argc++]=t;
			else
				argv[argc++]=buf[buf_len++];
			break;
		case '<':
			if(gettoken(0, &t) != 'w'){
				writef("syntax error: < not followed by word\n");
				exit();
			}
			r=stat(t,&state);
			if(r<0){
				writef("cannot open file\n");
				exit();
			}
			if(state.st_isdir){
				writef("specified path should be file\n");
				exit();
			}
			fdnum=open(t,O_RDONLY);
			dup(fdnum,0);
			close(fdnum);
			break;
		case '>':
			if(gettoken(0,&t)!='w'){
				writef("syntax error: > not followed by word\n");
				exit();
			}
			r=stat(t, &state);
			if(r>=0&&state.st_isdir){
				writef("specified path should be file\n");
				exit();
			}
			fdnum=open(t,O_WRONLY|O_CREAT);
			dup(fdnum,1);
			close(fdnum);
			break;
		case 'a':
			if(gettoken(0,&t)!='w'){
				writef("syntax error: >> not followed by word\n");
				exit();
			}
			r=stat(t,&state);
			if(r>=0&&state.st_isdir){
				writef("specified path should be file\n");
				exit();
			}
			fdnum=open(t,O_WRONLY|O_CREAT|O_APPEND);
			dup(fdnum,1);
			close(fdnum);
			break;
		case '|':
			pipe(p);
			if((r=fork())<0) {
				writef(" | error\n");
				exit();
			} 
			else if(r==0) {
				dup(p[0],0);
				close(p[0]);
				close(p[1]);
				goto again;
			} 
			else {
				dup(p[1],1);
				close(p[1]);
				close(p[0]);
				rightpipe=r;
				goto runit;
			}
			// user_panic("| not implemented");
			break;
		case ';':
			if ((pid=fork())==0){
				hang=0;
				goto runit;
			}
			wait(pid);
			argc=0;
			hang=0;
			buf_len = 0;
			rightpipe = 0;
			do{
				close(0);
				if ((r = opencons()) < 0)
					user_panic("opencons: %e", r);
			} while (r != 0);
			dup(0, 1);
			break;
		}
	}

runit:
	if(argc == 0) {
		if (debug_) writef("EMPTY COMMAND\n");
		return;
	}
	argv[argc] = 0;
	if (debug_) {
		writef("[%08x] SPAWN:", env->env_id);
		for (i=0; argv[i]; i++)
			writef(" %s", argv[i]);
		writef("\n");
	}
	if(call_inner_instr(argc,argv)){
		exit();
	}
	if ((r = spawn(argv[0], argv)) < 0){
		writef("spawn %s: %e\n", argv[0], r);
		//exit();
	}
	close_all();
	if (r >= 0) {
		// writef("hang:%d\n",hang);
		if (debug_) writef("[%08x] WAIT %s %08x\n", env->env_id, argv[0], r);
		if(!hang)
			wait(r);
		else{
			writef("\x1b[33m[%08x]\x1b[0m\t", r);
			for (i = 0; i < argc; ++i)
				writef("%s ", argv[i]);
			writef("\n");
			int pid = fork();
			if (pid == 0){
				wait(r);
				writef("\x1b[33m[%08x]\x1b[35m\tDone\x1b[0m\n", r);
				exit();
			}
		}
	}
	if (rightpipe) {
		if (debug_) writef("[%08x] WAIT right-pipe %08x\n", env->env_id, rightpipe);
		wait(rightpipe);
	}
	exit();
}
static int newcmd = 0;
static void save_cmd(char *buf){
	if (!*buf)return;
	newcmd=0;
	int his=open(".history",O_APPEND|O_RDWR|O_CREAT);
	if (his<0){
		fwritef(1, "cannot open history\n");
		return;
	}
	fwritef(his, "%s\n", buf);
	close(his);
}

static int hist(char *buf, int up){
	static int last=0;
	static char history_info[4*1024*1024];
	if(!newcmd){
		struct Stat state;
		if(stat(".history",&state)<0)return 0;
		int his=open(".history",O_RDONLY);
		if(his<0)return 0;
		read(his,history_info,state.st_size);
		history_info[state.st_size]=0;
		close(his);
	}
	int oi, i, j, offset;
	if (newcmd <= 1 && !up)
	{
		newcmd=0;
		last=newcmd==1?up:0;
		return 0;
	}
	if(!last){
		offset=newcmd+(up?1:-1);
	}
	else{
		if(last>0){
			if(up)
				return 0;
			else
				offset = newcmd;
		}
		else{
			if(!up)
				return 0;
			else
				offset = newcmd;
		}
	}
	for(j=0;history_info[j]&&offset!=0;++j){
		if(history_info[j]=='\n'){
			offset--;
		}
	}
	if(offset!=0){
		last=up?1:-1;
		return 0;
	}
	for (oi = i = 0; history_info[j]; ++j){
		if (history_info[j] == '\n'){
			oi = i;
			while (history_info[i] != '\n')
				++i;
			++i;
		}
	}
	char *ptr = buf;
	for (;; i++)
	{
		if (history_info[i] == '\n')
		{
			*ptr = 0;
			break;
		}
		*ptr = history_info[i];
		++ptr;
	}
	fwritef(1, "%s", buf);
	if (!last)
	{
		if (up)
			newcmd++;
		else
			newcmd--;
	}
	last = 0;
	return ptr - buf;
}
void readline(char *buf, u_int n){
	int i, r;
	char tmp;
	r = 0;
	for(i=0; i<n; i++){
		if((r = read(0, buf+i, 1)) != 1){
			if(r < 0)
				writef("read error: %e", r);
			exit();
		}
		if(buf[i] == 127){
			if(i > 0){
				fwritef(1,"\x1b[1D\x1b[0K");
				i -= 2;
			}
			else
				i = 0;
		}else if(buf[i]=='\x1b'){
			read(0,&tmp,1);
			if(tmp!='[')
				user_panic("\\x1b is not followed by '['");
			read(0,&tmp,1);
			if(tmp=='A'){
				if(i)
					fwritef(1, "\x1b[1B\x1b[%dD\x1b[K", i);
				else
					fwritef(1, "\x1b[1B");
				i = hist(buf, 1);
			}
			else if (tmp == 'B'){
				if (i)
					fwritef(1, "\x1b[1A\x1b[%dD\x1b[K", i);
				else
					fwritef(1, "\x1b[1A");
				i = hist(buf, 0);
			}
			else if (tmp == 'C'){
				fwritef(1,"\x1b[1D\x1b[1D");
			}
			else if (tmp == 'D'){
				fwritef(1,"\033[2C");
			}
			i--;
		}
		else if(buf[i] == '\r' || buf[i] == '\n'){
			buf[i] = 0;
			return;
		}
	}
	writef("line too long\n");
	while((r = read(0, buf, 1)) == 1 && buf[0] != '\n');
	buf[0] = 0;
}	
char buf[1024];
void usage(void){
	writef("usage: sh [-dix] [command-file]\n");
	exit();
}
void umain(int argc, char **argv){
	sh_id=env->env_id;
	int r, interactive, echocmds;
	interactive = '?';
	echocmds = 0;
	// writef("\n:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	// writef("::                                                         ::\n");
	// writef("::              Super Shell  V0.0.0_1                      ::\n");
	// writef("::                                                         ::\n");
	// writef(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	writef("	[0;37m              [0;1;34m__[0;37m           [0;34m__[0;37m         ____[0m\n\
	[0;37m  [0;1;34m______[0;34m___[0;37m  [0;34m/[0;37m [0;34m____[0;37m [0;34m____[0;37m__/ /_  ___  / / [0;1;30m/[0m\n\
	[0;37m [0;34m/[0;37m [0;34m___/[0;37m [0;34m__[0;37m [0;34m\\/[0;37m [0;34m/[0;37m __ `/ ___/ __ \\/ [0;1;30m_[0;37m [0;1;30m\\/[0;37m [0;1;30m/[0;37m [0;1;30m/[0;37m [0m\n\
	[0;34m/[0;37m [0;34m/__/[0;37m [0;34m/[0;37m_/ / / /_/ (__  [0;1;30m/[0;37m [0;1;30m/[0;37m [0;1;30m/[0;37m [0;1;30m/[0;37m  [0;1;30m__/[0;37m [0;1;30m/[0;37m [0;1;30m/[0;37m  [0m\n\
	[0;37m\\___/\\____/_/\\__[0;1;30m,_/____/_/[0;37m [0;1;30m/_/\\_[0;1;34m__/_/_/[0;37m   [0m\n\
	[0;37m                                          [0m");
	
	
	ARGBEGIN{
	case 'd':
		debug_++;
		break;
	case 'i':
		interactive = 1;
		break;
	case 'x':
		echocmds = 1;
		break;
	default:
		usage();
	}ARGEND

	if(argc > 1)
		usage();
	if(argc == 1){
		close(0);
		if ((r = open(argv[1], O_RDONLY)) < 0)
			user_panic("open %s: %e", r);
		user_assert(r==0);
	}
	if(interactive == '?')
		interactive = iscons(0);
	for(;;){
		if (interactive)
			fwritef(1,"\n\x1b[36m%d\x1b[0m\x1b[32m$\x1b[0m ",sh_id);
		readline(buf, sizeof buf);
		if(!*buf)continue;
		save_cmd(buf);
		if (buf[0] == '#')
			continue;
		if (echocmds)
			fwritef(1, "# %s\n", buf);
		if ((r = fork()) < 0)
			user_panic("fork: %e", r);
		if (r == 0) {
			runcmd(buf);
			exit();
			return;
		} else
			wait(r);
	}
}

