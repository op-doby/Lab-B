AntiVirus : AntiVirus.o 
	gcc -m32 -g -Wall -o AntiVirus AntiVirus.o 
AntiVirus.o : AntiVirus.c 
	gcc -m32 -g -Wall -c AntiVirus.c
.PHONY: clean all
clean :
	rm -f *.o AntiVirus
all: AntiVirus