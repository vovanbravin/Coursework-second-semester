OBJECTS= main.o bmp.o
all: $(OBJECTS) 
	gcc $(OBJECTS) -o cw
	make clean

main.o: main.c
	gcc -c main.c

bmp.o: bmp.c
	gcc -c bmp.c
clean:
	rm *.o
