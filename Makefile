# Main makefile
#
# Copyright (C) 2007 Beihang University
# Written by Zhu Like ( zlike@cse.buaa.edu.cn )
#

drivers_dir	  := drivers
boot_dir	  := boot
init_dir	  := init
my_cal_dir	:= my_cal
lib_dir		  := lib
tools_dir	  := tools
test_dir          :=
vmlinux_elf	  := gxemul/vmlinux

link_script   := $(tools_dir)/scse0_3.lds

modules		  := boot drivers init my_cal lib $(test_dir)
objects		  := $(boot_dir)/start.o			  \
				 $(init_dir)/main.o			  \
				 $(init_dir)/init.o			  \
			   	 $(drivers_dir)/gxconsole/console.o \
				 $(lib_dir)/*.o \
				 $(my_cal_dir)/*.o

ifneq ($(test_dir),)
objects :=$(objects) $(test_dir)/*.o
endif


.PHONY: all $(modules) clean

all: $(modules) vmlinux

vmlinux: $(modules)
	$(LD) -o $(vmlinux_elf) -N -T $(link_script) $(objects)
#	tips: add instruction here for your running

$(modules): 
	$(MAKE) --directory=$@

clean: 
	for d in $(modules);	\
		do					\
			$(MAKE) --directory=$$d clean; \
		done; \
	rm -rf *.o *~ $(vmlinux_elf)

run:
	/OSLAB/gxemul -E testmips -C R3000 -M 64 $(vmlinux_elf)

push:
	git push origin lab1-2-Extra:lab1-2-Extra

include include.mk
