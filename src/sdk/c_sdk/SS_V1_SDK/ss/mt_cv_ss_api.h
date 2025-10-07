/**************************************************************************
***
*** Copyright (c) Meeami Technologies
*** All rights reserved
***
*** This software embodies materials and concepts which are confidential
*** to Meeami Technologies and is made available solely pursuant to the terms of a
*** written license agreement with Meeami Technologies.
***
*** File Name  : mt_cv_ss_api.h
***
*** Module Name: ss
***
*** Comment: Contains the prototypes of ss module functions.
***
*** List of functions:
***
*** Author(s): Yugesh AV
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

#ifndef __MT_CV_SS_API_H__
#define __MT_CV_SS_API_H__

/* state/persistent memory size */
#define MT_CV_SS_STATE_MEM_SIZE          40000000

/* status */
#define MT_CV_SS_SUCCESS                     0
#define MT_CV_SS_FAILURE                    -1
#define MT_CV_SS_LICENSE_EXPIRED            -2

#ifdef __cplusplus
extern "C" {
#else
extern
#endif

#ifdef BUILD_DLL
#define MEEAMI_EXPORT __declspec(dllexport)
#else
#define MEEAMI_EXPORT 
#endif
MEEAMI_EXPORT int mt_cv_ss_init(int* mvns_obj_p, int  ipsr, int opsr);
MEEAMI_EXPORT int mt_cv_ss_process(int* mvns_obj_p, short* ip_p, short* op_p, int sampcnt);
MEEAMI_EXPORT int mt_cv_ss_deinit(int* mvns_obj_p);

/*SUB BAND APIs*/
MEEAMI_EXPORT int mt_cv_ss_sprocess(int* mvns_obj_p, short* lip_p, short* hip_p, short* lop_p, short* hop_p, int sampcnt);
#ifdef __cplusplus
}
#endif

#endif /* __MT_CV_SS_API_H__ */
/* EOF */
/* $Log */
