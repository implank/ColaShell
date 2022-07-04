#include "lib.h"
void printtab(int x){
	while(x--)fwritef(1,"\t");
}
void tree(int d,char *path) {
	struct Fd* fd;
	struct Filefd* fileFd;
	char _path[MAXPATHLEN];
	int i,r;
	if((r=open(path,O_RDONLY))<0)return;
	fd=(struct Fd*)num2fd(r);
	fileFd=(struct FileFd*) fd;
	u_int size=fileFd->f_file.f_size;
	fwritef(1,"\x1b[34m%s\x1b[0m\n",fileFd->f_file.f_name);
	u_int nblock=ROUND(size,sizeof(struct File))/sizeof(struct File);
	struct File *file=(struct File *)fd2data(fd);
	for(i=0;i<nblock;i++){
		if(file[i].f_name[0]=='\0')continue;
		printtab(d);
		fwritef(1,"|---");
		if(file[i].f_type==FTYPE_DIR){
			strcpy(_path,path);
			int len=strlen(path);
			if(_path[len-1]!='/')_path[len++]='/';
			strcat(_path,file[i].f_name);
			tree(d+1,_path);
		}
		else
			fwritef(1,"%s\n",file[i].f_name);
	}
}
void umain(int argc ,char** argv){
	char *dir;
	if(argc>2) {
		fwritef(1,"usage: tree [dir]\n");
		exit();
	}
	dir=(argc==1)?"/":argv[1];
	tree(0,dir);
}