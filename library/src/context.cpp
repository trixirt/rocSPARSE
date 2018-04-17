/* ************************************************************************
 * Copyright 2018 Advanced Micro Devices, Inc.
 * ************************************************************************ */

#include "context.h"
#include "rocsparse.h"

extern "C" rocsparseStatus_t rocsparseCreate(rocsparseHandle_t *handle)
{
    // Check if handle is valid
    if (handle == nullptr)
    {
        return ROCSPARSE_STATUS_INVALID_POINTER;
    }
    else
    {
        return ROCSPARSE_STATUS_SUCCESS;
    }
}

extern "C" rocsparseStatus_t rocsparseDestroy(rocsparseHandle_t handle)
{
    return ROCSPARSE_STATUS_SUCCESS;
}
