# GCC=gcc -g -Wall -Wextra -pedantic -std=gnu11 
GCC=gcc -g -Wall -Wextra -pedantic -std=gnu11 -O

all: test
rebuild: clean all

# sim uses simulate
test: *.c 
	$(GCC) *.c -o test 


clean:
	rm -rf *.o test  vgcore*
