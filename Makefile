# Main makefile
#
# Copyright (C) 2007 Beihang University
# Written by Zhu Like ( zlike@cse.buaa.edu.cn )
#

drivers_dir	  := drivers
boot_dir	  := boot
user_dir	  := user
init_dir	  := init
lib_dir		  := lib
fs_dir		  := fs
mm_dir		  := mm
tools_dir	  := tools
vmlinux_elf	  := gxemul/vmlinux

link_script   := $(tools_dir)/scse0_3.lds

modules		  := boot drivers init lib mm user fs
objects		  := $(boot_dir)/start.o			  \
				 $(init_dir)/*.o 				  \
			   	 $(drivers_dir)/gxconsole/console.o \
				 $(lib_dir)/*.o				  \
				 $(user_dir)/*.x \
				 $(fs_dir)/*.x \
				 $(mm_dir)/*.o

.PHONY: all $(modules) clean

all: $(modules) vmlinux

vmlinux: $(modules)
	$(LD) -o $(vmlinux_elf) -N -T $(link_script) $(objects)

$(modules): 
	$(MAKE) --directory=$@

clean: 
	for d in $(modules);	\
		do					\
			$(MAKE) --directory=$$d clean; \
		done; \
	rm -rf *.o *~ $(vmlinux_elf) gxemul/*.img
run:
	/OSLAB/gxemul -E testmips -C R3000 -M 64 -d 0:gxemul/fs.img -d 1:gxemul/fs1.img -d 2:gxemul/fs2.img -d 3:gxemul/fs3.img -d 4:gxemul/fs4.img -d 5:gxemul/fs5.img gxemul/vmlinux
bug:
	/OSLAB/gxemul -E testmips -C R3000 -M 64 -d gxemul/fs.img $(vmlinux_elf) -V
include include.mk
