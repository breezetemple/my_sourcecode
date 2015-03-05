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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <string.h>
#include <unistd.h>


#define COLOR_FULL (0)  //RGB 0: 5:6:5 1: 8:8:8 


//int main(unsigned int RGB)   //RGB : RRGGBB
int main(int argc,char *argv[])
{
    unsigned char r, g, b, count = 0;
    int opt;
    unsigned int RGB;

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
    while((opt = getopt(argc,argv,"s:r:w")) !=-1 )
    {
        switch(opt)
        {
            case 's':   //show RGB
                RGB = strtoul(optarg,NULL,0);
                printf("RGB : 0x%08x\n", RGB);
                r = (unsigned char)(((RGB>>16) & 0xF8)>>3);
                g = (unsigned char)(((RGB>>8) & 0xFC)>>2);
                b = (unsigned char)(((RGB>>0) & 0xF8)>>3);
                printf("%x %x %x\n", r, g, b);

                printf("\e]4;60;rgb:%d/%d/%d\e\\\e[38;5;60m %02x%02x%02x█████ \n\e[m", r<<3, g<<2, b<<3, r<<3, g<<2, b<<3);
                break;

            default:
                break;
        }
    }
#endif
}
