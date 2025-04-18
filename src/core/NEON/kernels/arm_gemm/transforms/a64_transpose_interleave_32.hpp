/*
 * Copyright (c) 2024-2025 Arm Limited.
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

#pragma once

#if defined(__aarch64__)

namespace {

void a64_transpose_interleave_32(uint16_t *out, const uint16_t *in, size_t width, size_t in_stride, size_t height)
{
    size_t out_stride = 16 * height * sizeof(uint16_t);

    __asm__ __volatile__(
      "cmp %x[height], #0x4\n"
      "blt 9f\n"
      "1:"  // Main row loop: Head
      "mov x25, %x[in]\n"
      "mov x24, %x[width]\n"
      "mov x23, %x[out]\n"
      "sub %x[height], %x[height], #0x4\n"
      "add x22, x25, %x[in_stride]\n"
      "add x21, x22, %x[in_stride]\n"
      "add x20, x21, %x[in_stride]\n"
      "cmp x24, #0x10\n"
      "add %x[in], x20, %x[in_stride]\n"
      "blt 3f\n"
      "2:"  // Main row loop: Column loop
      "ldr q23, [x25], #0x10\n"
      "ldr q22, [x22], #0x10\n"
      "sub x24, x24, #0x10\n"
      "ldr q21, [x21], #0x10\n"
      "ldr q20, [x20], #0x10\n"
      "cmp x24, #0x10\n"
      "ldr q19, [x25], #0x10\n"
      "ldr q18, [x22], #0x10\n"
      "ldr q17, [x21], #0x10\n"
      "ldr q16, [x20], #0x10\n"
      "str q23, [x23, #0x0]\n"
      "str q19, [x23, #0x10]\n"
      "str q22, [x23, #0x20]\n"
      "str q18, [x23, #0x30]\n"
      "str q21, [x23, #0x40]\n"
      "str q17, [x23, #0x50]\n"
      "str q20, [x23, #0x60]\n"
      "str q16, [x23, #0x70]\n"
      "add x23, x23, %x[out_stride]\n"
      "bge 2b\n"
      "3:"  // Main row loop: Column loop skip
      "cbz x24, 8f\n"
      "cmp x24, #0x4\n"
      "movi v16.8h, #0x0\n"
      "str q16, [x23, #0x0]\n"
      "str q16, [x23, #0x10]\n"
      "str q16, [x23, #0x20]\n"
      "str q16, [x23, #0x30]\n"
      "str q16, [x23, #0x40]\n"
      "str q16, [x23, #0x50]\n"
      "str q16, [x23, #0x60]\n"
      "str q16, [x23, #0x70]\n"
      "blt 5f\n"
      "4:"  // Main row loop: width 4 loop: loop
      "ldr d19, [x25], #0x8\n"
      "ldr d18, [x22], #0x8\n"
      "sub x24, x24, #0x4\n"
      "ldr d17, [x21], #0x8\n"
      "ldr d16, [x20], #0x8\n"
      "cmp x24, #0x4\n"
      "str d19, [x23, #0x0]\n"
      "str d18, [x23, #0x20]\n"
      "str d17, [x23, #0x40]\n"
      "str d16, [x23, #0x60]\n"
      "add x23, x23, #0x8\n"
      "bge 4b\n"
      "5:"  // Main row loop: width 4 loop: skip
      "cmp x24, #0x1\n"
      "blt 7f\n"
      "6:"  // Main row loop: width 1 loop: loop
      "ldr h19, [x25], #0x2\n"
      "ldr h18, [x22], #0x2\n"
      "sub x24, x24, #0x1\n"
      "ldr h17, [x21], #0x2\n"
      "ldr h16, [x20], #0x2\n"
      "cmp x24, #0x1\n"
      "str h19, [x23, #0x0]\n"
      "str h18, [x23, #0x20]\n"
      "str h17, [x23, #0x40]\n"
      "str h16, [x23, #0x60]\n"
      "add x23, x23, #0x2\n"
      "bge 6b\n"
      "7:"  // Main row loop: width 1 loop: skip
      "8:"  // Main row loop: odd col skip
      "cmp %x[height], #0x4\n"
      "add %x[out], %x[out], #0x80\n"
      "bge 1b\n"
      "cbz %x[height], 18f\n"
      "9:"  // Main loop skip
      "10:"  // Tail row loop: Head
      "mov x20, %x[width]\n"
      "mov x25, %x[in]\n"
      "mov x23, %x[out]\n"
      "sub %x[height], %x[height], #0x1\n"
      "cmp x20, #0x10\n"
      "add %x[in], x25, %x[in_stride]\n"
      "blt 12f\n"
      "11:"  // Tail row loop: Column loop
      "ldr q17, [x25], #0x10\n"
      "sub x20, x20, #0x10\n"
      "ldr q16, [x25], #0x10\n"
      "cmp x20, #0x10\n"
      "str q17, [x23, #0x0]\n"
      "str q16, [x23, #0x10]\n"
      "add x23, x23, %x[out_stride]\n"
      "bge 11b\n"
      "12:"  // Tail row loop: Column loop skip
      "cbz x20, 17f\n"
      "cmp x20, #0x4\n"
      "movi v16.8h, #0x0\n"
      "str q16, [x23, #0x0]\n"
      "str q16, [x23, #0x10]\n"
      "blt 14f\n"
      "13:"  // Tail row loop: width 4 loop: loop
      "ldr d16, [x25], #0x8\n"
      "sub x20, x20, #0x4\n"
      "cmp x20, #0x4\n"
      "str d16, [x23, #0x0]\n"
      "add x23, x23, #0x8\n"
      "bge 13b\n"
      "14:"  // Tail row loop: width 4 loop: skip
      "cmp x20, #0x1\n"
      "blt 16f\n"
      "15:"  // Tail row loop: width 1 loop: loop
      "ldr h16, [x25], #0x2\n"
      "sub x20, x20, #0x1\n"
      "cmp x20, #0x1\n"
      "str h16, [x23, #0x0]\n"
      "add x23, x23, #0x2\n"
      "bge 15b\n"
      "16:"  // Tail row loop: width 1 loop: skip
      "17:"  // Tail row loop: odd col skip
      "cmp %x[height], #0x1\n"
      "add %x[out], %x[out], #0x20\n"
      "bge 10b\n"
      "18:"  // Done
      : [height] "+&r" (height), [in] "+&r" (in), [out] "+&r" (out)
      : [in_stride] "r" (in_stride), [out_stride] "r" (out_stride), [width] "r" (width)
      : "cc", "memory", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "x20", "x21", "x22", "x23", "x24", "x25"
    );
}

} // anonymous namespace

template<>
void Transform<16, 1, true, VLType::None>(
    __fp16 *out, const __fp16 *in, int stride, int x0, int xmax, int k0, int kmax)
{
    a64_transpose_interleave_32(
        reinterpret_cast<uint16_t *>(out),
        reinterpret_cast<const uint16_t *>(in + k0 * stride + x0),
        (xmax-x0) * sizeof(__fp16) / 2,
        stride * sizeof(__fp16),
        (kmax-k0)
    );
}


#endif  // defined(__aarch64__)
