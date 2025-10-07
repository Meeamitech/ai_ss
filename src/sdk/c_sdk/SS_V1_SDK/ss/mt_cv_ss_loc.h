/**************************************************************************
***
*** Copyright (c) Meeami Technologies
*** All rights reserved
***
*** This software embodies materials and concepts which are confidential
*** to Meeami Technologies and is made available solely pursuant to the terms of a
*** written license agreement with Meeami Technologies.
***
*** File Name  : mt_cv_ss_loc.h
***
*** Module Name: ss
***
*** Comment: Contains the prototypes of ss module functions.
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

#ifndef __MT_CV_SS_LOC_H__
#define __MT_CV_SS_LOC_H__


void
mt_cv_ss_prelu
(
    float* ip,
    float* op,
    float alpha,
    int channel,
    int column,
    int row

);

void
mt_cv_ss_rearrange_f1
(
    float* ip,
    float* op,
    int channel,
    int column,
    int row
);

void
mt_cv_ss_rearrange_f2
(
    float* ip,
    float* op,
    int channel,
    int column,
    int row
);

void
mt_cv_ss_rearrange_f3
(
    float* ip,
    float* op,
    int channel,
    int column,
    int row
);

void
mt_cv_ss_rearrange_f4
(
    float* ip,
    float* op,
    int channel,
    int column,
    int row
);

void
mt_cv_ss_rearrange_f5
(
    float* ip,
    float* op,
    int channel,
    int column,
    int row
);

void
mt_cv_ss_mha
(
    float* q,
    float* k,
    float* v,
    float* op,
    float* out_proj_w,
    float* out_proj_b,
    int channel,
    int seg_size,
    int row,
    int d,
    int head_dim
);

int mt_cv_ss_linear
(
    float* ip,
    float* w,
    float* bias,
    int iclm,
    int ics,
    int row,
    float* op
);

void
mt_cv_ss_mask_multiply
(
    float* ip,
    float* mask,
    float* op,
    int row,
    int column
);

void
mt_cv_ss_mean
(
    float* ip,
    float* op,
    int ch,
    int column,
    int row
);

void
mt_cv_ss_nm
(
    float* A,
    float* B,
    float* C,
    float* D,
    float* E,
    int ics,
    int iclm
);


int mt_cv_ss_conv1d(
    float* input,
    float* weights,
    float* bias,
    float* output,
    int in_channels,
    int out_channels,
    int input_size,
    int kernel_size,
    int stride
);

int mt_cv_ss_conv1d_transpose(
    float* input,
    float* weights,
    float* output,
    int in_channels,
    int out_channels,
    int input_size,
    int kernel_size,
    int stride
);

void mt_cv_ss_gnm(
    float* A,
    float* B,
    float* C,
    float* D,
    int channel,
    int ics,
    int iclm
);

void
mt_cv_ss_add
(
    float* ip1,
    float* ip2,
    int channel,
    int column,
    int row,
    float* op
);

void
mt_cv_ss_add_r
(
    float* ip1,
    float* ip2,
    int channel,
    int column,
    int row,
    float* op
);

void mt_cv_ss_rearrange(
    float* ip,
    float* op,
    int channel,
    int column,
    int row,
    int mode
);

void
mt_cv_ss_relu
(
    float* ip,
    float* op,
    int channel,
    int column,
    int row
);

void
mt_cv_ss_view
(
    float* ip,
    float* op,
    int channel,
    int iclm,
    int ics,
    int head_dim
);

void
mt_cv_ss_mha1
(
    float* q,
    float* k,
    float* v,
    float* op,
    float* out_proj_w,
    float* out_proj_b,
    int channel,
    int seg_size,
    int row,
    int d,
    int head_dim
);

int mt_cv_ss_linear_qkv(
    float* ip,
    float* qw,
    float* kw,
    float* vw,
    float* qb,
    float* kb,
    float* vb,
    int iclm,
    int ics,
    int row,
    float* q_op,
    float* k_op,
    float* v_op
);

/* AVX */

void
mt_cv_ss_rearrange_f1_avx
(
    float* ip,
    float* op,
    int channel,
    int column,
    int row
);

void
mt_cv_ss_rearrange_f2_avx
(
    float* ip,
    float* op,
    int channel,
    int column,
    int row
);

void
mt_cv_ss_mha_avx
(
    float* q,
    float* k,
    float* v,
    float* op,
    float* out_proj_w,
    float* out_proj_b,
    int channel,
    int seg_size,
    int row,
    int d,
    int head_dim
);

int mt_cv_ss_linear_avx
(
    float* ip,
    float* w,
    float* bias,
    int iclm,
    int ics,
    int row,
    float* op
);

void
mt_cv_ss_mask_multiply_avx
(
    float* ip,
    float* mask,
    float* op,
    int row,
    int column
);

void
mt_cv_ss_nm_avx
(
    float* A,
    float* B,
    float* C,
    float* D,
    float* E,
    int ics,
    int iclm
);


int mt_cv_ss_conv1d_avx(
    float* input,
    float* weights,
    float* bias,
    float* output,
    int in_channels,
    int out_channels,
    int input_size,
    int kernel_size,
    int stride
);

void
mt_cv_ss_add_avx
(
    float* ip1,
    float* ip2,
    int channel,
    int column,
    int row,
    float* op
);

void
mt_cv_ss_add_r_avx
(
    float* ip1,
    float* ip2,
    int channel,
    int column,
    int row,
    float* op
);

void
mt_cv_ss_mha1_avx
(
    float* q,
    float* k,
    float* v,
    float* op,
    float* out_proj_w,
    float* out_proj_b,
    int channel,
    int seg_size,
    int row,
    int d,
    int head_dim
);

int mt_cv_ss_linear_qkv_avx(
    float* ip,
    float* qw,
    float* kw,
    float* vw,
    float* qb,
    float* kb,
    float* vb,
    int iclm,
    int ics,
    int row,
    float* q_op,
    float* k_op,
    float* v_op
);

#endif /* __MT_CV_SS_API_H__ */
/* EOF */
/* $Log */
