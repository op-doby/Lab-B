bubbleSort : BubbleSort.o 
	gcc -m32 -g -Wall -o bubbleSort BubbleSort.o 
BubbleSort.o : BubbleSort.c 
	gcc -m32 -g -Wall -c BubbleSort.c
hexaPrint : hexaPrint.o 
	gcc -m32 -g -Wall -o hexaPrint hexaPrint.o 
hexaPrint.o : hexaPrint.c 
	gcc -m32 -g -Wall -c hexaPrint.c
AntiVirus : AntiVirus.o 
	gcc -m32 -g -Wall -o AntiVirus AntiVirus.o 
AntiVirus.o : AntiVirus.c 
	gcc -m32 -g -Wall -c AntiVirus.c
.PHONY: clean all
clean :
	rm -f hexaPrint bubbleSort *.o AntiVirus
all: bubbleSort hexaPrint AntiVirus