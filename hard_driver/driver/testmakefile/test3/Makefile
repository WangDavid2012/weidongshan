all:test3 tar
.PHONY:all
CC := gcc
OPTIONS := -Wall
OBJECTS := test3.o
test3:$(OBJECTS)
	$(CC) $(OPTIONS) -o $@ $^
test3.o:test3.c
	gcc -c test3.c
tar:test3.c Makefile
	tar cvf test3.tar test3.c Makefile
.PHONY:clean
clean:
	rm *.o *.tar
