/*
 * Copyright (c) 2019-2020, 2024-2025 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef ACL_ARM_COMPUTE_RUNTIME_CPP_FUNCTIONS_CPPTOPKV_H
#define ACL_ARM_COMPUTE_RUNTIME_CPP_FUNCTIONS_CPPTOPKV_H

/** @file
 * @publicapi
 */

#include "arm_compute/core/Types.h"
#include "arm_compute/runtime/CPP/ICPPSimpleFunction.h"

namespace arm_compute
{
class ITensor;

/** Basic function to run CPPTopKVKernel */
class CPPTopKV : public ICPPSimpleFunction
{
public:
    /** Set the input and output of the kernel.
     *
     * @param[in]  predictions A batch_size x classes tensor. Data types supported: F16/S32/F32/QASYMM8/QASYMM8_SIGNED
     * @param[in]  targets     A batch_size 1D tensor of class ids. Data types supported: U32
     * @param[out] output      Computed precision at @p k as a bool 1D tensor. Data types supported: U8
     * @param[in]  k           Number of top elements to look at for computing precision.
     */
    void configure(const ITensor *predictions, const ITensor *targets, ITensor *output, const unsigned int k);

    /** Static function to check if given info will lead to a valid configuration of @ref CPPTopKVKernel
     *
     * @param[in] predictions A batch_size x classes tensor info. Data types supported: F16/S32/F32/QASYMM8/QASYMM8_SIGNED
     * @param[in] targets     A batch_size 1D tensor info of class ids. Data types supported: U32
     * @param[in] output      Computed precision at @p k as a bool 1D tensor info. Data types supported: U8
     * @param[in] k           Number of top elements to look at for computing precision.
     *
     * @return a status
     */
    static Status
    validate(const ITensorInfo *predictions, const ITensorInfo *targets, ITensorInfo *output, const unsigned int k);
};
} // namespace arm_compute
#endif // ACL_ARM_COMPUTE_RUNTIME_CPP_FUNCTIONS_CPPTOPKV_H
