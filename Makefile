allocate: allocate.c scheduling.c queue.c
	gcc -Wall -o allocate allocate.c scheduling.c queue.c -lm

clean:
	rm -f allocate