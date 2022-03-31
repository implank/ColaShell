INCLUDES		:= -I../include/

%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

.PHONY: clean

all: my_cal.o my_driver.o

clean: 
	rm -rf *~ *.o

include ../include.mk
