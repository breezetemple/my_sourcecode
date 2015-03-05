/*
 * =====================================================================================
 *
 *       Filename:  test_color.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/18/2014 02:04:31 PM
 *       Revision:  none
 *       Compiler:  gcc

 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>

#define max(a, b)  (((a) > (b)) ? (a) : (b))
#define min(a, b)  (((a) < (b)) ? (a) : (b))

void main(void)
{
    unsigned int i, count = 0;
    unsigned char y,u,v;//6:4:4:2
    int r,g,b;//5:6:5
    double Y, Cb, Cr;
    unsigned int rgb = 0;

    for(i = 0 ; i < (0x10000>>2); i++)
    {
        count++;

        y = ((i<<2)&0xFC00)>>10;
        u = ((i<<2)&0x03C0)>>6;
        v = ((i<<2)&0x003C)>>2;

        Y = (double) (y<<2);
        Cb = (double) (u<<4);
        Cr = (double) (v<<4);

        r = (int) (Y + 1.40200 * (Cr - 0x80));
        g = (int) (Y - 0.34414 * (Cb - 0x80) - 0.71414 * (Cr - 0x80));
        b = (int) (Y + 1.77200 * (Cb - 0x80));

        r = max(0, min(255, r));
        g = max(0, min(255, g));
        b = max(0, min(255, b));

        //r = (unsigned char)(1.164383*(y- 16) + 0 + 1.596027*(v - 128)); 
        //g = (unsigned char)(1.164383*(y- 16) - 0.391762*(u - 128) - 0.812969*(v - 128)); 
        //b = (unsigned char)(1.164383*(y- 16) + 2.017230*(u - 128) + 0 );
        
        //printf("\033[32m y : [0x%04x] %x u : %x v : %x \t RGB:[0x%02x%02x%02x] \n\033[0m", i, y, u ,v ,r, g, b);
        //printf("%x %x %x\n", (((unsigned int) r)&0x00F8)<<8, (((unsigned int) g)&0x00FC)<<3, (((unsigned int) b)&0x00F8)>>3);

        rgb = ((((unsigned int) r)&0x00F8)<<8) + ((((unsigned int) g)&0x00FC)<<3) + ((((unsigned int) b)&0x00F8)>>3);

        //printf("565RGB : %x", rgb);

        printf("0x%04x, ", rgb);
        if(count%8 == 0)
            printf("\n");
    }
}
