#VPATH and vpath both all right to define the source and header files path.
VPATH = src:include

#vpath %.c src
#vpath %.h include

all:test4 tar
.PHONY:all

test4:main.o fun1.o fun2.o 
	gcc  main.o fun1.o fun2.o -o test4 -g

#Remember, when you use "-Iinclude",you also must use "$@ $^"
# Note: don't add fun1.h fun2.h in connected list!
main.o:main.c
	gcc -c -Iinclude -o $@ $^ -g
fun1.o:fun1.c
	gcc -c -Iinclude -o $@ $^ -g
fun2.o:fun2.c
	gcc -c -Iinclude -o $@ $^ -g
tar:
	tar cvf test4.tar src include Makefile
	
.PHONY:clean
clean:
	rm *.o test4
