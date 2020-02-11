/*

Author:
  
  Jakub Lukac <xlukac09@stud.fit.vubr.cz> (inserting intrisnsic functions)

Objective:

  Individual project for Advanced Assembly Languages

Algorithm:
  
  Lanczos resampling

Task:
  
  Create an algorithm using SSE or MMX instructions to resample an image by the Lanczos algorithm (use either
  inline assembler or intrisnsic functions). Try to make the function run faster than the reference
  implementation. Fill your code in the prepared regions below.

Functions:

  CustomInit() is a one-time initialization function called from the constructor of an instance of the CLanczos class.

  CustomDeinit() is a one-time initialization function called from the destructor of an instance of the CLanczos class.

  Lanczos8Gray(const unsigned char *original_image, int original_w, int original_h, unsigned char *new_image, int new_w, int new_h)
  resamples the original grayscale image using a Lanzcos resampling method from the original size to the new one. The pointer
  original_image points at the input (original) grayscale image. Size of the original image is original_w x original_h pixels.
  The pointer new_image points at the output (new) grayscale image. Size of the new image is new_w x new_h pixels. Both pointers
  original_image and new_image are aligned to 16 bytes and memory for the output has been allocated.
  
*/

#include "lanczos.h"
#define _USE_MATH_DEFINES

#include <windows.h>
#include <conio.h>
#include <math.h>
#include <climits>

#include <intrin.h>
#include <immintrin.h>

#ifndef _IPA_BENCH_

#define v_inner_loop(n)                                                             \
    for (j = 0; j < v_count[i + n]; j += 8)                                         \
    {                                                                               \
        weights = _mm256_load_ps(p_weight + j);                                     \
        v_pixels_indices = _mm256_load_si256((__m256i *)(p_pixel + j));             \
        pixels = _mm256_i32gather_epi32((const int *)p_col, v_pixels_indices, 1);   \
        pixels = _mm256_and_si256(pixels, char_mask);                               \
        pixels_f = _mm256_cvtepi32_ps(pixels);                                      \
                                                                                    \
        pixels_f = _mm256_mul_ps(pixels_f, weights);                                \
        intensity_sums##n = _mm256_add_ps(intensity_sums##n, pixels_f);             \
    }

#define h_inner_loop(n)                                                             \
    for (j = 0; j < h_count[i + n]; j += 8)                                         \
    {                                                                               \
        weights = _mm256_load_ps(p_weight + j);                                     \
        pixels = _mm256_lddqu_si256((__m256i *)(p_row + p_pixel[j]));               \
        pixels_128 = _mm256_castsi256_si128(pixels);                                \
        pixels = _mm256_cvtepu8_epi32(pixels_128);                                  \
        pixels_f = _mm256_cvtepi32_ps(pixels);                                      \
                                                                                    \
        pixels_f = _mm256_mul_ps(pixels_f, weights);                                \
        intensity_sums##n = _mm256_add_ps(intensity_sums##n, pixels_f);             \
    }

void CLanczos::CustomInit()
{


    /*********************************************************/
    /* TO DO: Insert your one-time initialization code here. */
    /*********************************************************/


}

void CLanczos::CustomDeinit()
{


    /******************************************/
    /* TO DO: Insert your clean up code here. */
    /******************************************/


}

float mySinC(float x)
{
    if (x != 0) {
        x *= M_PI;
        return (sin(x) / x);
    }

    return 1;
}

float myLanczos8value(float x)
{
    if (x < 0) x = -x;
    if (x < 8) return (mySinC(x)*mySinC(x / 8.0));

    return 0;
}

void CLanczos::Lanczos8Gray(const unsigned char *original_image, int original_w, int original_h, unsigned char *new_image, int new_w, int new_h)
{

    // operator type changed from double to float, added the average error per pixel 0.002274 compered to reference implementation
    __int32 i, j, n;
    float xScale, yScale;

    float *h_weight = NULL;     // Weight contribution    [new_w][MAX_CONTRIBS]
    int *h_pixel = NULL;        // Pixel that contributes [new_w][MAX_CONTRIBS]
    int *h_count = NULL;        // How many contribution for the pixel [new_w]
    float *h_wsum = NULL;       // Sum of weights [new_w]

    float *v_weight = NULL;     // Weight contribution    [new_h][MAX_CONTRIBS]
    int *v_pixel = NULL;        // Pixel that contributes [new_h][MAX_CONTRIBS]
    int *v_count = NULL;        // How many contribution for the pixel [new_h]
    float *v_wsum = NULL;       // Sum of weights [new_h]

    unsigned char *tb = NULL;   // Temporary (intermediate buffer)

    // variables used in convolution
    float intensity; 
    const unsigned char *p_col;
    const unsigned char *p_row;
    __declspec(align(32)) unsigned char v_pixels_arr[32];

    // AVX registers
    __m256 intensity_sums0, intensity_sums1, intensity_sums2, intensity_sums3;
    __m256 intensity_sums4, intensity_sums5, intensity_sums6, intensity_sums7;
    __m256 weights, wsums, pixels_f;
    __m256d pixels_d;
    __m256i v_pixels_indices, pixels, zeros, char_mask, shift_mask, non_neg_mask;
    __m128i pixels_128;

    // variables used in pre-calculation
    float center;               // Center of current sampling 
    float weight;               // Current wight
    __int32 left;               // Left of current sampling
    __int32 right;              // Right of current sampling
    float *p_weight;            // Temporary pointer
    __int32 *p_pixel;           // Temporary pointer

    __int32 MAX_CONTRIBS_H, MAX_CONTRIBS_V;     // Almost-const: max number of contribution for current sampling
    float SCALED_RADIUS_H, SCALED_RADIUS_V;     // Almost-const: scaled radius for downsampling operations
    float FILTER_FACTOR_H, FILTER_FACTOR_V;     // Almost-const: filter factor for downsampling operations

    /* Preliminary (redundant ? ) check */
    if (original_w < 1 || original_h < 1 || original_image == NULL || new_w < 1 || new_h < 1 || new_image == NULL) return;

    if (new_w == original_w && new_h == original_h)
    {
        /* Same size => no resampling */
        CopyMemory(new_image, original_image, original_w*original_h);
        return;
    }

    // operators order changed
    xScale = ((float)original_w / new_w);
    yScale = ((float)original_h / new_h);

    if (xScale < 1.0)
    {
        /* Horizontal upsampling */
        FILTER_FACTOR_H = 1.0;
        SCALED_RADIUS_H = 8.0;
    }
    else
    {
        /* Horizontal downsampling */
        FILTER_FACTOR_H = 1.0 / xScale;
        SCALED_RADIUS_H = 8.0 * xScale;
    }

    if (yScale < 1.0)
    {
        FILTER_FACTOR_V = 1.0;
        SCALED_RADIUS_V = 8.0;
    }
    else
    {
        FILTER_FACTOR_V = 1.0 / yScale;
        SCALED_RADIUS_V = 8.0 * yScale;
    }

    MAX_CONTRIBS_H = (int)(2 * SCALED_RADIUS_H + 1);
    MAX_CONTRIBS_V = (int)(2 * SCALED_RADIUS_V + 1);
    // always divisible by 32
    MAX_CONTRIBS_H += 32 - (MAX_CONTRIBS_H % 32);
    MAX_CONTRIBS_V += 32 - (MAX_CONTRIBS_V % 32);

    /* Pre-allocating all of the needed memory */
    h_weight = (float *)_aligned_malloc(new_w*MAX_CONTRIBS_H*sizeof(float), 32);    /* weights */
    h_pixel = (int *)_aligned_malloc(new_w*MAX_CONTRIBS_H*sizeof(int), 32);         /* the contributing pixels */
    h_count = (int *)malloc(new_w*sizeof(int));                                     /* how may contributions for the target pixel */
    h_wsum = (float *)_aligned_malloc(new_w*sizeof(float), 32);                     /* sum of the weights for the target pixel */

    /* Pre-calculate filter contributions for a column */
    v_weight = (float *)_aligned_malloc(new_h*MAX_CONTRIBS_V*sizeof(float), 32);    /* weights */
    v_pixel = (int *)_aligned_malloc(new_h*MAX_CONTRIBS_V*sizeof(int), 32);         /* the contributing pixels */
    v_count = (int *)malloc(new_h*sizeof(int));                                     /* how may contributions for the target pixel */
    v_wsum = (float *)_aligned_malloc(new_h*sizeof(float), 32);                     /* sum of the weights for the target pixel */

    tb = (unsigned char *)_aligned_malloc(original_w*(new_h+7) + 32, 32);           /* added padding +7 rows */

    if (h_weight && h_pixel && h_count && h_wsum && v_weight && v_pixel && v_count && v_wsum && tb)
    {
        /* simulate _aligned_calloc */
        memset(h_weight, 0.0f, new_w*MAX_CONTRIBS_H*sizeof(float));
        memset(h_pixel, 0, new_w*MAX_CONTRIBS_H*sizeof(int));

        memset(v_weight, 0.0f, new_h*MAX_CONTRIBS_H*sizeof(float));
        memset(v_pixel, 0, new_h*MAX_CONTRIBS_H*sizeof(int));

        /* Vertical */

        /* Pre-calculate weights contribution for a column */
        for (i = 0; i < new_h; i++)
        {
            p_weight = v_weight + i*MAX_CONTRIBS_V;
            p_pixel = v_pixel + i*MAX_CONTRIBS_V;

            v_count[i] = 0;
            v_wsum[i] = 0.0;

            center = ((float)i) * yScale;
            left = (int)(center + 0.5 - SCALED_RADIUS_V);
            right = (int)(left + 2 * SCALED_RADIUS_V);

            for (j = left; j <= right; j++)
            {
                if (j < 0 || j >= original_h) continue;

                weight = myLanczos8value((center - j)*FILTER_FACTOR_V);

                if (weight == 0.0) continue;

                n = v_count[i]; /* Our current index */
                p_pixel[n] = original_w*j; // added multiplication
                p_weight[n] = weight;
                v_wsum[i] += weight;
                v_count[i]++; /* Increment the contribution count */
            }    /* j */

            v_wsum[i] = 1.0 / v_wsum[i]; // this removed division in convolution

        }    /* i */

        /* Filter vertically from input to temporary buffer */

        intensity = 0.0;
        // mask convert 4 pixels to one
        char_mask = _mm256_set1_epi32(0x000000FF);
        // mask shift partial sums next to each another
        shift_mask = _mm256_set_epi32(7, 3, 6, 2, 5, 1, 4, 0);
        zeros = _mm256_set_epi32(0, 0, 0, 0, 0, 0, 0, 0);

        for (n = 0; n < original_w; n++)
        {
            /* Here 'n' runs on the horizontal coordinate */

            p_col = original_image + n;

            for (i = 0; i < new_h; i += 8)
            {
                /* i runs on the horizontal coordinate */

                p_weight = v_weight + i * MAX_CONTRIBS_H;
                p_pixel = v_pixel + i * MAX_CONTRIBS_H;

                intensity_sums0 = _mm256_broadcast_ss(&intensity);

                v_inner_loop(0);

                p_weight += MAX_CONTRIBS_H;
                p_pixel += MAX_CONTRIBS_H;

                intensity_sums1 = _mm256_broadcast_ss(&intensity);

                v_inner_loop(1);

                p_weight += MAX_CONTRIBS_H;
                p_pixel += MAX_CONTRIBS_H;

                intensity_sums2 = _mm256_broadcast_ss(&intensity);

                v_inner_loop(2);

                p_weight += MAX_CONTRIBS_H;
                p_pixel += MAX_CONTRIBS_H;

                intensity_sums3 = _mm256_broadcast_ss(&intensity);

                v_inner_loop(3);

                p_weight += MAX_CONTRIBS_H;
                p_pixel += MAX_CONTRIBS_H;

                intensity_sums4 = _mm256_broadcast_ss(&intensity);

                v_inner_loop(4);

                p_weight += MAX_CONTRIBS_H;
                p_pixel += MAX_CONTRIBS_H;

                intensity_sums5 = _mm256_broadcast_ss(&intensity);

                v_inner_loop(5);

                p_weight += MAX_CONTRIBS_H;
                p_pixel += MAX_CONTRIBS_H;

                intensity_sums6 = _mm256_broadcast_ss(&intensity);

                v_inner_loop(6);

                p_weight += MAX_CONTRIBS_H;
                p_pixel += MAX_CONTRIBS_H;

                intensity_sums7 = _mm256_broadcast_ss(&intensity);

                v_inner_loop(7);

                // sum of two parts
                intensity_sums0 = _mm256_hadd_ps(intensity_sums0, intensity_sums1);
                intensity_sums2 = _mm256_hadd_ps(intensity_sums2, intensity_sums3);
                intensity_sums4 = _mm256_hadd_ps(intensity_sums4, intensity_sums5);
                intensity_sums6 = _mm256_hadd_ps(intensity_sums6, intensity_sums7);

                // sum of four parts
                intensity_sums0 = _mm256_hadd_ps(intensity_sums0, intensity_sums2);
                intensity_sums1 = _mm256_hadd_ps(intensity_sums4, intensity_sums6);

                // shift sums of four parts together
                intensity_sums0 = _mm256_permutevar8x32_ps(intensity_sums0, shift_mask);
                intensity_sums1 = _mm256_permutevar8x32_ps(intensity_sums1, shift_mask);

                // pre-load sum of weights
                wsums = _mm256_load_ps(v_wsum + i);

                pixels_f = _mm256_hadd_ps(intensity_sums0, intensity_sums1);

                // shift pixel values in right order
                pixels_d = _mm256_castps_pd(pixels_f);
                pixels_d = _mm256_permute4x64_pd(pixels_d, 0xD8);
                pixels_f = _mm256_castpd_ps(pixels_d);

                // normalized pixel values
                pixels_f = _mm256_mul_ps(pixels_f, wsums);

                // saturation to one byte 0 - 255
                pixels = _mm256_cvttps_epi32(pixels_f);
                non_neg_mask = _mm256_cmpgt_epi32(pixels, zeros);
                pixels = _mm256_and_si256(pixels, non_neg_mask);
                pixels = _mm256_packus_epi32(pixels, pixels);
                pixels = _mm256_permute4x64_epi64(pixels, 0xD8);
                pixels = _mm256_packus_epi16(pixels, pixels);

                // store
                //tb[n + i*original_w] = col;
                _mm256_store_si256((__m256i *)v_pixels_arr, pixels);
                for (int a = 0; a < 8; ++a) {
                    tb[n + (i + a)*original_w] = v_pixels_arr[a]; /* Temporary buffer original_w x new_h */
                }

            }    /* i */
        }    /* n */

        /* Horizontal */

        /* Pre-calculate weights contribution for a row */
        for (i = 0; i < new_w; i++)
        {
            p_weight = h_weight + i*MAX_CONTRIBS_H;
            p_pixel = h_pixel + i*MAX_CONTRIBS_H;
            h_count[i] = 0;
            h_wsum[i] = 0.0;

            center = ((float)i) * xScale;
            left = (int)((center + 0.5) - SCALED_RADIUS_H);
            right = (int)(left + 2 * SCALED_RADIUS_H);

            for (j = left; j <= right; j++)
            {
                if (j < 0 || j >= original_w) continue;

                weight = myLanczos8value((center - j)*FILTER_FACTOR_H);

                if (weight == 0.0) continue;

                n = h_count[i]; /* Since h_count[i] is our current index */
                p_pixel[n] = j;
                p_weight[n] = weight;
                h_wsum[i] += weight;
                h_count[i]++; /* Increment contribution count */
            }    /* j */

            h_wsum[i] = 1.0 / h_wsum[i]; // this removed division in convolution

        }    /* i */

        /* Filter horizontally from work to output */

        intensity = 0.125;

        shift_mask = _mm256_set_epi32(7, 3, 6, 2, 5, 1, 4, 0);
        zeros = _mm256_set_epi32(0, 0, 0, 0, 0, 0, 0, 0);

        for (n = 0; n < new_h; n++)
        {
            /* Here 'n' runs on the vertical coordinate */

            p_row = tb + n*original_w;

            for (i = 0; i < new_w; i += 8)
            {
                /* i runs on the horizontal coordinate */

                p_weight = h_weight + i * MAX_CONTRIBS_H;
                p_pixel = h_pixel + i * MAX_CONTRIBS_H;

                intensity_sums0 = _mm256_broadcast_ss(&intensity);

                h_inner_loop(0);

                p_weight += MAX_CONTRIBS_H;
                p_pixel += MAX_CONTRIBS_H;

                intensity_sums1 = _mm256_broadcast_ss(&intensity);

                h_inner_loop(1);

                p_weight += MAX_CONTRIBS_H;
                p_pixel += MAX_CONTRIBS_H;

                intensity_sums2 = _mm256_broadcast_ss(&intensity);

                h_inner_loop(2);

                p_weight += MAX_CONTRIBS_H;
                p_pixel += MAX_CONTRIBS_H;

                intensity_sums3 = _mm256_broadcast_ss(&intensity);

                h_inner_loop(3);

                p_weight += MAX_CONTRIBS_H;
                p_pixel += MAX_CONTRIBS_H;

                intensity_sums4 = _mm256_broadcast_ss(&intensity);

                h_inner_loop(4);

                p_weight += MAX_CONTRIBS_H;
                p_pixel += MAX_CONTRIBS_H;

                intensity_sums5 = _mm256_broadcast_ss(&intensity);

                h_inner_loop(5);

                p_weight += MAX_CONTRIBS_H;
                p_pixel += MAX_CONTRIBS_H;

                intensity_sums6 = _mm256_broadcast_ss(&intensity);

                h_inner_loop(6);

                p_weight += MAX_CONTRIBS_H;
                p_pixel += MAX_CONTRIBS_H;

                intensity_sums7 = _mm256_broadcast_ss(&intensity);

                h_inner_loop(7);

                // sum of two parts
                intensity_sums0 = _mm256_hadd_ps(intensity_sums0, intensity_sums1);
                intensity_sums2 = _mm256_hadd_ps(intensity_sums2, intensity_sums3);
                intensity_sums4 = _mm256_hadd_ps(intensity_sums4, intensity_sums5);
                intensity_sums6 = _mm256_hadd_ps(intensity_sums6, intensity_sums7);

                // sum of four parts
                intensity_sums0 = _mm256_hadd_ps(intensity_sums0, intensity_sums2);
                intensity_sums1 = _mm256_hadd_ps(intensity_sums4, intensity_sums6);

                // shift sums of four parts together
                intensity_sums0 = _mm256_permutevar8x32_ps(intensity_sums0, shift_mask);
                intensity_sums1 = _mm256_permutevar8x32_ps(intensity_sums1, shift_mask);

                // pre-load sum of weights
                wsums = _mm256_load_ps(h_wsum + i);

                pixels_f = _mm256_hadd_ps(intensity_sums0, intensity_sums1);

                // shift pixel values in right order
                pixels_d = _mm256_castps_pd(pixels_f);
                pixels_d = _mm256_permute4x64_pd(pixels_d, 0xD8);
                pixels_f = _mm256_castpd_ps(pixels_d);

                // normalized pixel values
                pixels_f = _mm256_mul_ps(pixels_f, wsums);
                
                // saturation to one byte 0 - 255
                pixels = _mm256_cvttps_epi32(pixels_f);
                non_neg_mask = _mm256_cmpgt_epi32(pixels, zeros);
                pixels = _mm256_and_si256(pixels, non_neg_mask);
                pixels = _mm256_packus_epi32(pixels, pixels);
                pixels = _mm256_permute4x64_epi64(pixels, 0xD8);
                pixels = _mm256_packus_epi16(pixels, pixels);

                // store
                _mm256_storeu_si256((__m256i *)(new_image + i + n*new_w), pixels);

            }    /* i */
        }    /* n */
    }

    if (tb) _aligned_free(tb);

    if (h_weight) _aligned_free(h_weight);
    if (h_pixel) _aligned_free(h_pixel);
    if (h_count) free(h_count);
    if (h_wsum) _aligned_free(h_wsum);

    if (v_weight) _aligned_free(v_weight);
    if (v_pixel) _aligned_free(v_pixel);
    if (v_count) free(v_count);
    if (v_wsum) _aligned_free(v_wsum);
}
#endif
