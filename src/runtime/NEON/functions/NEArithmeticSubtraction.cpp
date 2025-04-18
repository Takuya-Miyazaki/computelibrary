/*
 * Copyright (c) 2017-2021, 2024 Arm Limited.
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
#include "arm_compute/runtime/NEON/functions/NEArithmeticSubtraction.h"

#include "arm_compute/core/ITensor.h"
#include "arm_compute/core/Validate.h"

#include "src/cpu/operators/CpuSub.h"

#include <utility>

namespace arm_compute
{
struct NEArithmeticSubtraction::Impl
{
    const ITensor               *src_0{nullptr};
    const ITensor               *src_1{nullptr};
    ITensor                     *dst{nullptr};
    std::unique_ptr<cpu::CpuSub> op{nullptr};
};

NEArithmeticSubtraction::NEArithmeticSubtraction() : _impl(std::make_unique<Impl>())
{
}
NEArithmeticSubtraction::NEArithmeticSubtraction(NEArithmeticSubtraction &&)            = default;
NEArithmeticSubtraction &NEArithmeticSubtraction::operator=(NEArithmeticSubtraction &&) = default;
NEArithmeticSubtraction::~NEArithmeticSubtraction()                                     = default;

Status NEArithmeticSubtraction::validate(const ITensorInfo         *input1,
                                         const ITensorInfo         *input2,
                                         const ITensorInfo         *output,
                                         ConvertPolicy              policy,
                                         const ActivationLayerInfo &act_info)
{
    ARM_COMPUTE_RETURN_ERROR_ON_DYNAMIC_SHAPE(input1, input2, output);
    return cpu::CpuSub::validate(input1, input2, output, policy, act_info);
}

void NEArithmeticSubtraction::configure(const ITensor             *input1,
                                        const ITensor             *input2,
                                        ITensor                   *output,
                                        ConvertPolicy              policy,
                                        const ActivationLayerInfo &act_info)
{
    _impl->src_0 = input1;
    _impl->src_1 = input2;
    _impl->dst   = output;
    _impl->op    = std::make_unique<cpu::CpuSub>();
    _impl->op->configure(input1->info(), input2->info(), output->info(), policy, act_info);
}

void NEArithmeticSubtraction::run()
{
    ITensorPack pack;
    pack.add_tensor(TensorType::ACL_SRC_0, _impl->src_0);
    pack.add_tensor(TensorType::ACL_SRC_1, _impl->src_1);
    pack.add_tensor(TensorType::ACL_DST, _impl->dst);
    _impl->op->run(pack);
}
} // namespace arm_compute
