#include "memory.h"
#include "assembly.h"
#include "read_exec.h"
#include "simulate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
int main(int argc, char *argv[]){
FILE *log_file = NULL;
log_file = fopen(argv[2], "w");
struct memory *mem = memory_create();
struct assembly *as = assembly_create();
int start_addr = read_exec(mem, as, argv[1], log_file);
printf("start: %d, argc: %d\n",start_addr, argc);
return 0;
}