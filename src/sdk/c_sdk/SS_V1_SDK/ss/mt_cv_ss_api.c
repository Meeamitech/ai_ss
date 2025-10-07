/***************************************************************************
*** Copyright (c) Meeami Technologies
*** All rights reserved
***
*** This software embodies materials and concepts which are confidential
*** to Meeami Technologies and is made available solely pursuant to the terms of a
*** written license agreement with Meeami Technologies.
***
*** File Name: ms_cv_ss_api.c
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
*** Author   :
***
*** Rev History:
*** Sl    By               date           change details
*** ----------------------------------------------------------------------
***
*** ----------------------------------------------------------------------
***
**************************************************************************
*END*********************************************************************/

/* Include system level header files */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/* High level header files */
//#include "mt_cv_ss_fft.h"
#include "mt_cv_ss_api.h"
#include "mt_cv_ss_defines.h"
#include "mt_cv_ss_struct.h"
//#include "mt_cv_ss_loc.h"
#include "mt_cv_ss_tbl.h"

/*------------------------------------------------------------------------
*
* Function Name     : mt_cv_ss_init()
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
#ifdef BUILD_DLL
__declspec(dllexport)
#endif
int
mt_cv_ss_init
(
    int* mvns_obj_p,
    int  ipsr,
    int  opsr
)
{
    int status_l = 0;

    status_l = mt_cv_ss_reset(mvns_obj_p, ipsr, opsr);

    return status_l;
}

/*------------------------------------------------------------------------
*
* Function Name     : mt_cv_ss_set_dsp_opt()
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
#ifdef BUILD_DLL
__declspec(dllexport)
#endif
/*------------------------------------------------------------------------
*
* Function Name     : mt_cv_ss_process()
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
#ifdef BUILD_DLL
__declspec(dllexport)
#endif
int
mt_cv_ss_process
(
    int* mvns_obj_p,
    short* ip_p,
    short* op_p,
    int sampcnt
)
{
    int opcnt = 0;

#if MT_CV_SS_ENABLE_VALIDATION
    int status_l;
    status_l = mt_cv_ss_checkExpiryDate();
    if (status_l == -1)
    {
        return MT_CV_SS_LICENSE_EXPIRED;
    }
#endif
    opcnt = mt_cv_ss_proc(mvns_obj_p, ip_p, op_p, sampcnt);

    return opcnt;
}

/*------------------------------------------------------------------------
*
* Function Name     : mt_cv_ss_sprocess()
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
#ifdef BUILD_DLL
__declspec(dllexport)
#endif
int
mt_cv_ss_sprocess
(
    int* mvns_obj_p,
    short* lip_p,
    short* hip_p,
    short* lop_p,
    short* hop_p,
    int sampcnt
)
{
    int opcnt = 0;
    short ipbuf[MT_CV_SS_SRC_FRSZ * 2];
    short opbuf[MT_CV_SS_SRC_FRSZ * 2];
    mt_cv_ss_data_t* state_tp;
    state_tp = (mt_cv_ss_data_t*)mvns_obj_p;

#if MT_CV_SS_ENABLE_VALIDATION
    int status_l;
    status_l = mt_cv_ss_checkExpiryDate();
    if (status_l == -1)
    {
        return MT_CV_ss_LICENSE_EXPIRED;
    }
#endif

    // mt_cv_ss_qmix((short*)&state_tp->qstatemem[0], lip_p, hip_p, ipbuf, sampcnt);
    opcnt = mt_cv_ss_proc(mvns_obj_p, ipbuf, opbuf, sampcnt << 1);

    return opcnt;
}

/*------------------------------------------------------------------------
*
* Function Name     : mt_cv_ss_deinit()
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
#ifdef BUILD_DLL
__declspec(dllexport)
#endif
int
mt_cv_ss_deinit
(
    int* mvns_obj_p
)
{
    int opcnt = 0;

    //opcnt = mt_cv_ss_destry(mvns_obj_p);

    return opcnt;
}
/* EOF */
/* $Log */
