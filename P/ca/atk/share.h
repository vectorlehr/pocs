#ifndef _SHARE_H_
#define _SHARE_H_
#include <stdio.h>
#include <stdint.h>

#include "../../../ptedit_header.h"


// extern uint8_t* oracles;
extern uint8_t* data_RW;


int setup_pages(void){
    // Initialize PTEditor to manipulate page table entries
    // if(ptedit_init()) {
    //     printf("Could not initialize ptedit (did you load the kernel module?)\n");
    //     return 1;
    // }

    uint8_t *ptr = NULL;

    for(int i = 0; i < 3; ++i){
        ptr = (uint8_t *)&data_RW + i*4096;
        // ptr[0] = ptr[0] + 1 - 1;
        // ptedit_pte_clear_bit(ptr, 0, PTEDIT_PAGE_BIT_GLOBAL);
        ptedit_pte_set_bit(ptr, 0, PTEDIT_PAGE_BIT_GLOBAL);
    }
    


    return 0;
}

#endif