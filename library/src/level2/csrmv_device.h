#pragma once
#ifndef CSRMV_DEVICE_H
#define CSRMV_DEVICE_H

#include <hip/hip_runtime.h>

#if defined(__HIP_PLATFORM_HCC__)
// Swizzle-based reduction
template <rocsparse_int SUBWAVE_SIZE>
__device__ float reduction(float sum)
{
    // clang-format off
    if(SUBWAVE_SIZE > 32) sum += hc::__amdgcn_readlane(sum, 32);
    if(SUBWAVE_SIZE > 16) sum += hc::__amdgcn_ds_swizzle(sum, 0x401f);
    if(SUBWAVE_SIZE >  8) sum += hc::__amdgcn_ds_swizzle(sum, 0x201f);
    if(SUBWAVE_SIZE >  4) sum += hc::__amdgcn_ds_swizzle(sum, 0x101f);
    if(SUBWAVE_SIZE >  2) sum += hc::__amdgcn_ds_swizzle(sum, 0x081f);
    if(SUBWAVE_SIZE >  1) sum += hc::__amdgcn_ds_swizzle(sum, 0x041f);
    // clang-format on

    return sum;
}

// Swizzle-based reduction
template <rocsparse_int SUBWAVE_SIZE>
__device__ double reduction(double sum)
{
    typedef union dbl_b32
    {
        double val;
        uint32_t b32[2];
    } dbl_b32_t;

    dbl_b32_t upper_sum;
    dbl_b32_t temp_sum;

    temp_sum.val = sum;

    if(SUBWAVE_SIZE > 32)
    {
        upper_sum.b32[0] = hc::__amdgcn_readlane(temp_sum.b32[0], 32);
        upper_sum.b32[1] = hc::__amdgcn_readlane(temp_sum.b32[1], 32);
        temp_sum.val += upper_sum.val;
    }

    if(SUBWAVE_SIZE > 16)
    {
        upper_sum.b32[0] = hc::__amdgcn_ds_swizzle(temp_sum.b32[0], 0x401f);
        upper_sum.b32[1] = hc::__amdgcn_ds_swizzle(temp_sum.b32[1], 0x401f);
        temp_sum.val += upper_sum.val;
    }

    if(SUBWAVE_SIZE > 8)
    {
        upper_sum.b32[0] = hc::__amdgcn_ds_swizzle(temp_sum.b32[0], 0x201f);
        upper_sum.b32[1] = hc::__amdgcn_ds_swizzle(temp_sum.b32[1], 0x201f);
        temp_sum.val += upper_sum.val;
    }

    if(SUBWAVE_SIZE > 4)
    {
        upper_sum.b32[0] = hc::__amdgcn_ds_swizzle(temp_sum.b32[0], 0x101f);
        upper_sum.b32[1] = hc::__amdgcn_ds_swizzle(temp_sum.b32[1], 0x101f);
        temp_sum.val += upper_sum.val;
    }

    if(SUBWAVE_SIZE > 2)
    {
        upper_sum.b32[0] = hc::__amdgcn_ds_swizzle(temp_sum.b32[0], 0x081f);
        upper_sum.b32[1] = hc::__amdgcn_ds_swizzle(temp_sum.b32[1], 0x081f);
        temp_sum.val += upper_sum.val;
    }

    if(SUBWAVE_SIZE > 1)
    {
        upper_sum.b32[0] = hc::__amdgcn_ds_swizzle(temp_sum.b32[0], 0x041f);
        upper_sum.b32[1] = hc::__amdgcn_ds_swizzle(temp_sum.b32[1], 0x041f);
        temp_sum.val += upper_sum.val;
    }

    sum = temp_sum.val;
    return sum;
}
#elif defined(__HIP_PLATFORM_NVCC__)
template <rocsparse_int SUBWAVE_SIZE, typename T>
__device__ T reduction(T sum)
{
    for(rocsparse_int i = SUBWAVE_SIZE >> 1; i > 0; i >>= 1)
    {
        sum += __shfl_down_sync(0xffffffff, sum, i);
    }

    return sum;
}
#endif

template <typename T, rocsparse_int SUBWAVE_SIZE>
static __device__ void csrmvn_general_device(rocsparse_int m,
                                             T alpha,
                                             const rocsparse_int* row_offset,
                                             const rocsparse_int* csr_col_ind,
                                             const T* csr_val,
                                             const T* x,
                                             T beta,
                                             T* y,
                                             rocsparse_index_base idx_base)
{
    rocsparse_int tid = hipThreadIdx_x;
    rocsparse_int gid = hipBlockIdx_x * hipBlockDim_x + tid;
    rocsparse_int lid = tid & (SUBWAVE_SIZE - 1);
    rocsparse_int nwarps = hipGridDim_x * hipBlockDim_x / SUBWAVE_SIZE;

    // Loop over rows each subwave processes
    for(rocsparse_int row = gid / SUBWAVE_SIZE; row < m; row += nwarps)
    {
        // Each subwave processes one row
        rocsparse_int row_start = row_offset[row] - idx_base;
        rocsparse_int row_end   = row_offset[row + 1] - idx_base;

        T sum = 0.0;

        // Loop over non-zero elements of subwave row
        for(rocsparse_int j = row_start + lid; j < row_end; j += SUBWAVE_SIZE)
        {
            sum = fma(alpha * csr_val[j], __ldg(x + csr_col_ind[j] - idx_base), sum);
        }

        // Obtain row sum using parallel reduction
        sum = reduction<SUBWAVE_SIZE>(sum);

        // First thread of each subwave writes result into global memory
        if(lid == 0)
        {
            if(beta == 0)
            {
                y[row] = sum;
            }
            else
            {
                y[row] = fma(beta, y[row], sum);
            }
        }
    }
}

#endif // CSRMV_DEVICE_H
