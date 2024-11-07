#include "simulator.h"
#include <time.h>

#include <stdio.h>

int cyclic_test(int b, int s, int w, int repeats, int size){
    sim_start(b, s, w);

    long long addr = 0;

    for(int i = 0; i < repeats; i++){
        for(int j = 0; j <= size; j++){
            addr = 4 * j;
            sim_access(addr);
        }
    }

    int misses = sim_finish();

    return misses;
}

int sawtooth_test(int b, int s, int w, int repeats, int size){
    sim_start(b, s, w);

    long long addr = 0;

    for(int i = 0; i < repeats; i++){
        if(i % 2 == 0){
            for(int j = 0; j <= size; j++){
                addr = 4 * j;
                sim_access(addr);
            }
        }
        else{
            for(int j = 0; j <= size; j++){
                addr = 4 * (size - j);
                sim_access(addr);
            }
        }
    }

    int misses = sim_finish();
    
    return misses;
}

void main() {
    clock_t start, end;
    double cpu_time_used;

    int b = 6;
    int s = 11;
    int w = 3;

    int m = 1000000;

    int repeats = 10;

    start = clock();
    int cyclic_misses = cyclic_test(b, s, w, repeats, m);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Cyclic misses are: %d\n", cyclic_misses);
    printf("Time taken for Cyclic test: %f seconds\n", cpu_time_used);

    start = clock();
    int sawtooth_misses = sawtooth_test(b, s, w, repeats, m);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Sawtooth misses are: %d\n", sawtooth_misses);
    printf("Time taken for Sawtooth test: %f seconds\n", cpu_time_used);

    return;
}