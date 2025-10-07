/**************************************************************************
***
*** Copyright (c) Meeami Technologies
*** All rights reserved
***
*** This software embodies materials and concepts which are confidential
*** to Meeami Technologies and is made available solely pursuant to the terms of a
*** written license agreement with Meeami Technologies.
***
*** File Name  : mt_cv_ss_defines.h
***
*** Module Name: SS
***
*** Comments:
*** This file contains the defines of SS module
***
*** List of functions:
***
*** Author(s):
***
***
*** Rev History:
*** Sl   By         date           change details
*** ----------------------------------------------------------------------
*** 01.  <name>       <date>           <details>
*** ----------------------------------------------------------------------
***
**************************************************************************
*END*********************************************************************/

#ifndef __MT_CV_SS_DEFINES_H__
#define __MT_CV_SS_DEFINES_H__

/* Framesize configuration */
#define MT_CV_SS_OP_BUFFERING       1
#define MT_CV_SS_SAMP_RATE          16000
#define MT_CV_SS_MAX_SAMP_RATE      48000
#define MT_CV_SS_FRSZ_MS            10
#define MT_CV_SS_FRSZ               (MT_CV_SS_FRSZ_MS * (MT_CV_SS_SAMP_RATE / 1000))
#define MT_CV_SS_SRC_FRSZ           (MT_CV_SS_FRSZ_MS * (MT_CV_SS_MAX_SAMP_RATE / 1000))
#define MT_CV_SS_USE_AVX_INTR_MATH  1
#define MT_CV_SS_MAX_FRSZ           100

#define MT_CV_SS_DFN_WINDSZ_MS     20
#define MT_CV_SS_DFN_WINDSZ        (MT_CV_SS_DFN_WINDSZ_MS * (MT_CV_SS_SAMP_RATE / 1000))
#define MT_CV_SS_DFN_SHFTSZ_MS     10
#define MT_CV_SS_DFN_SHFTSZ        (MT_CV_SS_DFN_SHFTSZ_MS * (MT_CV_SS_SAMP_RATE / 1000))
#define MT_CV_SS_DFN_L0OPSZ        (MT_CV_SS_FRSZ / MT_CV_SS_DFN_SHFTSZ)
#define MT_CV_SS_DFN_FEATS_SZ      257
#define MT_CV_SS_DFN_FEATS_BUFSZ   256
#define MT_CV_SS_MAX_DFN_FEATS_BUFSZ 258

#define MT_CV_SS_TCN_WINDSZ_MS     15
#define MT_CV_SS_TCN_WINDSZ        (MT_CV_SS_TCN_WINDSZ_MS * (MT_CV_SS_SAMP_RATE / 1000))
#define MT_CV_SS_TCN_SHFTSZ_MS     5
#define MT_CV_SS_TCN_SHFTSZ        (MT_CV_SS_TCN_SHFTSZ_MS * (MT_CV_SS_SAMP_RATE / 1000))
#define MT_CV_SS_TCN_L0OPSZ        (MT_CV_SS_FRSZ / MT_CV_SS_TCN_SHFTSZ)

#define MT_CV_SS_DFN_KRN_SZ          3
#define MT_CV_SS_DFN_PAST_FRSZ       2/*MT_CV_SS_DFN_KRN_SZ-1*/
#define MT_CV_SS_DFN_IP_CH           2
#define MT_CV_SS_DFN_OP_CH           32
#define MT_CV_SS_DFN_OP_CH1          10
#define MT_CV_SS_DFN_OP_CH2          MT_CV_SS_DFN_OP_CH1/2

#define MT_CV_SS_DFN_R               5
#define MT_CV_SS_GRU_R              3
#define MT_CV_SS_GRU_X              4
#define MT_CV_SS_GRU_IPSZ           256
#define MT_CV_SS_GRU_HDSZ           128
#define MT_CV_SS_GRU_512           512
#define MT_CV_SS_GRU_1024          1024
#define MT_CV_SS_R                  2
#define MT_CV_SS_X                  7
#define MT_CV_SS_GRPS_4             4
#define MT_CV_SS_CHS_64             64
#define MT_CV_SS_CHS_128            128
#define MT_CV_SS_CHS_16             16
#define MT_CV_SS_CHS_32             32
#define MT_CV_SS_CHS_64             64
#define MT_CV_SS_CHS_256            256
#define MT_CV_SS_CHS_240            240
#define MT_CV_SS_CHS_512            512
#define MT_CV_SS_TCN_DKSZ            3
#define MT_CV_SS_TCN_ENCKSZ         256
#define MT_CV_SS_TCN_ENCKSHFT      MT_CV_SS_TCN_SHFTSZ
#if (MT_CV_SS_FRSZ_MS == 40)
#define MT_CV_SS_TCN_TBUFSZ        369000
#elif(MT_CV_SS_FRSZ_MS == 16)
#define MT_CV_SS_TCN_TBUFSZ        315400
#else
#define MT_CV_SS_TCN_TBUFSZ        289000*2
#endif

#define MT_CV_SS_MAX_L0OPSZ         ((MT_CV_SS_TCN_L0OPSZ > MT_CV_SS_DFN_L0OPSZ) ? MT_CV_SS_TCN_L0OPSZ : MT_CV_SS_DFN_L0OPSZ)
#define MT_CV_SS_MAX_CHS            512
#define MT_CV_SCRATCH_BUFSZ           (MT_CV_SS_MAX_CHS * MT_CV_SS_MAX_L0OPSZ)
#define MT_CV_SCRATCH_BUFSZ1          MT_CV_SS_DFN_OP_CH2*MT_CV_SS_MAX_DFN_FEATS_BUFSZ

#define MT_CV_SS_NOP                0
#define MT_CV_SS_ENABLE_VALIDATION  0
#define MT_CV_SS_TCN_SUPPORT       1
#define MT_CV_SS_INTRINSIC  1

#endif /* __MT_CV_SS_DEFINES_H__ */

/* EOF */
/* $Log */
