/* This is a simplefied ELF reader.
 * You can contact me if you find any bugs.
 *
 * Luming Wang<wlm199558@126.com>
 */

#include "kerelf.h"
#include <stdio.h>
/* Overview:
 *   Check whether it is a ELF file.
 *
 * Pre-Condition:
 *   binary must longer than 4 byte.
 *
 * Post-Condition:
 *   Return 0 if `binary` isn't an elf. Otherwise
 * return 1.
 */
int is_elf_format(u_char *binary)
{
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)binary;
	if (ehdr->e_ident[EI_MAG0] == ELFMAG0 &&
			ehdr->e_ident[EI_MAG1] == ELFMAG1 &&
			ehdr->e_ident[EI_MAG2] == ELFMAG2 &&
			ehdr->e_ident[EI_MAG3] == ELFMAG3) {
		return 1;
	}

	return 0;
}

/* Overview:
 *   read an elf format binary file. get ELF's information
 *
 * Pre-Condition:
 *   `binary` can't be NULL and `size` is the size of binary.
 *
 * Post-Condition:
 *   Return 0 if success. Otherwise return < 0.
 *   If success, output address of every section in ELF.
 */

/*
	 Exercise 1.2. Please complete func "readelf". 
 */
int readelf(u_char *binary, int size)
{
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)binary;

	int Nr;

	Elf32_Phdr *phdr = NULL;

	u_char *ptr_ph_table = NULL;
	Elf32_Half ph_entry_count;
	Elf32_Half ph_entry_size;


	// check whether `binary` is a ELF file.
	if (size < 4 || !is_elf_format(binary)) {
		printf("not a standard elf format\n");
		return -1;
	}

	// get section table addr, section header number and section header size.
	ptr_ph_table=binary+ehdr->e_phoff;
	ph_entry_count=ehdr->e_phnum;
	ph_entry_size=ehdr->e_phentsize;
	
	// for each section header, output section number and section addr. 
	// hint: section number starts at 0.
	phdr=(Elf32_Phdr *)ptr_ph_table;
	int f=-1;unsigned ans1=0xffffffff;
	for(Nr=0;Nr<ph_entry_count;Nr++){
		Elf32_Phdr *p1=phdr+Nr;
		int l1=p1->p_vaddr;
		int r1=l1+p1->p_memsz;
		for(int j=0;j<ph_entry_count;++j)if(j!=Nr){
			Elf32_Phdr *p2 = phdr+j;
			int l2=p2->p_vaddr;
			int r2=l2+p2->p_memsz;
			int round1=r1>>12,round2=l2>>12;
			if(round1==round2){
				if(r1<=l2){
			//		printf("Overlay at page va : 0x%x\n",l1);
			//		printf("0x%x 0x%x 0x%x\n",r1,l2,r2);
			//		return 0;
					f=0;
					if(l1<ans1)
							ans1=l1;
					
				}
				else {
				//	printf("Conflict at page va : 0x%x\n",l1);
			//		printf("0x%x 0x%x 0x%x\n",r1,l2,r2);
			//		return 0;
					f=1;
					if(l1<ans1)
							ans1=l1;
				}
			}
		}	
	}
	if(f==1)
		printf("Conflict at page va : 0x%x\n",ans1);
	else if(f==0)	
		printf("Overlay at page va : 0x%x\n",ans1);
	else {
	phdr=(Elf32_Phdr *)ptr_ph_table;
	for(Nr=0;Nr<ph_entry_count;Nr++){
		//phdr=(Elf32_Phdr *)(ptr_ph_table+Nr*ph_entry_size);
		printf("%d:0x%x,0x%x\n",Nr,phdr->p_filesz,phdr->p_memsz);
		phdr++;
	}
	}
	return 0;
}

