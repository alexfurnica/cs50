/**
 * copy.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./copy n infile outfile\n");
        return 1;
    }

    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];
    
    // remember resize factor
    int n = atoi(argv[1]);

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    BITMAPFILEHEADER bf_copy = bf;
    BITMAPINFOHEADER bi_copy = bi;
    
    bi_copy.biSizeImage = bi.biSizeImage * n;
    bf_copy.bfSize = 54 + bi_copy.biSizeImage;
    bi_copy.biWidth = bi.biWidth * n;
    bi_copy.biHeight = bi.biHeight *n;
    

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf_copy, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi_copy, sizeof(BITMAPINFOHEADER), 1, outptr);

    // determine paddings for scanlines
    int padding_original =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    int padding_copy = (4 - (bi_copy.biWidth * sizeof(RGBTRIPLE))% 4) % 4;
    
    long loop_pos = ftell(inptr); // contains file position used later for vertical resize looping

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        
        for (int vertrept = 0; vertrept < n; vertrept++)
        {
            if (ftell(inptr) != loop_pos)
            {
                fseek(inptr, loop_pos, SEEK_SET);
            }
            
            // iterate over pixels in scanline
            for (int j = 0; j < bi.biWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // write RGB triple to outfile n times (horizontal resize)
                for (int k = 0; k < n; k++)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }
            
            // then add it back (to demonstrate how)
                for (int k = 0; k < padding_copy; k++)
                {
                    fputc(0x00, outptr);
                }
            
            // skip over padding, if any
                fseek(inptr, padding_original, SEEK_CUR);
        }
        
        
        
        loop_pos = ftell(inptr);
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
