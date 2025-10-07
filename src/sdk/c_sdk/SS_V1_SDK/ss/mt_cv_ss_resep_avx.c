/***************************************************************************
*** Copyright (c) Meeami Technologies
*** All rights reserved
***
*** This software embodies materials and concepts which are confidential
*** to Meeami Technologies and is made available solely pursuant to the terms of a
*** written license agreement with Meeami Technologies.
***
*** File Name: ms_cv_ss_hf.c
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

#include "mt_cv_ss_defines.h"
#include "mt_cv_ss_struct.h"
#include "mt_cv_ss_loc.h"
#include "mt_cv_ss_tbl.h"
#include <immintrin.h>


#if MT_CV_SS_INTRINSIC

#include <immintrin.h> // For AVX2 intrinsics

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
)
{
    int output_size = (input_size - kernel_size) / stride + 1;
    int op_cnt = 0;

    __m256 vec_sum;
    __m256 vec_input, vec_weight;

    for (int oc = 0; oc < out_channels; oc++) {
        for (int i = 0; i < output_size; i++) {
            float sum = bias[oc];
            vec_sum = _mm256_setzero_ps(); // Initialize the AVX register with zero

            for (int ic = 0; ic < in_channels; ic++) {
                int input_base = ic * input_size + i * stride;
                int weight_base = oc * in_channels * kernel_size + ic * kernel_size;

                // Process kernel_size = 16 using AVX2: Load 8 elements at a time
                for (int k = 0; k < kernel_size; k += 8) {
                    vec_input = _mm256_load_ps(&input[input_base + k]);
                    vec_weight = _mm256_load_ps(&weights[weight_base + k]);
                    vec_sum = _mm256_fmadd_ps(vec_input, vec_weight, vec_sum);
                }
            }

            float temp_sum[8];
            _mm256_store_ps(temp_sum, vec_sum);
            sum += temp_sum[0] + temp_sum[1] + temp_sum[2] + temp_sum[3] +
                temp_sum[4] + temp_sum[5] + temp_sum[6] + temp_sum[7];

            if (sum < 0)
                sum = 0;

            output[oc * output_size + i] = sum;
            op_cnt++;
        }
    }

    return op_cnt;
}

float _mm256_reduce_add_ps(__m256 vec) {
    __m256 hadd = _mm256_hadd_ps(vec, vec);  // Horizontal addition
    __m256 hadd2 = _mm256_hadd_ps(hadd, hadd);  // Horizontal addition again
    float result[8];
    _mm256_storeu_ps(result, hadd2);
    return result[0] + result[4];  // Sum the two remaining values
}




int mt_cv_ss_linear_qkv_avx(
    float* ip, float* qw, float* kw, float* vw,
    float* qb, float* kb, float* vb,
    int iclm, int ics, int row,
    float* q_op, float* k_op, float* v_op
) {
    float q_buf[8], k_buf[8], v_buf[8];
    float* A_lp, * B_lp, * C_lp, * D_lp;
    __m256 q_acc0, q_acc1, k_acc0, k_acc1, v_acc0, v_acc1;
    __m256 a0, a1, b0, b1, c0, c1, d0, d1;
    __m256 q_total, k_total, v_total;

    for (int i = 0; i < iclm; i++) {
        A_lp = &ip[i * row];

        for (int j = 0; j < ics; j++) {
            B_lp = &qw[j * row];
            C_lp = &kw[j * row];
            D_lp = &vw[j * row];

            q_acc0 = _mm256_setzero_ps();
            q_acc1 = _mm256_setzero_ps();
            k_acc0 = _mm256_setzero_ps();
            k_acc1 = _mm256_setzero_ps();
            v_acc0 = _mm256_setzero_ps();
            v_acc1 = _mm256_setzero_ps();

            for (int k = 0; k < row; k += 16) {
                a0 = _mm256_loadu_ps(&A_lp[k]);
                a1 = _mm256_loadu_ps(&A_lp[k + 8]);

                b0 = _mm256_loadu_ps(&B_lp[k]);
                b1 = _mm256_loadu_ps(&B_lp[k + 8]);

                c0 = _mm256_loadu_ps(&C_lp[k]);
                c1 = _mm256_loadu_ps(&C_lp[k + 8]);

                d0 = _mm256_loadu_ps(&D_lp[k]);
                d1 = _mm256_loadu_ps(&D_lp[k + 8]);

                q_acc0 = _mm256_fmadd_ps(a0, b0, q_acc0);
                q_acc1 = _mm256_fmadd_ps(a1, b1, q_acc1);

                k_acc0 = _mm256_fmadd_ps(a0, c0, k_acc0);
                k_acc1 = _mm256_fmadd_ps(a1, c1, k_acc1);

                v_acc0 = _mm256_fmadd_ps(a0, d0, v_acc0);
                v_acc1 = _mm256_fmadd_ps(a1, d1, v_acc1);
            }

            q_total = _mm256_add_ps(q_acc0, q_acc1);
            k_total = _mm256_add_ps(k_acc0, k_acc1);
            v_total = _mm256_add_ps(v_acc0, v_acc1);

            _mm256_storeu_ps(q_buf, q_total);
            _mm256_storeu_ps(k_buf, k_total);
            _mm256_storeu_ps(v_buf, v_total);

            float tq = 0, tk = 0, tv = 0;
            for (int m = 0; m < 8; m++) {
                tq += q_buf[m];
                tk += k_buf[m];
                tv += v_buf[m];
            }

            q_op[i * ics + j] = tq + qb[j];
            k_op[i * ics + j] = tk + kb[j];
            v_op[i * ics + j] = tv + vb[j];
        }
    }
    return 0;
}



int mt_cv_ss_linear_avx(
    float* ip,
    float* w,
    float* bias,
    int iclm,
    int ics,
    int row,
    float* op
)
{
    for (int i = 0; i < iclm; i++) {
        float* A_lp = &ip[i * row];

        for (int j = 0; j < ics; j++) {
            float* B_lp = &w[j * row];
            __m256 sum1 = _mm256_setzero_ps();
            __m256 sum2 = _mm256_setzero_ps();

            for (int k = 0; k < row; k += 16) {
                __m256 A1 = _mm256_load_ps(&A_lp[k]);
                __m256 B1 = _mm256_load_ps(&B_lp[k]);
                sum1 = _mm256_fmadd_ps(A1, B1, sum1);

                __m256 A2 = _mm256_load_ps(&A_lp[k + 8]);
                __m256 B2 = _mm256_load_ps(&B_lp[k + 8]);
                sum2 = _mm256_fmadd_ps(A2, B2, sum2);
            }

            __m256 sum = _mm256_add_ps(sum1, sum2);

            // Horizontal sum (AVX + SSE)
            __m128 low = _mm256_castps256_ps128(sum);
            __m128 high = _mm256_extractf128_ps(sum, 1);
            __m128 temp = _mm_add_ps(low, high);
            temp = _mm_hadd_ps(temp, temp);
            temp = _mm_hadd_ps(temp, temp);

            float total_sum = _mm_cvtss_f32(temp);
            op[i * ics + j] = total_sum + bias[j];
        }
    }

    return 0;
}



void mt_cv_ss_add_avx(
    float* ip1,
    float* ip2,
    int channel,
    int column,
    int row,
    float* op
)
{
    int total_elements = channel * column * row;
    int i = 0;

    // Process 8 elements at a time using AVX
    for (; i <= total_elements - 8; i += 8)
    {
        __m256 a = _mm256_loadu_ps(&ip1[i]);
        __m256 b = _mm256_loadu_ps(&ip2[i]);
        __m256 result = _mm256_add_ps(a, b);
        _mm256_storeu_ps(&op[i], result);
    }

    for (; i < total_elements; i++)
    {
        op[i] = ip1[i] + ip2[i];
    }
}

void mt_cv_ss_mha_avx(
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
) {
    int dim = head_dim * channel;
    float temp = 1.0f / sqrtf((float)d);

    for (int p_l = 0; p_l < dim; p_l++) {
        for (int i_l = 0; i_l < seg_size; i_l++) {
            float* A_lp = &q[i_l * d + p_l * seg_size * d];
            for (int j_l = 0; j_l < seg_size; j_l++) {
                float* B_lp = &k[j_l * d + p_l * seg_size * d];
                __m256 sum_vec = _mm256_setzero_ps();

                for (int k_l = 0; k_l < d; k_l += 8) {
                    __m256 a = _mm256_loadu_ps(&A_lp[k_l]);
                    __m256 b = _mm256_loadu_ps(&B_lp[k_l]);
                    sum_vec = _mm256_fmadd_ps(a, b, sum_vec);
                }

                __m128 low = _mm256_castps256_ps128(sum_vec);
                __m128 high = _mm256_extractf128_ps(sum_vec, 1);
                __m128 sum128 = _mm_add_ps(low, high);
                sum128 = _mm_hadd_ps(sum128, sum128);
                sum128 = _mm_hadd_ps(sum128, sum128);
                float sum = _mm_cvtss_f32(sum128);

                op[(i_l * seg_size + j_l) + p_l * seg_size * seg_size] = sum * temp;
            }
        }
    }

    for (int p_l = 0; p_l < dim; p_l++) {
        for (int i_l = 0; i_l < seg_size; i_l++) {
            float* A_lp = &op[i_l * seg_size + p_l * seg_size * seg_size];
            float sum = 0.0f;
            float tmp_op[250]; // Assume seg_size <= 250

            for (int j_l = 0; j_l < seg_size; j_l++) {
                tmp_op[j_l] = expf(A_lp[j_l]);
                sum += tmp_op[j_l];
            }

            for (int j_l = 0; j_l < seg_size; j_l++) {
                A_lp[j_l] = tmp_op[j_l] / sum;
            }
        }
    }


    for (int p_l = 0; p_l < dim; p_l++) {
        for (int i_l = 0; i_l < seg_size; i_l++) {
            float* A_lp = &op[i_l * seg_size + p_l * seg_size * seg_size];
            float* C_lp = &q[i_l * d * channel * head_dim + p_l * d];

            for (int j_l = 0; j_l < d; j_l++) {
                float* B_lp = &v[j_l + p_l * d * seg_size];
                float sum = 0.0f;

                for (int k_l = 0; k_l < seg_size; k_l++) {
                    sum += A_lp[k_l] * B_lp[k_l * d];
                }

                C_lp[j_l] = sum;
            }
        }
    }


    for (int p_l = 0; p_l < seg_size * channel; p_l++) {
        float* A_lp = &q[p_l * row];

        for (int i_l = 0; i_l < row; i_l++) {
            float* B_lp = &out_proj_w[i_l * row];
            __m256 sum_vec = _mm256_setzero_ps();

            for (int j_l = 0; j_l < row; j_l += 8) {
                __m256 a = _mm256_loadu_ps(&A_lp[j_l]);
                __m256 b = _mm256_loadu_ps(&B_lp[j_l]);
                sum_vec = _mm256_fmadd_ps(a, b, sum_vec);
            }

            __m128 low = _mm256_castps256_ps128(sum_vec);
            __m128 high = _mm256_extractf128_ps(sum_vec, 1);
            __m128 sum128 = _mm_add_ps(low, high);
            sum128 = _mm_hadd_ps(sum128, sum128);
            sum128 = _mm_hadd_ps(sum128, sum128);
            float sum = _mm_cvtss_f32(sum128);

            op[i_l + p_l * row] = sum + out_proj_b[i_l];
        }
    }
}
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
)
{
    int i_l, j_l, k_l, p_l = 0, d_l = 0;
    float sum = 0;
    float temp, * A_lp, * B_lp, * C_lp;
    float tmp_op[250];

    int dim = head_dim * channel;
    temp = sqrt(1.0 / ((float)(d)));
    p_l = 0; d_l = 0;
    //linear operation
    __m256 A_vals, B_vals, mul_vals, sum_vals;
    float exp_scores[250]; // Adjust size as needed

    for (i_l = 0; i_l < dim * seg_size; i_l++) {
        const float* A_lp = &q[i_l * d];
        float sum = 0.0f;

        for (j_l = 0; j_l < seg_size; j_l++) {
            const float* B_lp = &k[j_l * d + d_l * seg_size * d];

            __m256 vdot = _mm256_setzero_ps();
            for (k_l = 0; k_l < d; k_l += 8) {
                __m256 a = _mm256_loadu_ps(&A_lp[k_l]);
                __m256 b = _mm256_loadu_ps(&B_lp[k_l]);
                vdot = _mm256_fmadd_ps(a, b, vdot); // vdot += a * b
            }

            // Horizontal sum of vdot
            __m128 low = _mm256_castps256_ps128(vdot);
            __m128 high = _mm256_extractf128_ps(vdot, 1);
            __m128 sum128 = _mm_add_ps(low, high);
            sum128 = _mm_hadd_ps(sum128, sum128);
            sum128 = _mm_hadd_ps(sum128, sum128);
            float dot = _mm_cvtss_f32(sum128);

            float scaled = dot * temp;
            float exp_val = expf(scaled);  // Can replace with fast exp approximation
            exp_scores[j_l] = exp_val;
            sum += exp_val;
        }

        float inv_sum = 1.0f / sum;
        for (j_l = 0; j_l < seg_size; j_l++) {
            op[i_l * seg_size + j_l] = exp_scores[j_l] * inv_sum;
        }

        p_l++;
        if (p_l == seg_size) {
            d_l++;
            p_l = 0;
        }
    }


    for (p_l = 0; p_l < dim; p_l++) {
        for (i_l = 0; i_l < seg_size; i_l++) {
            const float* A_lp = &op[i_l * seg_size + p_l * seg_size * seg_size];
            float* C_lp = &q[i_l * d * channel * head_dim + p_l * d];

            for (j_l = 0; j_l < d; j_l++) {
                const float* B_lp = &v[j_l + p_l * d * seg_size];
                __m256 vsum = _mm256_setzero_ps();

                for (k_l = 0; k_l < seg_size; k_l += 8) {
                    // Load 8 floats from A
                    __m256 a_vec = _mm256_loadu_ps(&A_lp[k_l]);

                    // Gather 8 floats from B with stride `d`
                    __m256 b_vec = _mm256_set_ps(
                        B_lp[(k_l + 7) * d],
                        B_lp[(k_l + 6) * d],
                        B_lp[(k_l + 5) * d],
                        B_lp[(k_l + 4) * d],
                        B_lp[(k_l + 3) * d],
                        B_lp[(k_l + 2) * d],
                        B_lp[(k_l + 1) * d],
                        B_lp[(k_l + 0) * d]);

                    // Multiply and accumulate
                    vsum = _mm256_fmadd_ps(a_vec, b_vec, vsum);
                }

                // Horizontal sum of vsum
                __m128 low = _mm256_castps256_ps128(vsum);
                __m128 high = _mm256_extractf128_ps(vsum, 1);
                __m128 sum128 = _mm_add_ps(low, high);
                sum128 = _mm_hadd_ps(sum128, sum128);
                sum128 = _mm_hadd_ps(sum128, sum128);
                float sum = _mm_cvtss_f32(sum128);

                C_lp[j_l] = sum;
            }
        }
    }




    for (p_l = 0; p_l < seg_size * channel; p_l++) {
        float* A_lp = &q[p_l * row];
        for (i_l = 0; i_l < row; i_l++) {
            float* B_lp = &out_proj_w[i_l * row];
            sum = 0.0f;
            sum_vals = _mm256_setzero_ps();  // Initialize accumulator for AVX sum

            // Process 8 elements at a time using AVX
            for (j_l = 0; j_l < row; j_l += 8) {
                A_vals = _mm256_loadu_ps(&A_lp[j_l]);  // Load 8 values from A_lp
                B_vals = _mm256_loadu_ps(&B_lp[j_l]);  // Load 8 values from B_lp

                // Multiply the 8 values at once
                mul_vals = _mm256_mul_ps(A_vals, B_vals);

                // Accumulate the result
                sum_vals = _mm256_add_ps(sum_vals, mul_vals);
            }

            // Reduce the sum from the 8 values to a single scalar value
            float result[8];
            _mm256_storeu_ps(result, sum_vals);
            for (int k = 0; k < 8; k++) {
                sum += result[k];
            }

            // Store the final value in op with the bias added
            op[i_l + p_l * row] = sum + out_proj_b[i_l];
        }
    }


}





void mt_cv_ss_add_r_avx(
    float* ip1,
    float* ip2,
    int channel,
    int column,
    int row,
    float* op
)
{
    int i_l, j_l, k_l, p_l = 0;
    float* A_lp;
    float* B_lp;

    // Loop over channels
    for (i_l = 0; i_l < channel; i_l++)
    {
        A_lp = &ip2[i_l * row];  // Base pointer for ip2
        for (j_l = 0; j_l < column; j_l++)
        {
            B_lp = &ip1[j_l * row + i_l * row * column]; // Base pointer for ip1

            // Process 8 elements at a time using AVX
            for (k_l = 0; k_l < row; k_l += 8)
            {
                __m256 a = _mm256_loadu_ps(&A_lp[k_l]);
                __m256 b = _mm256_loadu_ps(&B_lp[k_l]);
                __m256 result = _mm256_add_ps(a, b);
                _mm256_storeu_ps(&op[p_l], result);
                p_l += 8;
            }
        }
    }
}


void mt_cv_ss_rearrange_f1_avx(
    float* ip,
    float* op,
    int channel,
    int column,
    int row
)
{
    int i_l, j_l, k_l = 0;
    float* A_lp;

    for (i_l = 0; i_l < channel * column; i_l++)
    {
        if (i_l % column == 0) {
            k_l = i_l / column;
        }

        A_lp = &ip[k_l * row];
        k_l += channel;

        // Process 8 elements at a time using AVX
        for (j_l = 0; j_l < row; j_l += 8)
        {
            __m256 data = _mm256_loadu_ps(&A_lp[j_l]);
            _mm256_storeu_ps(&op[j_l + i_l * row], data);
        }
    }
}


void mt_cv_ss_rearrange_f2_avx(
    float* ip,
    float* op,
    int channel,
    int column,
    int row
)
{
    float* A_lp, * B_lp;
    int i_l, j_l, k_l = 1;

    for (i_l = 0; i_l < channel * column; i_l++)
    {
        if (i_l % column == 0) {
            k_l = i_l / column;
        }

        A_lp = &ip[i_l * row];
        B_lp = &op[k_l * row];
        k_l += channel;

        // Process 8 elements at a time using AVX
        for (j_l = 0; j_l < row; j_l += 8)
        {
            __m256 data = _mm256_loadu_ps(&A_lp[j_l]);  // Load 8 floats from input
            _mm256_storeu_ps(&B_lp[j_l], data);  // Store them in output
        }
    }
}


void mt_cv_ss_mask_multiply_avx(
    float* ip,
    float* mask,
    float* op,
    int row,
    int column
)
{
    int total = row * column;
    int i;

    // Process 8 floats at a time using AVX
    for (i = 0; i < total; i += 8)
    {
        __m256 vec_ip = _mm256_loadu_ps(&ip[i]);    // Load 8 floats from ip
        __m256 vec_mask = _mm256_loadu_ps(&mask[i]); // Load 8 floats from mask
        __m256 vec_op = _mm256_mul_ps(vec_ip, vec_mask); // Multiply
        _mm256_storeu_ps(&op[i], vec_op);           // Store result in op
    }
}



void mt_cv_ss_nm_avx(
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
    float eps = 1e-8F;
    float* A_lp, * D_lp;
    __m256 mean_vec, var_vec, inv_std_vec, temp_vec;
    __m256 B_vec, C_vec, A_vec, D_vec;

    for (i_l = 0; i_l < iclm; i_l++)
    {
        A_lp = &A[i_l * ics];
        D_lp = &D[i_l * ics];
        double tsum_l = 0.0;
        double tsum_sq = 0.0;

        // Compute mean
        for (j_l = 0; j_l < ics; j_l++)
            tsum_l += (double)A_lp[j_l];

        float mean = (float)(tsum_l / (double)ics);

        // Compute variance
        for (j_l = 0; j_l < ics; j_l++)
        {
            float va = A_lp[j_l] - mean;
            D_lp[j_l] = B[j_l] * va;
            tsum_sq += (double)(va * va);
        }

        float var = (float)(tsum_sq / (double)ics);
        float inv_std = 1.0f / sqrtf(var + eps);
        E[i_l] = inv_std;

        // Apply normalization with AVX
        inv_std_vec = _mm256_set1_ps(inv_std);

        for (j_l = 0; j_l < ics; j_l += 8)
        {
            B_vec = _mm256_loadu_ps(&B[j_l]);
            C_vec = _mm256_loadu_ps(&C[j_l]);
            D_vec = _mm256_loadu_ps(&D_lp[j_l]);

            // Normalize: D[i] = (D[i] * inv_std) + C[i]
            D_vec = _mm256_fmadd_ps(D_vec, inv_std_vec, C_vec);

            _mm256_storeu_ps(&D_lp[j_l], D_vec);
        }
    }
}


#endif