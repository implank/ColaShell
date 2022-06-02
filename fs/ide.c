/*
 * operations on	IDE disk.
 */

#include "fs.h"
#include "lib.h"
#include <mmu.h>


int raid4_valid(u_int diskno){
	int tmp = diskno;//diskno
	if(syscall_write_dev(&tmp,0x13000010,4))user_panic("Error occurred during read the IDE disk!\n");
	tmp = 0;//offset
	if(syscall_write_dev(&tmp,0x13000000,4))user_panic("Error occurred during read the IDE disk!\n");
	tmp = 0;//read status
	if(syscall_write_dev(&tmp,0x13000020,4))user_panic("Error occurred during read the IDE disk!\n");
	if(syscall_read_dev(&tmp,0x13000030,4))user_panic("Error occurred during read the IDE disk!\n");
	return tmp;
}
int raid4_write(u_int blockno, void *src){
	int i,j,f=0;
	u_char data[6][2*512];
	user_bzero(data,6*2*512);
	for(i=1;i<=4;++i)
		user_bcopy(src+512*(i-1),&data[i][0],512),
		user_bcopy(src+2048+512*(i-1),&data[i][512],512);
	for(i=0;i<512;++i){
		int res=0;
		for(j=1;j<=4;++j)
			res^=data[j][i];
		data[5][i]=res;
		res=0;
		for(j=1;j<=4;++j)
			res^=data[j][i+512];
		data[5][i+512]=res;
	}
	for(i=1;i<=5;++i){
		if(!raid4_valid(i)){
			f++;
		}
		else {
			ide_write(i,blockno*2,&data[i][0],1);
			ide_write(i,blockno*2+1,&data[i][512],1);
		}
	}
	return f;
}
int raid4_read(u_int blockno, void *dst){
	int i,j,f=0;
	u_char data[6][2*512];
	user_bzero(data,6*2*512);
	for(i=1;i<=5;++i){
		if(!raid4_valid(i))
			f++;
		else {
			ide_read(i,blockno*2,&data[i][0],1);
			ide_read(i,blockno*2+1,&data[i][512],1);
		}
	}
	if(!f){
		int cor=1;
		for(i=0;i<512;++i){
			int res=0;
			for(j=1;j<=4;++j)
				res^=data[j][i];
			if(data[5][i]!=res)cor=0;
			res=0;
			for(j=1;j<=4;++j)
				res^=data[j][i+512];
			if(data[5][i+512]!=res)cor=0;
		}
		for(i=1;i<=4;++i){
			user_bcopy(&data[i][0],dst+512*(i-1),512);
			user_bcopy(&data[i][512],dst+2048+512*(i-1),512);
		}
		if(cor)return 0;
		else return -1;
	}
	else if(f==1){
		if(!raid4_valid(5)){
			for(i=1;i<=4;++i){
				user_bcopy(&data[i][0],dst+512*(i-1),512);
				user_bcopy(&data[i][512],dst+2048+512*(i-1),512);
			}
		}
		else {
			int inv=0;
			for(i=1;i<=4;++i)if(!raid4_valid(i))inv=i;
			for(i=0;i<512;++i){
				int res=0;
				for(j=1;j<=5;++j)if(j!=inv)
					res^=data[j][i];
				data[inv][i]=res;
				res=0;
				for(j=1;j<=5;++j)if(j!=inv)
					res^=data[j][i+512];
				data[inv][i+512]=res;
			}
			for(i=1;i<=4;++i){
				user_bcopy(&data[i][0],dst+512*(i-1),512);
				user_bcopy(&data[i][512],dst+2048+512*(i-1),512);
			}
		}
	}
	return f;
}
// Overview:
// 	read data from IDE disk. First issue a read request through
// 	disk register and then copy data from disk buffer
// 	(512 bytes, a sector) to destination array.
//
// Parameters:
//	diskno: disk number.
// 	secno: start sector number.
// 	dst: destination for data read from IDE disk.
// 	nsecs: the number of sectors to read.
//
// Post-Condition:
// 	If error occurrs during the read of the IDE disk, panic.
//
// Hint: use syscalls to access device registers and buffers
/*** exercise 5.2 ***/
void ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs){
	// 0x200: the size of a sector: 512 bytes.
	int offset_begin = secno * 0x200;
	int offset_end = offset_begin + nsecs * 0x200;
	int offset = 0;
	int tmp;
	while (offset_begin + offset < offset_end) {
		tmp = diskno;
		if(syscall_write_dev(&tmp,0x13000010,4))user_panic("Error occurred during read the IDE disk!\n");
		tmp = offset_begin + offset;
		if(syscall_write_dev(&tmp,0x13000000,4))user_panic("Error occurred during read the IDE disk!\n");
		tmp = 0;
		if(syscall_write_dev(&tmp,0x13000020,4))user_panic("Error occurred during read the IDE disk!\n");
		if(syscall_read_dev(&tmp,0x13000030,4))user_panic("Error occurred during read the IDE disk!\n");
		if(tmp==0)user_panic("Error occurred during read the IDE disk!\n");
		if(syscall_read_dev(dst+offset,0x13004000,512))user_panic("Error occurred during read the IDE disk!\n");
		offset += 0x200;
	}
}
// Overview:
// 	write data to IDE disk.
//
// Parameters:
//	diskno: disk number.
//	secno: start sector number.
// 	src: the source data to write into IDE disk.
//	nsecs: the number of sectors to write.
//
// Post-Condition:
//	If error occurrs during the read of the IDE disk, panic.
//
// Hint: use syscalls to access device registers and buffers
/*** exercise 5.2 ***/
void ide_write(u_int diskno, u_int secno, void *src, u_int nsecs){
	int offset_begin = secno * 0x200;
	int offset_end = offset_begin + nsecs * 0x200;
	int offset = 0;
	// DO NOT DELETE WRITEF !!!
	writef("diskno: %d\n", diskno);
	int tmp;
	while (offset_begin + offset < offset_end) {
		if(syscall_write_dev(src+offset,0x13004000,512))user_panic("Error occurred during write the IDE disk!\n");
		tmp=diskno;
		if(syscall_write_dev(&tmp,0x13000010,4)) user_panic("Error occurred during write the IDE disk!\n");
		tmp=offset_begin + offset;
		if(syscall_write_dev(&tmp,0x13000000,4)) user_panic("Error occurred during write the IDE disk!\n");
		tmp=1;
		if(syscall_write_dev(&tmp,0x13000020,4)) user_panic("Error occurred during write the IDE disk!\n");
		if(syscall_read_dev(&tmp,0x13000030,4))user_panic("Error occurred during write the IDE disk!\n");
		if(tmp==0)user_panic("Error occurred during write the IDE disk!\n");
		offset+=0x200;
	}
}
