/**************************************************************************
***
*** Copyright (c) Meeami Technologies
*** All rights reserved
***
*** This software embodies materials and concepts which are confidential
*** to Meeami Technologies and is made available solely pursuant to the terms of a
*** written license agreement with Meeami Technologies.
***
*** File Name  : mt_cv_ss_tbl.h
***
*** Module Name: SS
***
*** Comments:
*** This file contains the defines of MVNS module
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
#ifndef __MT_CV_SS_TBL_H__
#define __MT_CV_SS_TBL_H__

float conv1d_weights[128 * 1 * 16];
float pos_encoder[100][128];

float t1_norm1_gamma[4][128];
float t1_norm1_beta[4][128];
float t1_norm2_gamma[4][128];
float t1_norm2_beta[4][128];
float q_bias1[4][128];
float k_bias1[4][128];
float v_bias1[4][128];
float q_w1[4][128 * 128];
float k_w1[4][128 * 128];
float v_w1[4][128 * 128];
float t1_out_proj_bias[4][128];
float t1_out_proj_weight[4][128 * 128];
float t2_out_proj_weight[4][128 * 128];
float t1_ffn_linear_weight1[4][512 * 128];
float t1_ffn_linear_bias1[4][512];
float t1_ffn_linear_weight2[4][128 * 512];
float t1_ffn_linear_bias2[4][128];
float transformer1_norm_beta[128];
float transformer1_norm_gamma[128];
float SB1_norm_gamma[128];
float SB1_norm_beta[128];


float q_bias2[4][128];
float k_bias2[4][128];
float v_bias2[4][128];
float q_w2[4][128 * 128];
float k_w2[4][128 * 128];
float v_w2[4][128 * 128];
float t2_norm1_gamma[4][128];
float t2_norm1_beta[4][128];
float t2_norm2_gamma[4][128];
float t2_norm2_beta[4][128];
float t2_out_proj_bias[4][128];
float t2_ffn_linear_weight1[4][512 * 128];
float t2_ffn_linear_bias1[4][512];
float t2_ffn_linear_weight2[4][128 * 512];
float t2_ffn_linear_bias2[4][128];
float transformer2_norm_beta[128];
float transformer2_norm_gamma[128];
float SB2_norm_gamma[128];
float SB2_norm_beta[128];

float q_bias3[4][128];
float k_bias3[4][128];
float v_bias3[4][128];
float q_w3[4][128 * 128];
float k_w3[4][128 * 128];
float v_w3[4][128 * 128];
float t3_norm1_gamma[4][128];
float t3_norm1_beta[4][128];
float t3_norm2_gamma[4][128];
float t3_norm2_beta[4][128];
float t3_out_proj_bias[4][128];
float t3_out_proj_weight[4][128 * 128];
float t3_ffn_linear_weight1[4][512 * 128];
float t3_ffn_linear_bias1[4][512];
float t3_ffn_linear_weight2[4][128 * 512];
float t3_ffn_linear_bias2[4][128];
float transformer3_norm_beta[128];
float transformer3_norm_gamma[128];
float SB3_norm_gamma[128];
float SB3_norm_beta[128];

float output_conv1d_w[256 * 128];
float output_conv1d_b[256];

float conv1d_transpose_w1[128 * 16];
float conv1d_transpose_w2[128 * 16];
#endif /* __MT_CV_SS_TBL_H__ */

/* EOF */
/* $Log */
