/***************************************************************************
*** Copyright (c) Meeami Technologies
*** All rights reserved
***
*** This software embodies materials and concepts which are confidential
*** to Meeami Technologies and is made available solely pursuant to the terms of a
*** written license agreement with Meeami Technologies.
***
*** File Name: ms_cv_ss_proc.c
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


#if (defined WIN32 || defined _WIN64 || defined _WIN32 || defined ANDROID_CHANGES)
#include<math.h>
#else
#define _USE_MATH_DEFINES
#include <cmath>
#endif
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

/* High level header files */

#include "mt_cv_ss_api.h"
#include "mt_cv_ss_defines.h"
#include "mt_cv_ss_struct.h"
#include "mt_cv_ss_loc.h"
#include "mt_cv_ss_tbl.h"

extern FILE* outputFile;
/*------------------------------------------------------------------------
*
* Function Name     : mt_cv_ss_proc()
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
int frm_cnt;
unsigned int s_time, e_time;

int
mt_cv_ss_proc
(
    int* mvns_obj_p,
    short* ip_p,
    short* op_p,
    int sampcnt
)
{
    int i = 0, j = 0, k = 0, p = 0, l = 0, m = 0, i_l = 0, j_l = 0;
    int buffer_lenth = 32000;
    int op_cnt;
    float tmp;
    int channel;
    int layers = 4;
    int tmp32_l;
    int start_i, end_i, ind;



    mt_cv_ss_data_t* state_tp;
    state_tp = (mt_cv_ss_data_t*)mvns_obj_p;


    if (frm_cnt <= 200)
    {
        for (i = 0; i < sampcnt; i++)
        {
            state_tp->his_buf[state_tp->cnt] = ((float)ip_p[i]) / 32768;
            state_tp->cnt++;
        }
    }
    else
    {
        for (i = 0; i < (buffer_lenth - sampcnt); i++)
        {
            state_tp->his_buf[i] = state_tp->his_buf[i + sampcnt];
        }
        j = buffer_lenth - sampcnt;
        for (i = 0; i < sampcnt; i++)
        {
            state_tp->his_buf[j++] = ((float)ip_p[i]) / 32768;
        }

    }


    if (frm_cnt >= 3)
    {

        j = 0;
        // Perform convolution
        if (frm_cnt < 200)
        {
            op_cnt = state_tp->conv_1d(state_tp->his_buf, conv1d_weights, state_tp->bias, state_tp->conv1d, 1, 128, frm_cnt * sampcnt, 16, 8);
            ind = frm_cnt - 3;
            start_i = ind * sampcnt;
            end_i = (ind + 1) * sampcnt;
        }
        else
        {
            op_cnt = state_tp->conv_1d(state_tp->his_buf, conv1d_weights, state_tp->bias, state_tp->conv1d, 1, 128, 200 * sampcnt, 16, 8);
            start_i = 31520;
            end_i = 31680;
        }


        int itr = op_cnt / 128;
        k = 0, l = 0;
        m = 0;

        if (itr % 40 == 0)
        {
            channel = round(itr / 40);
        }
        else
        {
            channel = round(itr / 40) + 1;
        }

        for (j = 0; j < itr; j = j + 40)
        {
            for (i = 0; i < 128; i++)
            {
                l = i * itr + j;
                m = i + (40 * 128 * (j / 40));
                if (itr - j >= 40)
                {
                    for (p = 0; p < 40; p++)
                    {

                        state_tp->x[m] = state_tp->conv1d[l++];
                        state_tp->l1[m] = state_tp->x[m] + pos_encoder[p][i];
                        m = m + 128;
                    }
                }
                else
                {
                    for (p = 0; p < itr - j; p++)
                    {
                        state_tp->x[m] = state_tp->conv1d[l++];
                        state_tp->l1[m] = state_tp->x[m] + pos_encoder[p][i];
                        m = m + 128;
                    }
                    for (p = itr - j; p < 40; p++)
                    {
                        state_tp->x[m] = 0;
                        state_tp->l1[m] = pos_encoder[p][i];
                        m = m + 128;
                    }

                }
            }
            k++;
        }

        channel = k;
        for (i = 0; i < layers; i++)
        {


            state_tp->nm(state_tp->l1, t1_norm1_gamma[i], t1_norm1_beta[i], state_tp->l2, state_tp->l3, 128, channel * 40);
            state_tp->linear_qkv(state_tp->l2, q_w1[i], k_w1[i], v_w1[i], q_bias1[i], k_bias1[i], v_bias1[i], channel * 40, 128, 128, state_tp->l3, state_tp->l4, state_tp->l5);

            state_tp->view(state_tp->l3, state_tp->l6, channel, 40, 128, 16);
            state_tp->view(state_tp->l4, state_tp->l3, channel, 40, 128, 16);
            state_tp->view(state_tp->l5, state_tp->l4, channel, 40, 128, 16);

            state_tp->mha1(state_tp->l6, state_tp->l3, state_tp->l4, state_tp->l2, t1_out_proj_weight[i], t1_out_proj_bias[i], channel, 40, 128, 16, 8);

            state_tp->rearrange_f1(state_tp->l2, state_tp->l5, channel, 40, 128);
            state_tp->add(state_tp->l5, state_tp->l1, channel, 40, 128, state_tp->l6);
            state_tp->nm(state_tp->l6, t1_norm2_gamma[i], t1_norm2_beta[i], state_tp->l2, state_tp->l3, 128, channel * 40);
            state_tp->rearrange_f2(state_tp->l2, state_tp->l1, channel, 40, 128);
            //ffn

            state_tp->linear(state_tp->l1, t1_ffn_linear_weight1[i], t1_ffn_linear_bias1[i], channel * 40, 512, 128, state_tp->l2);

            state_tp->relu(state_tp->l2, state_tp->l1, channel, 40, 512);
            state_tp->linear(state_tp->l1, t1_ffn_linear_weight2[i], t1_ffn_linear_bias2[i], channel * 40, 128, 512, state_tp->l5);
            state_tp->rearrange_f1(state_tp->l5, state_tp->l4, channel, 40, 128);
            state_tp->add(state_tp->l4, state_tp->l6, channel, 40, 128, state_tp->l1);

        }
        state_tp->nm(state_tp->l1, transformer1_norm_gamma, transformer1_norm_beta, state_tp->l2, state_tp->l3, 128, channel * 40);
        state_tp->rearrange_f3(state_tp->l2, state_tp->l1, channel, 40, 128);
        state_tp->gnm(state_tp->l1, SB1_norm_gamma, SB1_norm_beta, state_tp->l2, channel, 40, 128);
        state_tp->rearrange_f3(state_tp->l2, state_tp->l1, channel, 128, 40);
        state_tp->add(state_tp->l1, state_tp->x, channel, 40, 128, state_tp->y);

        //2nd tranformer
        state_tp->mean(state_tp->y, state_tp->x, channel, 40, 128);

        for (j = 0; j < channel; j++)
        {
            m = j * 128;
            for (i = 0; i < 128; i++)
            {
                state_tp->l1[m] = state_tp->x[m] + pos_encoder[j][i];
                m++;
            }
        }

        for (i = 0; i < layers; i++)
        {
            state_tp->nm(state_tp->l1, t2_norm1_gamma[i], t2_norm1_beta[i], state_tp->l2, state_tp->l3, 128, channel);

            state_tp->linear_qkv(state_tp->l2, q_w2[i], k_w2[i], v_w2[i], q_bias2[i], k_bias2[i], v_bias2[i], channel, 128, 128, state_tp->l3, state_tp->l4, state_tp->l5);

            state_tp->view(state_tp->l3, state_tp->l6, 1, channel, 128, 16);
            state_tp->view(state_tp->l4, state_tp->l3, 1, channel, 128, 16);
            state_tp->view(state_tp->l5, state_tp->l4, 1, channel, 128, 16);

            state_tp->mha(state_tp->l6, state_tp->l3, state_tp->l4, state_tp->l2, t2_out_proj_weight[i], t2_out_proj_bias[i], 1, channel, 128, 16, 8);
            state_tp->rearrange_f1(state_tp->l2, state_tp->l5, channel, 1, 128);
            state_tp->add(state_tp->l5, state_tp->l1, channel, 1, 128, state_tp->l6);
            state_tp->nm(state_tp->l6, t2_norm2_gamma[i], t2_norm2_beta[i], state_tp->l2, state_tp->l3, 128, channel);
            state_tp->rearrange_f2(state_tp->l2, state_tp->l1, channel, 1, 128);

            //ffn
            state_tp->linear(state_tp->l1, t2_ffn_linear_weight1[i], t2_ffn_linear_bias1[i], channel, 512, 128, state_tp->l3);
            state_tp->relu(state_tp->l3, state_tp->l4, 1, channel, 512);
            state_tp->linear(state_tp->l4, t2_ffn_linear_weight2[i], t2_ffn_linear_bias2[i], channel, 128, 512, state_tp->l5);
            state_tp->rearrange_f1(state_tp->l5, state_tp->l4, channel, 1, 128);
            state_tp->add(state_tp->l4, state_tp->l6, 1, channel, 128, state_tp->l1);
        }
        state_tp->nm(state_tp->l1, transformer2_norm_gamma, transformer2_norm_beta, state_tp->l2, state_tp->l3, 128, channel);
        state_tp->rearrange_f3(state_tp->l2, state_tp->l1, 1, channel, 128);
        state_tp->gnm(state_tp->l1, SB2_norm_gamma, SB2_norm_beta, state_tp->l2, 1, channel, 128);
        state_tp->rearrange_f3(state_tp->l2, state_tp->l1, 1, 128, channel);
        state_tp->add(state_tp->l1, state_tp->x, 1, channel, 128, state_tp->l2);

        state_tp->add_r(state_tp->y, state_tp->l2, channel, 40, 128, state_tp->x);

        //3rd Transformer
        m = 0;
        for (k = 0; k < channel; k++)
        {
            for (j = 0; j < 40; j++)
            {
                for (i = 0; i < 128; i++)
                {
                    state_tp->l1[m] = state_tp->x[m] + pos_encoder[j][i];
                    m++;
                }
            }
        }

        for (i = 0; i < layers; i++)
        {
            state_tp->nm(state_tp->l1, t3_norm1_gamma[i], t3_norm1_beta[i], state_tp->l2, state_tp->l3, 128, channel * 40);

            state_tp->linear_qkv(state_tp->l2, q_w3[i], k_w3[i], v_w3[i], q_bias3[i], k_bias3[i], v_bias3[i], channel * 40, 128, 128, state_tp->l3, state_tp->l4, state_tp->l5);

            state_tp->view(state_tp->l3, state_tp->l6, channel, 40, 128, 16);
            state_tp->view(state_tp->l4, state_tp->l3, channel, 40, 128, 16);
            state_tp->view(state_tp->l5, state_tp->l4, channel, 40, 128, 16);

            state_tp->mha1(state_tp->l6, state_tp->l3, state_tp->l4, state_tp->l2, t3_out_proj_weight[i], t3_out_proj_bias[i], channel, 40, 128, 16, 8);
            state_tp->rearrange_f1(state_tp->l2, state_tp->l5, channel, 40, 128);
            state_tp->add(state_tp->l5, state_tp->l1, channel, 40, 128, state_tp->l6);
            state_tp->nm(state_tp->l6, t3_norm2_gamma[i], t3_norm2_beta[i], state_tp->l2, state_tp->l3, 128, channel * 40);
            state_tp->rearrange_f2(state_tp->l2, state_tp->l1, channel, 40, 128);

            //ffn
            state_tp->linear(state_tp->l1, t3_ffn_linear_weight1[i], t3_ffn_linear_bias1[i], channel * 40, 512, 128, state_tp->l2);
            state_tp->relu(state_tp->l2, state_tp->l1, channel, 40, 512);
            state_tp->linear(state_tp->l1, t3_ffn_linear_weight2[i], t3_ffn_linear_bias2[i], channel * 40, 128, 512, state_tp->l5);
            state_tp->rearrange_f1(state_tp->l5, state_tp->l4, channel, 40, 128);
            state_tp->add(state_tp->l4, state_tp->l6, channel, 40, 128, state_tp->l1);
        }
        state_tp->nm(state_tp->l1, transformer3_norm_gamma, transformer3_norm_beta, state_tp->l2, state_tp->l3, 128, channel * 40);
        state_tp->rearrange_f3(state_tp->l2, state_tp->l1, channel, 40, 128);
        state_tp->gnm(state_tp->l1, SB3_norm_gamma, SB3_norm_beta, state_tp->l2, channel, 40, 128);
        state_tp->rearrange_f3(state_tp->l2, state_tp->l1, channel, 128, 40);
        state_tp->add(state_tp->l1, state_tp->x, channel, 40, 128, state_tp->l2);
        state_tp->rearrange_f4(state_tp->l2, state_tp->l1, 1, itr, 128);
        state_tp->prelu(state_tp->l1, state_tp->l2, state_tp->prelu_param, 1, itr, 128);
        //state_tp->conv_1d(state_tp->l2, output_conv1d_w, output_conv1d_b, state_tp->l1, 128, 256, itr, 1, 1);
        mt_cv_ss_conv1d(state_tp->l2, output_conv1d_w, output_conv1d_b, state_tp->l1, 128, 256, itr, 1, 1);
        state_tp->rearrange_f5(state_tp->l1, state_tp->l2, 1, 256, itr);
        //mask multiplication

        state_tp->mask_multiply(state_tp->conv1d, &state_tp->l2[0], state_tp->y, 128, itr);
        //    mt_cv_ss_mask_multiply(state_tp->conv1d, & state_tp->l2[itr*128], state_tp->x, 128, itr);
        state_tp->conv1d_transpose(state_tp->y, conv1d_transpose_w1, state_tp->l1, 128, 1, itr, 16, 8);
        //   mt_cv_ss_conv1d_transpose(state_tp->x, conv1d_transpose_w2, state_tp->l2, 128, 1, itr, 16, 8);

        j_l = 0;
        for (i_l = start_i; i_l < end_i; i_l++)
        {
            op_p[j_l] = (short)(state_tp->l1[i_l] * 32768);
            j_l++;
        }
        op_cnt = sampcnt;
    }

    else
        op_cnt = 0;

    frm_cnt++;
    return op_cnt;
}

int
mt_cv_ss_reset
(
    int* mvns_obj_p,
    int ip_sr,
    int op_sr
)
{
    mt_cv_ss_data_t* state_tp;

    state_tp = (mt_cv_ss_data_t*)mvns_obj_p;
    memset(mvns_obj_p, 0, sizeof(mt_cv_ss_data_t));
    state_tp->prelu_param = 0.2449;

    state_tp->conv1d_transpose = mt_cv_ss_conv1d_transpose;
    state_tp->gnm = mt_cv_ss_gnm;
    state_tp->mean = mt_cv_ss_mean;
    state_tp->prelu = mt_cv_ss_prelu;
    state_tp->rearrange_f3 = mt_cv_ss_rearrange_f3;
    state_tp->rearrange_f4 = mt_cv_ss_rearrange_f4;
    state_tp->rearrange_f5 = mt_cv_ss_rearrange_f5;
    state_tp->relu = mt_cv_ss_relu;
    state_tp->view = mt_cv_ss_view;

#if MT_CV_SS_INTRINSIC
    state_tp->add = mt_cv_ss_add_avx;
    state_tp->add_r = mt_cv_ss_add_r_avx;
    state_tp->conv_1d = mt_cv_ss_conv1d_avx;

    state_tp->linear = mt_cv_ss_linear_avx;
    state_tp->linear_qkv = mt_cv_ss_linear_qkv_avx;
    state_tp->mask_multiply = mt_cv_ss_mask_multiply_avx;

    state_tp->mha = mt_cv_ss_mha_avx;
    state_tp->mha1 = mt_cv_ss_mha1_avx;
    state_tp->nm = mt_cv_ss_nm_avx;

    state_tp->rearrange_f1 = mt_cv_ss_rearrange_f1_avx;
    state_tp->rearrange_f2 = mt_cv_ss_rearrange_f2_avx;
#else
    state_tp->add = mt_cv_ss_add;
    state_tp->add_r = mt_cv_ss_add_r;
    state_tp->conv_1d = mt_cv_ss_conv1d;

    state_tp->linear = mt_cv_ss_linear;
    state_tp->linear_qkv = mt_cv_ss_linear_qkv;
    state_tp->mask_multiply = mt_cv_ss_mask_multiply;

    state_tp->mha = mt_cv_ss_mha;
    state_tp->mha1 = mt_cv_ss_mha1;
    state_tp->nm = mt_cv_ss_nm;

    state_tp->rearrange_f1 = mt_cv_ss_rearrange_f1;
    state_tp->rearrange_f2 = mt_cv_ss_rearrange_f2;
#endif



    return 0;

}
