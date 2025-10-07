
/***************************************************************************
*** Copyright (c) Meeami Technologies
*** All rights reserved
***
*** This software embodies materials and concepts which are confidential
*** to Meeami Technologies and is made available solely pursuant to the terms of a
*** written license agreement with Meeami Technologies.
***
*** File Name: mt_cv_ss_test.c
***
*** Module Name: ss
***
*** File Description:
***
*** List of functions:
*** This file contains following functions for the ss module
***
***
***
*** Author   : P Tejaswini
***
*** Rev History:
***Sl   By                      date                  Version Name                       change details
***
*
**************************************************************************
*END*********************************************************************/

/* Include system level header files */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define MT_PROFILE 1
#if (defined WIN32 || defined _WIN64 || defined _WIN32)
#include <winsock2.h>
#include <windows.h>
#else
#include <time.h>
#include <sys/time.h>
#endif
/* High level header files */
#include "mt_cv_ss_api.h"

/*Local function definition*/
int mt_cv_float_to_short(float* ip_p, short* op_p, int ipsmpcnt);
int mt_cv_short_to_float(short* ip_p, float* op_p, int ipsmpcnt);

int mt_cv_read_wave_hdr(FILE* fp, short* NumChannels, short* BitsPerSample,
    int* ByteRate, int* SamplingRate, short* BlockAlign, int* DataSize, short* audioFormat);

int mt_cv_write_wave_hdr(FILE* fp, short NumChannels, short BitsPerSample,
    int ByteRate, int SamplingRate, short BlockAlign, int DataSize, short audioFormat);

#if MT_PROFILE 
double PCFreq;
unsigned int s_time, e_time;
float  time_elapsed_ms;
float t_time_elapsed_ms = 0;
float  boardclock = 2200;
double mHz;
double sumMhz = 0;
int    sgmnts = 0;
float mHz_g;
float mTime_g;
unsigned int mt_cv_ss_get_time_usec();
#endif

int frm_cnt = 1;
FILE* outputFile;
int main(int argc, char* argv[])
{
    /* Varaible Declaration */
    char* ipfilename;
    FILE* ipfp = NULL, * opfp = NULL;
    int* mvns_obj_p;
    int   status_l = 0;
    int   op_sampcnt = 0;
    int   framesz, framesz_ms, mseg_frcnts;
    short* ipbuf;
    short* opbuf;
    short* opbuf2;
    float* fipbuf;
    float* fopbuf;
    int   i_l;
    int   totopcnt = 0, totipcnt = 0;
    int   readsamples;
    int   baltobecpd = 0;
    int   frcnt = 0;
    int   opSamplingRate;
    int   opDataSize;
    float DataSize_sec;
    float ipbuf1[300];
    short NumChannels;
    short BitsPerSample;
    int   ByteRate;
    int   SamplingRate;
    short BlockAlign;
    int   DataSize;
    short audioFormat;

#if MT_PROFILE 
    if (argc != 6)
    {
        printf("Error - Invalid argument list <ipfile> <opfile> <op-sr> <fr-ms> <boardclock in Mhz>\n");
        return -1;
    }
#else
    if (argc != 3)
    {
        printf("Error - Invalid argument list <ipfile> <opfile> <op-sr> <fr-ms>\n");
        return -1;
    }
#endif

    /*check whether input is wav or not */
    ipfilename = argv[1];
    if (strcmp(&ipfilename[strlen(ipfilename) - 4], ".wav") != 0)
    {
        printf("Error - Invalid input file name... expected .wav file \n");
        return -1;
    }

    /* Input and Ouput file pointers*/
    ipfp = fopen(ipfilename, "rb");
    if (ipfp == NULL)
    {
        printf("unbale to open input file ....\n");
        return -1;
    }

    /* Input and Ouput file pointers*/
    opfp = fopen(argv[2], "wb");
    if (opfp == NULL)
    {
        printf("unable to open output file ....\n");
        return -1;
    }

    /*output samplingrate*/
    opSamplingRate = atoi(argv[3]);
    if ((opSamplingRate != 8000) && (opSamplingRate != 11025) &&
        (opSamplingRate != 16000) && (opSamplingRate != 22050) &&
        (opSamplingRate != 24000) && (opSamplingRate != 32000) &&
        (opSamplingRate != 44100) && (opSamplingRate != 48000) &&
        (opSamplingRate != -1))
    {
        printf("invalid output samplingrate....\n");
        return -1;
    }

    status_l = mt_cv_read_wave_hdr(ipfp, &NumChannels, &BitsPerSample,
        &ByteRate, &SamplingRate, &BlockAlign, &DataSize, &audioFormat);
    if (status_l != 0)
    {
        printf("invlaid file format....\n");
        return -1;
    }

    framesz_ms = atoi(argv[4]);
    if (framesz_ms > 500)
    {
        printf("invlaid file frame size....\n");
        return -1;
    }
    mseg_frcnts = 1000 / framesz_ms;
    //outputFile = fopen("output.txt", "w");
    /*estimate output sample size*/
    if (opSamplingRate == -1)
    {
        opSamplingRate = SamplingRate;
    }
    DataSize_sec = (float)DataSize / (float)ByteRate;
    opDataSize = (int)(DataSize_sec * (float)opSamplingRate * (float)(BitsPerSample / 8));

    /* Framesize*/
    framesz = (int)(((float)framesz_ms * (float)SamplingRate) / 1000.0);

    ipbuf = (short*)malloc(framesz * sizeof(short) * 4);
    opbuf = (short*)malloc(framesz * sizeof(short) * 4);
    fipbuf = (float*)malloc(framesz * sizeof(float) * 4);
    fopbuf = (float*)malloc(framesz * sizeof(float) * 4);

    /* output Wave header */
    mt_cv_write_wave_hdr(opfp, NumChannels, BitsPerSample,
        ByteRate, opSamplingRate, BlockAlign, opDataSize, audioFormat);

    /* Allocate State Memory */
    mvns_obj_p = (int*)malloc(MT_CV_SS_STATE_MEM_SIZE);
    if ((int)sizeof(mvns_obj_p) > (int)MT_CV_SS_STATE_MEM_SIZE)
    {
        printf("StateMemory Allocated:%d Required:%d \n", MT_CV_SS_STATE_MEM_SIZE, (int)sizeof(mvns_obj_p));
        return -1;
    }
    /* ss Intialization */
    status_l = mt_cv_ss_init(mvns_obj_p, SamplingRate, opSamplingRate);
    if (status_l != MT_CV_SS_SUCCESS)
    {
        printf("Unable to initialize ss \n");
        return -1;
    }

#if MT_PROFILE 
    boardclock = (float)atoi(argv[5]);
    printf("CPU speed:%f Mhz\n", boardclock);
#endif
    printf("SS Initialization Success\nProcessing....\n");

    /* ss Process */
    /* Feed T ms of audio data accumulated to get T ms of de-noisy data,
    The output pointer 'opbuf' provides the de-noisy data. */
    while (1)
    {
        /* Read frame data from wav file */
        if (audioFormat == 3)
        {
            readsamples = (int)fread(&fipbuf[0], 4, framesz, ipfp);
            readsamples = mt_cv_float_to_short(fipbuf, ipbuf, readsamples);

        }
        else
        {
            readsamples = (int)fread(&ipbuf[0], 2, framesz, ipfp);
        }

        if (readsamples != framesz)
        {
            if (totopcnt >= opDataSize)
            {
                break;
            }
            if (readsamples < 0)
                readsamples = 0;
            for (i_l = readsamples; i_l < framesz; i_l++)
            {
                ipbuf[i_l] = 0;
            }
        }
        totipcnt += (framesz * 2);

        frcnt++;

#if MT_PROFILE
        s_time = mt_cv_ss_get_time_usec();
#endif
        op_sampcnt = mt_cv_ss_process(mvns_obj_p, ipbuf, opbuf, framesz);

#if MT_PROFILE
        e_time = mt_cv_ss_get_time_usec();
        //printf("frm_cnt:%d op_sampcnt: %d \n", frcnt, op_sampcnt);

        time_elapsed_ms = ((float)e_time - (float)s_time) / (float)1000.0;
        // printf("t_time_elapsed_ms:%f \n", time_elapsed_ms);
        t_time_elapsed_ms += time_elapsed_ms;

        if (frcnt == mseg_frcnts)
        {
            mHz = (double)(t_time_elapsed_ms / 1000.0) * (double)boardclock;
            mHz_g = (float)mHz;
            mTime_g = t_time_elapsed_ms;
            printf("time_elapsed_ms:%f  mHz:%f \n", mTime_g, mHz_g);
            t_time_elapsed_ms = 0;
            frcnt = 0;
            sumMhz += mHz;
            sgmnts++;
        }
#endif

        if (op_sampcnt == MT_CV_SS_FAILURE)
        {
            printf("Data Unable to Process by ss\n");
            return -1;
        }
        else if (op_sampcnt == MT_CV_SS_LICENSE_EXPIRED)
        {
            printf("\n\n Product date has been expired !!!\n");
            return -2;
        }
        if (audioFormat == 3)
        {
            op_sampcnt = mt_cv_short_to_float(opbuf, fopbuf, op_sampcnt);
            totopcnt += (4 * op_sampcnt);
            if (totopcnt >= opDataSize)
            {
                op_sampcnt -= ((totopcnt - opDataSize) / 4);
                totopcnt -= (totopcnt - opDataSize);
            }
            fwrite(fopbuf, 4, op_sampcnt, opfp);
        }
        else
        {
            totopcnt += (2 * op_sampcnt);
            if (totopcnt >= opDataSize)
            {
                op_sampcnt -= ((totopcnt - opDataSize) / 2);
                totopcnt -= (totopcnt - opDataSize);
            }
            fwrite(opbuf, 2, op_sampcnt, opfp);
        }
    }

    /* ss De-Intialization */
   // status_l = mt_cv_ss_deinit(mvns_obj_p);
    if (status_l != MT_CV_SS_SUCCESS)
    {
        printf("Unable to de-initialize ss \n");
        return -1;
    }

    printf("SS Process Completed \n");
#if MT_PROFILE
    printf("AVG MHz:%f \n", (sumMhz / (float)sgmnts));
#endif
    free(mvns_obj_p);
    free(ipbuf);
    free(opbuf);
    free(fipbuf);
    free(fopbuf);
    fclose(ipfp);
    fclose(opfp);
    return 0;
}
/*------------------------------------------------------------------------
*
* Function Name     : mt_cv_read_wave_hdr()
*
* Description       :
*
* Returned Value    :
*
* Parameters        :
*
* Functions Used    : None
*
*   ---------------+-----+-----+-----+------------------------------------
*   Name           | I/P | O/P | I/O | Purpose
*   ---------------+-----+-----+-----+------------------------------------
*                  |     |     |     |
*------------------------------------------------------------------------*/
int
mt_cv_read_wave_hdr
(
    FILE* fp,
    short* NumChannels,
    short* BitsPerSample,
    int* ByteRate,
    int* SamplingRate,
    short* BlockAlign,
    int* DataSize,
    short* audioFormat
)
{
    unsigned char  audioData[4096 * 4];
    char ChunkID[5] = { 0 }, Format[5] = { 0 };
    int  ChunkSize;
    char Subchunk1ID[5] = { 0 };
    int Subchunk1Size;
    char Subchunk2ID[5] = { 0 }, listtype[5] = { 0 };
    char listitemid[5] = { 0 };
    int Subchunk2Size;
    int listOffset, listitemsize, error = 0;
    int read_l;

    read_l = (int)fread(ChunkID, 1, 4, fp);
    read_l = (int)fread(&ChunkSize, 1, 4, fp);
    read_l = (int)fread(&Format, 1, 4, fp);
    if (strcmp(ChunkID, "RIFF") == 0)
    {
        printf("detected little-endian WAVE file\n");
    }
    /*else if (strcmp(ChunkID, "RIFX") == 0)
    {
       printf("detected big-endian WAVE file\n");
    }*/
    else
    {
        printf("bad ChunkID expected RIFF or RIFX, got %s\n", ChunkID);
        return -20;
    }

    if (strcmp(Format, "WAVE") != 0)
    {
        printf("error bad format: expected WAVE, got %s\n", Format);
        return -21;
    }

    /* skip the unknown tags till "fmt " */
    error = 1;
    while (1)
    {
        if (fread(&Subchunk1ID, 1, 4, fp) != 4)
        {
            break;
        }
        read_l = (int)fread(&Subchunk1Size, 1, 4, fp);
        read_l = (int)fread(&audioData, 1, Subchunk1Size, fp);
        if (strcmp(Subchunk1ID, "fmt ") == 0)
        {
            error = 0;
            break;
        }
    }
    if (error == 1)
    {
        printf("Error: no 'fmt'....\n");
        return -22;
    }
    *audioFormat = ((int)audioData[0] | ((int)audioData[1] << 8));
    *NumChannels = ((int)audioData[2] | ((int)audioData[3] << 8));
    *SamplingRate = ((int)audioData[4] | ((int)audioData[5] << 8) |
        ((int)audioData[6] << 16) | ((int)audioData[7] << 24));
    *ByteRate = ((int)audioData[8] | ((int)audioData[9] << 8) |
        ((int)audioData[10] << 16) | ((int)audioData[11] << 24));
    *BlockAlign = ((int)audioData[12] | ((int)audioData[13] << 8));
    *BitsPerSample = ((int)audioData[14] | ((int)audioData[15] << 8));
    if (*NumChannels != 1)
    {
        printf("invlaid input channels....\n");
        return -23;
    }
    if ((*BitsPerSample != 16) && (*BitsPerSample != 32))
    {
        printf("invlaid input BytesPersample....\n");
        return -24;
    }
    if ((*audioFormat != 1) && (*audioFormat != 3))
    {
        printf("invlaid input Audio format....\n");
        return -25;
    }

    /* skip the unknown tags till "data" */
    while (1)
    {
        if (fread(&audioData[0], 1, 8, fp) != 8)
        {
            break;
        }
        memcpy(Subchunk2ID, &audioData[0], 4);
        Subchunk2Size = ((int)audioData[4] | ((int)audioData[5] << 8)
            | ((int)audioData[6] << 16) | ((int)audioData[7] << 24));
        //printf("Subchunk2ID:%s, Subchunk2Size:%d\n", Subchunk2ID, Subchunk2Size);
        if (strcmp(Subchunk2ID, "LIST") == 0)
        {
            read_l = (int)fread(&audioData[0], 1, 4, fp);
            memcpy(listtype, &audioData[0], 4);
            //printf("Subchunk2ID:%s, Subchunk2Size:%d ListType:%s \n", Subchunk2ID, Subchunk2Size, listtype);
            listOffset = 0;
            while ((Subchunk2Size - 8) >= listOffset)
            {
                read_l = (int)fread(&audioData[0], 1, 8, fp);
                memcpy(listitemid, &audioData[0], 4);
                listitemsize = ((int)audioData[4] | ((int)audioData[5] << 8)
                    | ((int)audioData[6] << 16) | ((int)audioData[7] << 24));
                listOffset = listOffset + listitemsize + 8;
                read_l = (int)fread(&audioData[0], 1, listitemsize, fp);
                //printf("listitemid:%s, listitemid:%d listOffset:%d\n", listitemid, listitemsize, listOffset);
            }
        }
        else if (strcmp(Subchunk2ID, "data") == 0)
        {
            //printf("Found data");
            *DataSize = Subchunk2Size;
            break;
        }
        else
        {
            read_l = (int)fread(&audioData[0], 1, Subchunk2Size, fp);
        }
    }
    return 0;
}
/*------------------------------------------------------------------------
*
* Function Name     : mt_cv_write_wave_hdr()
*
* Description       :
*
* Returned Value    :
*
* Parameters        :
*
* Functions Used    : None
*
*   ---------------+-----+-----+-----+------------------------------------
*   Name           | I/P | O/P | I/O | Purpose
*   ---------------+-----+-----+-----+------------------------------------
*                  |     |     |     |
*------------------------------------------------------------------------*/
int
mt_cv_write_wave_hdr
(
    FILE* fp,
    short NumChannels,
    short BitsPerSample,
    int   ByteRate,
    int   SamplingRate,
    short BlockAlign,
    int   DataSize,
    short audioFormat
)
{
    unsigned char  audioData[44];
    int totalSize, chunkSize = 16;

    totalSize = 36 + DataSize;
    audioData[0] = 'R'; audioData[1] = 'I'; audioData[2] = 'F'; audioData[3] = 'F';
    audioData[4] = (totalSize >> 0) & 0xFF;
    audioData[5] = (totalSize >> 8) & 0xFF;
    audioData[6] = (totalSize >> 16) & 0xFF;
    audioData[7] = (totalSize >> 24) & 0xFF;
    audioData[8] = 'W'; audioData[9] = 'A'; audioData[10] = 'V'; audioData[11] = 'E';
    audioData[12] = 'f'; audioData[13] = 'm'; audioData[14] = 't'; audioData[15] = ' ';
    audioData[16] = (chunkSize >> 0) & 0xFF;
    audioData[17] = (chunkSize >> 8) & 0xFF;
    audioData[18] = (chunkSize >> 16) & 0xFF;
    audioData[19] = (chunkSize >> 24) & 0xFF;
    audioData[20] = (audioFormat >> 0) & 0xFF;
    audioData[21] = (audioFormat >> 8) & 0xFF;
    audioData[22] = (NumChannels >> 0) & 0xFF;
    audioData[23] = (NumChannels >> 8) & 0xFF;
    audioData[24] = (SamplingRate >> 0) & 0xFF;
    audioData[25] = (SamplingRate >> 8) & 0xFF;
    audioData[26] = (SamplingRate >> 16) & 0xFF;
    audioData[27] = (SamplingRate >> 24) & 0xFF;
    audioData[28] = (ByteRate >> 0) & 0xFF;
    audioData[29] = (ByteRate >> 8) & 0xFF;
    audioData[30] = (ByteRate >> 16) & 0xFF;
    audioData[31] = (ByteRate >> 24) & 0xFF;
    audioData[32] = (BlockAlign >> 0) & 0xFF;
    audioData[33] = (BlockAlign >> 8) & 0xFF;
    audioData[34] = (BitsPerSample >> 0) & 0xFF;
    audioData[35] = (BitsPerSample >> 8) & 0xFF;
    audioData[36] = 'd'; audioData[37] = 'a'; audioData[38] = 't'; audioData[39] = 'a';
    audioData[40] = (DataSize >> 0) & 0xFF;
    audioData[41] = (DataSize >> 8) & 0xFF;
    audioData[42] = (DataSize >> 16) & 0xFF;
    audioData[43] = (DataSize >> 24) & 0xFF;
    fwrite(&audioData[0], 1, 44, fp);
    return 0;
}
/*------------------------------------------------------------------------
*
* Function Name     : mt_cv_float_to_short()
*
* Description       :
*
* Returned Value    :
*
* Parameters        :
*
* Functions Used    : None
*
*   ---------------+-----+-----+-----+------------------------------------
*   Name           | I/P | O/P | I/O | Purpose
*   ---------------+-----+-----+-----+------------------------------------
*                  |     |     |     |
*------------------------------------------------------------------------*/
int
mt_cv_float_to_short
(
    float* ip_p,
    short* op_p,
    int ipsmpcnt
)
{
    float val;
    for (int i_l = 0; i_l < ipsmpcnt; i_l++)
    {
        val = ip_p[i_l] * 32768;
        op_p[i_l] = (short)val;
    }
    return ipsmpcnt;
}

/*------------------------------------------------------------------------
*
* Function Name     : mt_cv_short_to_float()
*
* Description       :
*
* Returned Value    :
*
* Parameters        :
*
* Functions Used    : None
*
*   ---------------+-----+-----+-----+------------------------------------
*   Name           | I/P | O/P | I/O | Purpose
*   ---------------+-----+-----+-----+------------------------------------
*                  |     |     |     |
*------------------------------------------------------------------------*/
int
mt_cv_short_to_float
(
    short* ip_p,
    float* op_p,
    int ipsmpcnt
)
{
    float val;
    for (int i_l = 0; i_l < ipsmpcnt; i_l++)
    {
        val = (float)ip_p[i_l];
        op_p[i_l] = val / 32768;
    }
    return ipsmpcnt;
}

#if MT_PROFILE
/*------------------------------------------------------------------------
*
* Function Name     : mt_cv_ss_get_time_usec()
*
* Description       :
*
* Returned Value    :
*
* Parameters        :
*
* Functions Used    : None
*
*   ---------------+-----+-----+-----+------------------------------------
*   Name           | I/P | O/P | I/O | Purpose
*   ---------------+-----+-----+-----+------------------------------------
*                  |     |     |     |
*------------------------------------------------------------------------*/
unsigned int
mt_cv_ss_get_time_usec
(
)
{
    unsigned int  ctime_l;

#if (defined WIN32 || defined _WIN64 || defined _WIN32)
    double cntr_l;
    LARGE_INTEGER tmp_l;
    if (PCFreq == 0)
    {
        QueryPerformanceFrequency(&tmp_l);
        PCFreq = (double)((double)tmp_l.QuadPart) / 1000000.00;
    }
    QueryPerformanceCounter(&tmp_l);
    cntr_l = (double)((double)tmp_l.QuadPart) / PCFreq;
    ctime_l = (unsigned int)cntr_l;
#else
    struct timeval timeval_l;
    gettimeofday(&timeval_l, NULL);
    ctime_l = (timeval_l.tv_sec * 1000000) + timeval_l.tv_usec;
#endif
    return(ctime_l);
}
#endif
/* EOF */
/* $Log */
