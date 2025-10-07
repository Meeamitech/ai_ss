/***************************************************************************
*** Copyright (c) Meeami Technologies
*** All rights reserved
***
*** This software embodies materials and concepts which are confidential
*** to Meeami Technologies and is made available solely pursuant to the terms of a
*** written license agreement with Meeami Technologies.
***
*** File Name: ms_cv_ss_resep.c
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

/* High level header files */
//#include "mt_cv_ss_fft.h"
#include "mt_cv_ss_defines.h"
#include "mt_cv_ss_struct.h"
#include "mt_cv_ss_loc.h"
#include "mt_cv_ss_tbl.h"
#include <immintrin.h>



void mt_cv_ss_gnm(
    float* A,
    float* B,
    float* C,
    float* D,
    int channel,
    int ics,
    int iclm
)
{
    int ch, i, j;
    float* A_lp;
    float eps = 1e-8F;
    float sum, sq_sum, inv_std;
    int total_elements = iclm * ics;
    float val;
    float mean[200], var[200];
    // Compute mean and variance per channel
    for (ch = 0; ch < channel; ch++) {
        sum = 0.0f;
        sq_sum = 0.0f;
        A_lp = &A[ch * ics * iclm];
        for (i = 0; i < iclm * ics; i++)
        {
            val = A_lp[i];
            sum += val;
            sq_sum += val * val;
        }
        mean[ch] = sum / total_elements;
        var[ch] = (sq_sum / total_elements) - (mean[ch] * mean[ch]);
    }


    for (ch = 0; ch < channel; ch++) {
        inv_std = 1.0f / sqrt(var[ch] + eps);

        for (i = 0; i < iclm; i++) {
            for (j = 0; j < ics; j++) {
                int idx = (i * ics) + j + (ics * iclm * ch);
                D[idx] = ((A[idx] - mean[ch]) * inv_std) * B[i] + C[i];
            }
        }
    }
}

void
mt_cv_ss_mean
(
    float* ip,
    float* op,
    int ch,
    int column,
    int row
)
{
    int i_l, j_l, k_l = 0, l = 0, p_l = 0;
    float sum = 0;
    float* A_lp, * B_lp;

    for (i_l = 0; i_l < ch; i_l++)
    {
        for (j_l = 0; j_l < row; j_l++)
        {
            A_lp = &ip[column * row * i_l + j_l];
            sum = 0;
            p_l = 0;
            for (k_l = 0; k_l < column; k_l++)
            {
                sum = sum + A_lp[p_l];
                p_l = p_l + row;
            }
            op[row * i_l + j_l] = sum / column;
        }
    }


}


void
mt_cv_ss_view
(
    float* ip,
    float* op,
    int channel,
    int iclm,
    int ics,
    int head_dim
)
{
    int i, j, k, l, m = 0;;
    int p = 0;

    float* A_lp;

    for (l = 0; l < channel; l++)
    {
        for (i = 0; i < ics; i = i + head_dim)
        {
            A_lp = &ip[l * iclm * ics + i];
            m = 0;
            for (j = 0; j < iclm; j++)
            {
                for (k = 0; k < head_dim; k++)
                {
                    op[p++] = A_lp[m];
                    m++;
                }

                m = m + 128 - 16;
            }
        }
    }

}







void
mt_cv_ss_rearrange_f3
(
    float* ip,
    float* op,
    int channel,
    int column,
    int row
)
{
    float* A_lp;
    int i_l, j_l, k_l = 0;

    for (i_l = 0; i_l < channel * column; i_l++)
    {

        A_lp = &ip[i_l * row];
        if (i_l % column == 0)
            k_l = i_l * row;

        for (j_l = 0; j_l < row; j_l++)
        {
            op[j_l * column + k_l] = A_lp[j_l];
        }

        k_l++;

    }

}

void
mt_cv_ss_rearrange_f4
(
    float* ip,
    float* op,
    int channel,
    int column,
    int row
)
{
    float* A_lp;
    int i_l, j_l, k_l = 0;

    for (i_l = 0; i_l < channel * column; i_l++)
    {

        A_lp = &ip[i_l * row];

        for (j_l = 0; j_l < row; j_l++)
        {
            op[j_l * column + k_l] = A_lp[j_l];
        }

        k_l++;

    }

}



void
mt_cv_ss_rearrange_f5
(
    float* ip,
    float* op,
    int channel,
    int column,
    int row
)
{
    float* A_lp;
    int i_l, j_l, k_l = 0, p_l = 0;

    for (i_l = 0; i_l < channel * column; i_l++)
    {

        A_lp = &ip[i_l * row];

        if (i_l % 2 == 0)
        {
            for (j_l = 0; j_l < row; j_l++)
            {
                op[row * (p_l)+j_l] = A_lp[j_l];
            }
            p_l++;
        }
        else
        {
            for (j_l = 0; j_l < row; j_l++)
            {
                op[j_l + row * ((column / 2) + k_l)] = A_lp[j_l];
            }
            k_l++;
        }
    }
}


void
mt_cv_ss_relu
(
    float* ip,
    float* op,
    int channel,
    int column,
    int row
)
{
    int i_l, j_l, k_l;
    float* A_lp;

    for (i_l = 0; i_l < channel * column * row; i_l++)
    {
        if (ip[i_l] > 0)
            op[i_l] = ip[i_l];
        else
            op[i_l] = 0;
    }


}


void
mt_cv_ss_prelu
(
    float* ip,
    float* op,
    float alpha,
    int channel,
    int column,
    int row

)
{
    int i_l, j_l, k_l;
    float* A_lp;

    for (i_l = 0; i_l < channel * column * row; i_l++)
    {
        if (ip[i_l] > 0)
            op[i_l] = ip[i_l];
        else
            op[i_l] = alpha * ip[i_l];
    }


}


int
mt_cv_ss_conv1d_transpose(
    float* input,
    float* weights,
    float* output,
    int in_channels,
    int out_channels,
    int input_size,
    int kernel_size,
    int stride
)
{
    int output_size;
    output_size = (input_size - 1) * stride + kernel_size;

    memset(output, 0, out_channels * output_size * sizeof(float));



    // Iterate over each input channel
    for (int in_c = 0; in_c < in_channels; in_c++) {
        // Iterate over each input position
        for (int i = 0; i < input_size; i++) {
            int out_start = i * stride;  // Start position in output
            // Apply kernel
            for (int k = 0; k < kernel_size; k++) {
                if (out_start + k < output_size) {  // Bounds check
                    output[out_start + k] +=
                        input[in_c * input_size + i] * weights[in_c * kernel_size + k];
                }
            }
        }
    }
}



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
)
{
    int output_size = (input_size - kernel_size) / stride + 1;
    int op_cnt = 0;


    for (int oc = 0; oc < out_channels; oc++) {
        for (int i = 0; i < output_size; i++) {
            float sum = bias[oc];
            for (int ic = 0; ic < in_channels; ic++)
            {
                int input_index = ic * input_size + i * stride;
                int weight_index = oc * in_channels + ic;
                sum += input[input_index] * weights[weight_index];
            }

            if (sum < 0)
                sum = 0;
            output[oc * output_size + i] = sum;
            op_cnt++;
        }
    }
    return op_cnt;
}







//#ifndef MT_CV_ss_INTRINSIC

#if 1

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
)
{
    int i_l, j_l, k_l, p_l, l_l;
    float sum = 0;
    float temp, * A_lp, * B_lp, * C_lp;
    float tmp_op[250];

    int dim = head_dim * channel;
    temp = sqrt(1.0 / ((float)(d)));




    for (p_l = 0; p_l < dim; p_l++)
    {
        for (i_l = 0; i_l < seg_size; i_l++)
        {
            A_lp = &q[i_l * d + p_l * (seg_size)*d];
            sum = 0;
            for (j_l = 0; j_l < seg_size; j_l++)
            {
                B_lp = &k[j_l * d + p_l * seg_size * d];
                sum = 0;
                for (k_l = 0; k_l < d; k_l += 8) {
                    sum += A_lp[k_l + 0] * B_lp[k_l + 0];
                    sum += A_lp[k_l + 1] * B_lp[k_l + 1];
                    sum += A_lp[k_l + 2] * B_lp[k_l + 2];
                    sum += A_lp[k_l + 3] * B_lp[k_l + 3];
                    sum += A_lp[k_l + 4] * B_lp[k_l + 4];
                    sum += A_lp[k_l + 5] * B_lp[k_l + 5];
                    sum += A_lp[k_l + 6] * B_lp[k_l + 6];
                    sum += A_lp[k_l + 7] * B_lp[k_l + 7];
                }
                op[(i_l * seg_size + j_l) + p_l * seg_size * seg_size] = sum * temp;
            }
        }
    }

    for (p_l = 0; p_l < dim; p_l++)
    {
        for (i_l = 0; i_l < seg_size; i_l++)
        {
            A_lp = &op[i_l * seg_size + p_l * (seg_size)*seg_size];
            sum = 0;
            for (j_l = 0; j_l < seg_size; j_l++)
            {
                tmp_op[j_l] = exp(A_lp[j_l]);
                sum += tmp_op[j_l];
            }
            for (j_l = 0; j_l < seg_size; j_l++)
            {

                op[(i_l * seg_size + j_l) + p_l * seg_size * seg_size] = tmp_op[j_l] / sum;
            }
        }
    }

    //multiplication with v
    for (p_l = 0; p_l < dim; p_l++)
    {
        for (i_l = 0; i_l < seg_size; i_l++)
        {
            A_lp = &op[i_l * seg_size + p_l * (seg_size)*seg_size];

            C_lp = &q[i_l * d * channel * head_dim + p_l * d];

            sum = 0;
            for (j_l = 0; j_l < d; j_l++)
            {
                B_lp = &v[j_l + p_l * d * seg_size];
                sum = 0;
                for (k_l = 0; k_l < seg_size; k_l++)
                {
                    sum = sum + A_lp[k_l] * B_lp[k_l * d];
                }
                C_lp[j_l] = sum;
            }
        }
    }
    //linear operation
    for (p_l = 0; p_l < seg_size * channel; p_l++)
    {
        A_lp = &q[p_l * row];
        for (i_l = 0; i_l < row; i_l++)
        {
            B_lp = &out_proj_w[i_l * row];
            sum = 0;
            for (j_l = 0; j_l < row; j_l += 16) {
                sum += A_lp[j_l] * B_lp[j_l];
                sum += A_lp[j_l + 1] * B_lp[j_l + 1];
                sum += A_lp[j_l + 2] * B_lp[j_l + 2];
                sum += A_lp[j_l + 3] * B_lp[j_l + 3];
                sum += A_lp[j_l + 4] * B_lp[j_l + 4];
                sum += A_lp[j_l + 5] * B_lp[j_l + 5];
                sum += A_lp[j_l + 6] * B_lp[j_l + 6];
                sum += A_lp[j_l + 7] * B_lp[j_l + 7];
                sum += A_lp[j_l + 8] * B_lp[j_l + 8];
                sum += A_lp[j_l + 9] * B_lp[j_l + 9];
                sum += A_lp[j_l + 10] * B_lp[j_l + 10];
                sum += A_lp[j_l + 11] * B_lp[j_l + 11];
                sum += A_lp[j_l + 12] * B_lp[j_l + 12];
                sum += A_lp[j_l + 13] * B_lp[j_l + 13];
                sum += A_lp[j_l + 14] * B_lp[j_l + 14];
                sum += A_lp[j_l + 15] * B_lp[j_l + 15];
            }

            op[i_l + p_l * row] = sum + out_proj_b[i_l];
        }
    }

}


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
)
{
    int i_l, j_l, k_l, p_l = 0, d_l = 0;
    float sum = 0;
    float temp, * A_lp, * B_lp, * C_lp;
    float tmp_op[250];

    int dim = head_dim * channel;
    temp = sqrt(1.0 / ((float)(d)));
    p_l = 0; d_l = 0;

    for (i_l = 0; i_l < dim * seg_size; i_l++) {
        A_lp = &q[i_l * d];
        float exp_scores[250]; // store exp(dot)
        float sum = 0.0f;

        for (j_l = 0; j_l < seg_size; j_l++) {
            B_lp = &k[j_l * d + d_l * seg_size * d];
            float dot = 0.0f;

            for (k_l = 0; k_l < d; k_l += 8) {
                dot += A_lp[k_l + 0] * B_lp[k_l + 0];
                dot += A_lp[k_l + 1] * B_lp[k_l + 1];
                dot += A_lp[k_l + 2] * B_lp[k_l + 2];
                dot += A_lp[k_l + 3] * B_lp[k_l + 3];
                dot += A_lp[k_l + 4] * B_lp[k_l + 4];
                dot += A_lp[k_l + 5] * B_lp[k_l + 5];
                dot += A_lp[k_l + 6] * B_lp[k_l + 6];
                dot += A_lp[k_l + 7] * B_lp[k_l + 7];
            }

            float scaled = dot * temp;
            float exp_val = expf(scaled);
            exp_scores[j_l] = exp_val;
            sum += exp_val;
        }

        for (j_l = 0; j_l < seg_size; j_l++) {
            op[i_l * seg_size + j_l] = exp_scores[j_l] / sum;
        }

        p_l++;
        if (p_l == seg_size) {
            d_l++;
            p_l = 0;
        }
    }


    //multiplication with v
    for (p_l = 0; p_l < dim; p_l++)
    {
        for (i_l = 0; i_l < seg_size; i_l++)
        {
            A_lp = &op[i_l * seg_size + p_l * (seg_size)*seg_size];

            C_lp = &q[i_l * d * channel * head_dim + p_l * d];

            sum = 0;
            for (j_l = 0; j_l < d; j_l++)
            {
                B_lp = &v[j_l + p_l * d * seg_size];
                sum = 0;
                for (k_l = 0; k_l < seg_size; k_l = k_l + 8)
                {
                    sum = sum + A_lp[k_l + 0] * B_lp[(k_l + 0) * d];
                    sum = sum + A_lp[k_l + 1] * B_lp[(k_l + 1) * d];
                    sum = sum + A_lp[k_l + 2] * B_lp[(k_l + 2) * d];
                    sum = sum + A_lp[k_l + 3] * B_lp[(k_l + 3) * d];
                    sum = sum + A_lp[k_l + 4] * B_lp[(k_l + 4) * d];
                    sum = sum + A_lp[k_l + 5] * B_lp[(k_l + 5) * d];
                    sum = sum + A_lp[k_l + 6] * B_lp[(k_l + 6) * d];
                    sum = sum + A_lp[k_l + 7] * B_lp[(k_l + 7) * d];
                }
                C_lp[j_l] = sum;
            }
        }
    }
    //linear operation
    for (p_l = 0; p_l < seg_size * channel; p_l++)
    {
        A_lp = &q[p_l * row];
        for (i_l = 0; i_l < row; i_l++)
        {
            B_lp = &out_proj_w[i_l * row];
            sum = 0;
            for (j_l = 0; j_l < row; j_l += 16) {
                sum += A_lp[j_l] * B_lp[j_l];
                sum += A_lp[j_l + 1] * B_lp[j_l + 1];
                sum += A_lp[j_l + 2] * B_lp[j_l + 2];
                sum += A_lp[j_l + 3] * B_lp[j_l + 3];
                sum += A_lp[j_l + 4] * B_lp[j_l + 4];
                sum += A_lp[j_l + 5] * B_lp[j_l + 5];
                sum += A_lp[j_l + 6] * B_lp[j_l + 6];
                sum += A_lp[j_l + 7] * B_lp[j_l + 7];
                sum += A_lp[j_l + 8] * B_lp[j_l + 8];
                sum += A_lp[j_l + 9] * B_lp[j_l + 9];
                sum += A_lp[j_l + 10] * B_lp[j_l + 10];
                sum += A_lp[j_l + 11] * B_lp[j_l + 11];
                sum += A_lp[j_l + 12] * B_lp[j_l + 12];
                sum += A_lp[j_l + 13] * B_lp[j_l + 13];
                sum += A_lp[j_l + 14] * B_lp[j_l + 14];
                sum += A_lp[j_l + 15] * B_lp[j_l + 15];
            }

            op[i_l + p_l * row] = sum + out_proj_b[i_l];
        }
    }

}
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
)
{
    int i, j, k;
    float* A_lp, * B_lp, * C_lp, * D_lp;
    float q_sum[8], k_sum[8], v_sum[8];
    float tq = 0, tk = 0, tv = 0;
    for (i = 0; i < iclm; i++)
    {
        A_lp = &ip[i * row];

        for (j = 0; j < ics; j++)
        {
            B_lp = &qw[j * row];
            C_lp = &kw[j * row];
            D_lp = &vw[j * row];
            for (k = 0; k < 8; k++) {
                q_sum[k] = 0.0f;
                k_sum[k] = 0.0f;
                v_sum[k] = 0.0f;
            }
            // Unroll the loop processing 8 elements at a time
            for (k = 0; k < row; k += 8)
            {
                // Compute 8 multiplications and additions at once
                q_sum[0] += A_lp[k + 0] * B_lp[k + 0];
                q_sum[1] += A_lp[k + 1] * B_lp[k + 1];
                q_sum[2] += A_lp[k + 2] * B_lp[k + 2];
                q_sum[3] += A_lp[k + 3] * B_lp[k + 3];
                q_sum[4] += A_lp[k + 4] * B_lp[k + 4];
                q_sum[5] += A_lp[k + 5] * B_lp[k + 5];
                q_sum[6] += A_lp[k + 6] * B_lp[k + 6];
                q_sum[7] += A_lp[k + 7] * B_lp[k + 7];
                k_sum[0] += A_lp[k + 0] * C_lp[k + 0];
                k_sum[1] += A_lp[k + 1] * C_lp[k + 1];
                k_sum[2] += A_lp[k + 2] * C_lp[k + 2];
                k_sum[3] += A_lp[k + 3] * C_lp[k + 3];
                k_sum[4] += A_lp[k + 4] * C_lp[k + 4];
                k_sum[5] += A_lp[k + 5] * C_lp[k + 5];
                k_sum[6] += A_lp[k + 6] * C_lp[k + 6];
                k_sum[7] += A_lp[k + 7] * C_lp[k + 7];
                v_sum[0] += A_lp[k + 0] * D_lp[k + 0];
                v_sum[1] += A_lp[k + 1] * D_lp[k + 1];
                v_sum[2] += A_lp[k + 2] * D_lp[k + 2];
                v_sum[3] += A_lp[k + 3] * D_lp[k + 3];
                v_sum[4] += A_lp[k + 4] * D_lp[k + 4];
                v_sum[5] += A_lp[k + 5] * D_lp[k + 5];
                v_sum[6] += A_lp[k + 6] * D_lp[k + 6];
                v_sum[7] += A_lp[k + 7] * D_lp[k + 7];
            }

            // After processing all elements, sum the results
            tq = 0; tk = 0; tv = 0;
            for (k = 0; k < 8; k++) {
                tq += q_sum[k];
                tk += k_sum[k];
                tv += v_sum[k];
            }

            // Store the result with the bias addition
            q_op[i * ics + j] = tq + qb[j];
            k_op[i * ics + j] = tk + kb[j];
            v_op[i * ics + j] = tv + vb[j];
        }
    }
    return 0;
}



int mt_cv_ss_linear
(
    float* ip,
    float* w,
    float* bias,
    int iclm,
    int ics,
    int row,
    float* op
)
{
    int i, j, k = 0, l = 0, p = 0;;
    float* A_lp, * B_lp;
    float tmp[128];
    float sum = 0;

    for (i = 0; i < iclm; i++)
    {
        A_lp = &ip[i * row];
        for (j = 0; j < ics; j++)
        {
            B_lp = &w[j * row];
            for (k = 0; k < row; k++)
            {
                sum = sum + A_lp[k] * B_lp[k];
            }
            op[i * ics + j] = sum + bias[j];
            sum = 0;
        }

    }
}

void
mt_cv_ss_add
(
    float* ip1,
    float* ip2,
    int channel,
    int column,
    int row,
    float* op
)
{
    int sum = 0;
    int i_l = 0, j_l = 0;


    for (i_l = 0; i_l < channel * column * row; i_l++)
    {

        op[i_l] = ip1[i_l] + ip2[i_l];

    }
}


void
mt_cv_ss_add_r
(
    float* ip1,
    float* ip2,
    int channel,
    int column,
    int row,
    float* op
)
{
    int sum = 0;
    int i_l = 0, j_l = 0, k_l = 0, p_l = 0;;
    float* A_lp, * B_lp;

    for (i_l = 0; i_l < channel; i_l++)
    {
        A_lp = &ip2[i_l * row];
        k_l = 0;
        for (j_l = 0; j_l < column; j_l++)
        {
            B_lp = &ip1[j_l * row + i_l * row * column];
            for (k_l = 0; k_l < row; k_l++)
            {
                op[p_l] = A_lp[k_l] + B_lp[k_l];
                p_l++;
            }
        }

    }
}


void
mt_cv_ss_rearrange_f1
(
    float* ip,
    float* op,
    int channel,
    int column,
    int row
)
{
    float* A_lp;
    int i_l, j_l, k_l = 0;

    for (i_l = 0; i_l < channel * column; i_l++)
    {
        if (i_l % column == 0) {
            k_l = i_l / column;
        }

        A_lp = &ip[k_l * row];
        k_l = k_l + channel;
        for (j_l = 0; j_l < row; j_l++)
        {
            op[j_l + i_l * row] = A_lp[j_l];
        }

    }

}


void
mt_cv_ss_rearrange_f2
(
    float* ip,
    float* op,
    int channel,
    int column,
    int row
)
{
    float* A_lp, * B_lp;
    int i_l, j_l, k_l = 1;;

    for (i_l = 0; i_l < channel * column; i_l++)
    {
        if (i_l % column == 0) {
            k_l = i_l / column;
        }
        A_lp = &ip[i_l * row];
        B_lp = &op[k_l * row];
        k_l = k_l + channel;
        for (j_l = 0; j_l < row; j_l++)
        {
            B_lp[j_l] = A_lp[j_l];
        }
    }

}


void
mt_cv_ss_mask_multiply
(
    float* ip,
    float* mask,
    float* op,
    int row,
    int column
)
{
    int i_l, j_l, k_l;


    for (i_l = 0; i_l < row * column; i_l++)
    {
        op[i_l] = ip[i_l] * mask[i_l];
    }
}

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
)
{
    int i_l, j_l;
    float tempf;
    float eps = 0.00000001F;
    float* A_lp, * D_lp;
    double tsum_l;
    float  mean = 0, va = 0;


    for (i_l = 0; i_l < iclm; i_l++)
    {
        A_lp = &A[i_l * ics];
        D_lp = &D[i_l * ics];
        tsum_l = 0;
        for (j_l = 0; j_l < ics; j_l++)
        {
            tsum_l += (double)A_lp[j_l];
        }
        mean = (float)((double)tsum_l / (double)ics);
        tsum_l = 0;
        for (j_l = 0; j_l < ics; j_l++)
        {
            va = A_lp[j_l] - mean;
            D_lp[j_l] = B[j_l] * va;
            tsum_l += (double)(va * va);
        }
        tempf = (float)((double)tsum_l / (double)ics);
        E[i_l] = (float)(1 / (sqrt(tempf + eps)));
    }
    for (i_l = 0; i_l < iclm; i_l++)
    {
        D_lp = &D[i_l * ics];
        for (j_l = 0; j_l < ics; j_l += 4)
        {
            D_lp[j_l] = (D_lp[j_l] * E[i_l]) + C[j_l];
            D_lp[j_l + 1] = (D_lp[j_l + 1] * E[i_l]) + C[j_l + 1];
            D_lp[j_l + 2] = (D_lp[j_l + 2] * E[i_l]) + C[j_l + 2];
            D_lp[j_l + 3] = (D_lp[j_l + 3] * E[i_l]) + C[j_l + 3];
        }
    }
}

#endif
