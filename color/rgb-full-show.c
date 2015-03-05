/*
 * =====================================================================================
 *
 *       Filename:  rgb-full-show.c
 *
 *    Description:  RGB Show
 *
 *        Version:  1.0
 *        Created:  11/19/2014 03:17:18 PM
 *       Revision:  none
 *       Compiler:  gcc

 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>

#define COLOR_FULL (0)  //RGB 0: 5:6:5 1: 8:8:8 

void main(void)
{
    unsigned char r, g, b, count = 0;

#if COLOR_FULL
    for(r = 0 ; r < 0xFF; r++)
    {
        for(g = 0 ; g < 0xFF; g++)
        {
            for(b = 0 ; b < 0xFF; b++)
            {
                count++;

                printf("\e]4;60;rgb:%d/%d/%d\e\\\e[38;5;60m 0x%02x%02x%02x██████\e[m", r, g, b, r, g, b);

                if(count%8 == 0)
                    printf("\n");
            }
        }
    }
#else
    for(r = 0 ; r < 0x1F; r++)
    {
        for(g = 0 ; g < 0x3F; g++)
        {
            for(b = 0 ; b < 0x1F; b++)
            {
                count++;

                //printf("\e]4;60;rgb:%d/%d/%d\e\\\e[38;5;60m 0x%02x%02x%02x██████\e[m", r<<3, g<<2, b<<3, r<<3, g<<2, b<<3);
                printf("\e]4;60;rgb:%d/%d/%d\e\\\e[38;5;60m %02x%02x%02x█████\e[m", r<<3, g<<2, b<<3, r<<3, g<<2, b<<3);

                if(count%8 == 0)
                    printf("\n");
            }
        }
    }
#endif
}
