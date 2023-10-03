#include <stdio.h>
#include <stdint.h>


//8b words to 32b words conversion, 256b data block
void w8_to_w32(uint8_t *x, uint32_t *y)
{
    //printf("\nw8_to_w32\n");
    for(int i=0; i<8; i++)
    {
        *(y+i) = (*(x+i*4) << 24) + (*(x+i*4+1) << 16) + (*(x+i*4+2)  << 8) + *(x+i*4+3);
        //printf("%x ",*(y+i));
    }

}

//8b words to 32b words conversion, 256b data block
void w32_to_w8(uint8_t *x, uint32_t *y)
{

    for(int i=0; i<8; i++)
    {
        *(x+i*4) = (uint8_t)((*(y + i)) >> 24);
        *(x+i*4+1) = (uint8_t)(((*(y + i)) & 0xff0000) >> 16);
        *(x+i*4+2) = (uint8_t)(((*(y + i)) & 0xff00) >> 8);
        *(x+i*4+3) = (uint8_t)(((*(y + i)) & 0xff));
    }

}
