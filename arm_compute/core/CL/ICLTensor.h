/*
 * Copyright (c) 2016-2019, 2023, 2025 Arm Limited.
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
#ifndef ACL_ARM_COMPUTE_CORE_CL_ICLTENSOR_H
#define ACL_ARM_COMPUTE_CORE_CL_ICLTENSOR_H

/** @file
 * @publicapi
 */

#include "arm_compute/core/CL/CLTypes.h"
#include "arm_compute/core/ITensor.h"

#include <cstdint>

namespace cl
{
class Buffer;
class CommandQueue;
} // namespace cl

namespace arm_compute
{
/** Interface for OpenCL tensor */
class ICLTensor : public ITensor
{
public:
    /** Default constructor. */
    ICLTensor();
    /** Prevent instances of this class from being copy constructed */
    ICLTensor(const ICLTensor &) = delete;
    /** Prevent instances of this class from being copied */
    ICLTensor &operator=(const ICLTensor &) = delete;
    /** Allow instances of this class to be move constructed */
    ICLTensor(ICLTensor &&) = default;
    /** Allow instances of this class to be copied */
    ICLTensor &operator=(ICLTensor &&) = default;
    /** Default virtual destructor. */
    virtual ~ICLTensor() = default;

    /** Interface to be implemented by the child class to return the wrapped quantization info data
     *
     * @return A wrapped quantization info object.
     */
    virtual CLQuantization quantization() const = 0;
    /** Interface to be implemented by the child class to return a reference to the OpenCL buffer containing the image's data.
     *
     * @return A reference to an OpenCL buffer containing the image's data.
     */
    virtual const cl::Buffer &cl_buffer() const = 0;
    /** Enqueue a map operation of the allocated buffer on the given queue.
     *
     * @param[in,out] q        The CL command queue to use for the mapping operation.
     * @param[in]     blocking If true, then the mapping will be ready to use by the time
     *                         this method returns, else it is the caller's responsibility
     *                         to flush the queue and wait for the mapping operation to have completed before using the returned mapping pointer.
     */
    void map(cl::CommandQueue &q, bool blocking = true);
    /** Enqueue an unmap operation of the allocated and mapped buffer on the given queue.
     *
     * @note This method simply enqueues the unmap operation, it is the caller's responsibility to flush the queue and make sure the unmap is finished before
     *       the memory is accessed by the device.
     *
     * @param[in,out] q The CL command queue to use for the mapping operation.
     */
    void unmap(cl::CommandQueue &q);
    /** Clear the contents of the tensor synchronously.
     *
     * @param[in,out] q The CL command queue to use for the clear operation.
     */
    void clear(cl::CommandQueue &q);

    // Inherited methods overridden:
    uint8_t *buffer() const override;

protected:
    /** Method to be implemented by the child class to map the OpenCL buffer
     *
     * @param[in,out] q        The CL command queue to use for the mapping operation.
     * @param[in]     blocking If true, then the mapping will be ready to use by the time
     *                         this method returns, else it is the caller's responsibility
     *                         to flush the queue and wait for the mapping operation to have completed before using the returned mapping pointer.
     */
    virtual uint8_t *do_map(cl::CommandQueue &q, bool blocking) = 0;
    /** Method to be implemented by the child class to unmap the OpenCL buffer
     *
     * @note This method simply enqueues the unmap operation, it is the caller's responsibility to flush the queue and make sure the unmap is finished before
     *       the memory is accessed by the device.
     *
     * @param[in,out] q The CL command queue to use for the mapping operation.
     */
    virtual void do_unmap(cl::CommandQueue &q) = 0;

private:
    uint8_t *_mapping;
};

using ICLImage = ICLTensor;
} // namespace arm_compute
#endif // ACL_ARM_COMPUTE_CORE_CL_ICLTENSOR_H
