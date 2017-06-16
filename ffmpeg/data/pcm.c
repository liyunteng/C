/*
 * Filename: pcm.c
 * Description:
 *
 * Copyright (C) 2017 liyunteng
 *
 * Author: liyunteng <liyunteng@streamocean.com>
 * License: StreamOcean
 * Last-Updated: 2017/06/16 17:55:46
 */
#include <stdio.h>
#include <stdlib.h>

int split_pcm16le(char *url)
{
    FILE *fp = fopen(url, "rb+");
    FILE *fp1 = fopen("output_1.pcm", "wb+");
    FILE *fp2 = fopen("output_2.pcm", "wb+");

    unsigned char *sample = (unsigned char *)malloc(4);
    while (!feof(fp)) {
        fread(sample, 1, 4, fp);
        fwrite(sample, 1, 2, fp1);
        fwrite(sample+2, 1, 2, fp2);
    }

    free(sample);
    fclose(fp);
    fclose(fp1);
    fclose(fp2);

    return 0;
}

int halfvalueleft_pcm16le(char *url)
{
    FILE *fp = fopen(url, "rb+");
    FILE *fp1 = fopen("output_halfleft.pcm", "wb+");

    unsigned char *sample = (unsigned char *)malloc(4);

    while (!feof(fp)) {
        short *samplenum = NULL;
        fread(samplenum, 1, 4, fp);

        samplenum = (short *)sample;
        *samplenum = *samplenum/2;

        fwrite(sample, 1, 2, fp1);
        fwrite(sample+2, 1, 2, fp1);
    }

    free(sample);
    fclose(fp);
    fclose(fp1);

    return 0;
}

int doublespeed_pcm16le(char *url)
{
    FILE *fp = fopen(url, "rb+");
    FILE *fp1 = fopen("output_doublespeed.pcm", "wb+");

    unsigned char *sample = (unsigned char *)malloc(4);
    int cnt = 0;
    while(!feof(fp)) {
        fread(sample, 1, 4, fp);

        if (cnt%2 != 0) {
            fwrite(sample, 1, 2, fp1);
            fwrite(sample+2, 1, 2, fp1);
        }
        cnt++;
    }

    free(sample);
    fclose(fp);
    fclose(fp1);
    return 0;
}


int pcm16le_to_pcm8(char *url)
{
    FILE *fp = fopen(url, "rb+");
    FILE *fp1 = fopen("output_8.pcm", "wb+");

    unsigned char *sample=(unsigned char *)malloc(4);

    while(!feof(fp)) {
        short *samplenum16=NULL;
        char samplenum8 = 0;
        unsigned char samplenum8_u = 0;
        fread(sample, 1, 4, fp);
        samplenum16 = (short *)sample;
        samplenum8 = (*samplenum16) >> 8;
        samplenum8_u = samplenum8 + 128;

        fwrite(&samplenum8_u, 1, 1, fp1);

        samplenum16 = (short *)(sample + 2);
        samplenum8 = (*samplenum16) >> 8;
        samplenum8_u = samplenum8 + 128;

        fwrite(&samplenum8_u, 1, 1, fp1);
    }

    free(sample);
    fclose(fp);
    fclose(fp1);
    return 0;
}
