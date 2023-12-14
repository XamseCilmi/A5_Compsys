# GCC=gcc -g -Wall -Wextra -pedantic -std=gnu11 
GCC=gcc -g -Wall -Wextra -pedantic -std=gnu11 -O

all: sim
rebuild: clean all
test: test_sim

# test_sim builds all .c files except main.c
test_sim: $(filter-out main.c, $(wildcard *.c))
	$(GCC) $^ -o test_sim
# sim uses simulate
sim: *.c 
	$(GCC) *.c -o sim 

zip: ../src.zip

../src.zip: clean
	cd .. && zip -r src.zip src/Makefile src/*.c src/*.h

clean:
	rm -rf *.o sim  vgcore*
