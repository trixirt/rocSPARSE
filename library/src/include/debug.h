/*! \file */
/* ************************************************************************
 * Copyright (C) 2023 Advanced Micro Devices, Inc. All rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * ************************************************************************ */

#pragma once

#include "envariables.h"
#include "status.h"

///
/// @brief Structure to store debug global variables.
///
struct rocsparse_debug_variables_st
{
private:
    bool debug         = ROCSPARSE_ENVARIABLES.get(rocsparse_envariables::DEBUG);
    bool debug_verbose = ROCSPARSE_ENVARIABLES.get(rocsparse_envariables::DEBUG_VERBOSE);
    bool debug_arguments_verbose
        = ROCSPARSE_ENVARIABLES.get(rocsparse_envariables::DEBUG_VERBOSE)
          || ROCSPARSE_ENVARIABLES.get(rocsparse_envariables::DEBUG_ARGUMENTS_VERBOSE);
    bool debug_arguments = ROCSPARSE_ENVARIABLES.get(rocsparse_envariables::DEBUG)
                           || ROCSPARSE_ENVARIABLES.get(rocsparse_envariables::DEBUG_ARGUMENTS);

public:
    bool get_debug() const;
    bool get_debug_verbose() const;
    bool get_debug_arguments() const;
    bool get_debug_arguments_verbose() const;

    void set_debug(bool value);
    void set_debug_verbose(bool value);
    void set_debug_arguments(bool value);
    void set_debug_arguments_verbose(bool value);
};

struct rocsparse_debug_st
{
private:
    rocsparse_debug_variables_st m_var;

public:
    static rocsparse_debug_st& instance()
    {
        static rocsparse_debug_st self;
        return self;
    }

    static rocsparse_debug_variables_st& var()
    {
        return instance().m_var;
    }

    ~rocsparse_debug_st() = default;

private:
    rocsparse_debug_st() = default;
};

#define rocsparse_debug_variables rocsparse_debug_st::instance().var()
