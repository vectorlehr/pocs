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
extern uint8_t* stub;
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

// __attribute__((always_inline)) get_addr(void *func){

// } 

long long int pmc[5][1003];

void train(){
    mprotect(vic, 4096, PROT_READ | PROT_WRITE | PROT_EXEC);
    // mprotect((uint8_t*)&stub, 4096, PROT_READ | PROT_WRITE | PROT_EXEC);
    for(int i = 0; i < 256*4096; ++i)
        probe_array[i] = 1;


    int cond = 1;
    int ncond = 0;
    int rcx = 0x9090;
    int nrcx = 0x9090;
    uint64_t instr1 = 0x9090;
    uint64_t instr2 = 0x9090;


    // print address of vic, safe, target
    printf("pc: %p\n", (uint16_t *)&safe - 1);
    printf("bin: 0x%2x%2x\n", *((uint8_t*)&safe-2), *((uint8_t*)&safe-1));
    printf("target: %p\n", &target);

    while(1){
        vic(1, instr1);
        sched_yield();
    }
}


int main(){
    // spectre_v1_arguments();
    // spectre_v1_smc();
    
    train();
}
