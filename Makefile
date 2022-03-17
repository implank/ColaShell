make: hello_os.c
	gcc -o os_hello hello_os.c
clean: os_hello
	rm os_hello
