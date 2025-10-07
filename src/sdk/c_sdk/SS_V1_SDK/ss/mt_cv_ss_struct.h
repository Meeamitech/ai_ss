/**************************************************************************
***
*** Copyright (c) Meeami Technologies
*** All rights reserved
***
*** This software embodies materials and concepts which are confidential
*** to Meeami Technologies and is made available solely pursuant to the terms of a
*** written license agreement with Meeami Technologies.
***
*** File Name  : mt_cv_ss_struct.h
***
*** Module Name: ss
***
*** Comments:
*** This file contains the defines of ss module
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
#ifndef __MT_CV_SS_STRUCT_H__
#define __MT_CV_SS_STRUCT_H__

typedef struct mt_cv_ss_data
{
	float temp;
	int cnt;
	float his_buf[32000 + 160];
	float y[128 * 4000];
	float x[100 * 40 * 128 + 128];
	float bias[400]; //need to remove
	float conv1d[128 * 4000];
	float l1[100 * 40 * 512];
	float l2[800 * 40 * 128];
	float l3[100 * 40 * 128 + 128];
	float l4[100 * 40 * 128 + 128];
	float l5[100 * 40 * 128 + 128];
	float l6[100 * 40 * 128 + 128];
	float prelu_param;


	void (*add)(float*, float*, int, int, int, float*);
	void (*add_r)(float*, float*, int, int, int, float*);
	int (*conv_1d)(float*, float*, float*, float*, int, int, int, int, int);
	int (*conv1d_transpose)(float*, float*, float*, int, int, int, int, int);
	void (*gnm)(float*, float*, float*, float*, int, int, int);
	int (*linear)(float*, float*, float*, int, int, int, float*);
	int (*linear_qkv)(float*, float*, float*, float*, float*, float*, float*, int, int, int, float*, float*, float*);
	void (*mask_multiply)(float*, float*, float*, int, int);
	void(*mean)(float*, float*, int, int, int);
	void(*mha)(float*, float*, float*, float*, float*, float*, int, int, int, int, int);
	void (*mha1)(float*, float*, float*, float*, float*, float*, int, int, int, int, int);
	void (*nm)(float*, float*, float*, float*, float*, int, int);
	void (*prelu)(float*, float*, float, int, int, int);
	void (*rearrange_f1)(float*, float*, int, int, int);
	void (*rearrange_f2)(float*, float*, int, int, int);
	void (*rearrange_f3)(float*, float*, int, int, int);
	void (*rearrange_f4)(float*, float*, int, int, int);
	void (*rearrange_f5)(float*, float*, int, int, int);
	void (*relu)(float*, float*, int, int, int);
	void(*view)(float*, float*, int, int, int, int);

}mt_cv_ss_data_t;

#endif /* __MT_CV_SS_STRUCT_H__ */
/* EOF */
/* $Log */
