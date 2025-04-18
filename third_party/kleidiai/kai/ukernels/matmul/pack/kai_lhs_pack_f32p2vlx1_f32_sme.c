//
// SPDX-FileCopyrightText: Copyright 2024 Arm Limited and/or its affiliates <open-source-office@arm.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#if !defined(__aarch64__) || !defined(__ARM_FEATURE_SVE2)
#error This file must be compiled for AArch64, FEAT_SVE2.
#else  // Architectural features check.

#include <stddef.h>
#include <stdint.h>

#include "kai/kai_common.h"

static const size_t kai_mr = 2;
static const size_t kai_kr = 1;
static const size_t kai_sr = 1;

size_t kai_get_m_step_lhs_pack_f32p2vlx1_f32_sme(size_t mr) {
    KAI_ASSUME(mr == kai_mr * kai_get_sme_vector_length_u32());
    KAI_UNUSED(mr);

    return kai_mr * kai_get_sme_vector_length_u32();
}

size_t kai_get_lhs_offset_lhs_pack_f32p2vlx1_f32_sme(size_t m_idx, size_t lhs_stride) {
    KAI_ASSUME(m_idx % (kai_mr * kai_get_sme_vector_length_u32()) == 0);

    return m_idx * lhs_stride;
}

size_t kai_get_lhs_packed_offset_lhs_pack_f32p2vlx1_f32_sme(size_t m_idx, size_t k, size_t mr, size_t kr, size_t sr) {
    const size_t scaled_mr = kai_mr * kai_get_sme_vector_length_u32();
    KAI_ASSUME(m_idx % scaled_mr == 0);
    KAI_ASSUME(mr == scaled_mr);
    KAI_ASSUME(kr == kai_kr);
    KAI_ASSUME(sr == kai_sr);

    KAI_UNUSED(mr);
    KAI_UNUSED(kr);
    KAI_UNUSED(sr);

    return m_idx * k * sizeof(float);
}

size_t kai_get_lhs_packed_size_lhs_pack_f32p2vlx1_f32_sme(size_t m, size_t k, size_t mr, size_t kr, size_t sr) {
    KAI_ASSUME(mr == kai_mr * kai_get_sme_vector_length_u32());
    KAI_ASSUME(kr == kai_kr);
    KAI_ASSUME(sr == kai_sr);

    KAI_UNUSED(mr);
    KAI_UNUSED(kr);
    KAI_UNUSED(sr);

    return kai_roundup(m, kai_mr * kai_get_sme_vector_length_u32()) * k * sizeof(float);
}

void kai_run_lhs_pack_f32p2vlx1_f32_sme(
    size_t m, size_t k, size_t mr, size_t kr, size_t sr, size_t m_idx_start, const void* lhs, size_t lhs_stride,
    void* lhs_packed) {
    KAI_ASSUME(mr == kai_mr * kai_get_sme_vector_length_u32());
    KAI_ASSUME(kr == kai_kr);
    KAI_ASSUME(sr == kai_sr);
    KAI_ASSUME(lhs != NULL);
    KAI_ASSUME(lhs_packed != NULL);

    KAI_ASSUME(m_idx_start == 0);

    const size_t block_height = kai_mr * kai_get_sme_vector_length_u32();
    const size_t width = k;
    const size_t row_offset = 0;

    const void* in[block_height];

    uint8_t* lhs_packed_ptr = lhs_packed;
    const uint8_t* lhs_ptr = lhs;
    for (size_t block_y = 0; block_y < m; block_y += block_height) {
        const size_t height = KAI_MIN(m - block_y, block_height);
        void* out = lhs_packed_ptr + block_y * k * sizeof(float);

        for (size_t y = 0; y < height; y++) {
            in[y] = lhs_ptr + (block_y + y) * lhs_stride;
        }

        __asm__ __volatile__(
            ".inst 0xd503477f  // SMSTART ZA\n"
            "mov x21, %x[width]\n"
            "mov x20, %x[width]\n"
            "incw x21\n"
            "cntw x17\n"
            "sub x21, x21, #0x1\n"
            "sub x16, x17, #0x1\n"
            "udiv x21, x21, x17\n"  // n_passes = ceildiv(width, VL<T>)
            "ands x16, x20, x16\n"
            "sub x20, x21, #0x1\n"
            "sub x15, x17, #0x2\n"
            "mov x14, #0x0\n"
            "mov x11, %x[in]\n"
            "add x10, %x[in], x17, LSL #3\n"
            "cntw x9, ALL, MUL #2\n"
            "ldr x28, [x11, #0x0]\n"
            "cntw x27, ALL, MUL #3\n"
            "lsr x20, x20, #0x1\n"  // n_loops = (n_passes - 1) / 2
            "ldr x26, [x10, #0x0]\n"
            "and x25, x21, #0x1\n"  // odd_tail = bool(n_passes & 0x1)
            "csel x16, x16, x17, NE\n"
            "ldr x24, [x11, #0x8]\n"
            "ptrue p12.s\n"
            "whilelt p11.s, XZR, %x[height]\n"
            "ldr x21, [x10, #0x8]\n"
            "whilelt p10.s, x17, %x[height]\n"
            "mov x23, %x[row_offset]\n"
            "mov x22, %x[out]\n"
            "whilelt p9.s, x14, %x[width]\n"
            "whilelt p8.s, x14, %x[width]\n"
            "add x11, x11, #0x10\n"
            "add x10, x10, #0x10\n"
            "mov x12, #0x0\n"
            "cbz x15, 2f\n"
            "1:"  // K loop: Charge: Loop
            ".inst 0x25306163  // psel p3.s, p8.s/Z, p11.s[w12]\n"
            ".inst 0x25306142  // psel p2.s, p8.s/Z, p10.s[w12]\n"
            ".inst 0x25706161  // psel p1.s, p8.s/Z, p11.s[w12, #1]\n"
            ".inst 0x25706140  // psel p0.s, p8.s/Z, p10.s[w12, #1]\n"
            ".inst 0xe0970f80  // ld1w { za0h.s[x12] }, p3/Z, [x28, x23, LSL #2]\n"
            "ldr x28, [x11, #0x0]\n"
            ".inst 0xe0970b44  // ld1w { za1h.s[x12] }, p2/Z, [x26, x23, LSL #2]\n"
            "ldr x26, [x10, #0x0]\n"
            ".inst 0xe0970701  // ld1w { za0h.s[x12, #1] }, p1/Z, [x24, x23, LSL #2]\n"
            "ldr x24, [x11, #0x8]\n"
            "add x11, x11, #0x10\n"
            ".inst 0xe09702a5  // ld1w { za1h.s[x12, #1] }, p0/Z, [x21, x23, LSL #2]\n"
            "add x12, x12, #0x2\n"
            "ldr x21, [x10, #0x8]\n"
            "add x10, x10, #0x10\n"
            "cmp x12, x15\n"
            "blt 1b\n"
            "2:"  // K loop: Charge: End
            ".inst 0x25306163  // psel p3.s, p8.s/Z, p11.s[w12]\n"
            ".inst 0x25306142  // psel p2.s, p8.s/Z, p10.s[w12]\n"
            ".inst 0x25706161  // psel p1.s, p8.s/Z, p11.s[w12, #1]\n"
            ".inst 0x25706140  // psel p0.s, p8.s/Z, p10.s[w12, #1]\n"
            "mov x11, %x[in]\n"
            "add x10, %x[in], x17, LSL #3\n"
            ".inst 0xe0970f80  // ld1w { za0h.s[x12] }, p3/Z, [x28, x23, LSL #2]\n"
            "ldr x28, [x11, #0x0]\n"
            "incw x14\n"
            ".inst 0xe0970b44  // ld1w { za1h.s[x12] }, p2/Z, [x26, x23, LSL #2]\n"
            "ldr x26, [x10, #0x0]\n"
            ".inst 0xe0970701  // ld1w { za0h.s[x12, #1] }, p1/Z, [x24, x23, LSL #2]\n"
            "ldr x24, [x11, #0x8]\n"
            "add x11, x11, #0x10\n"
            ".inst 0xe09702a5  // ld1w { za1h.s[x12, #1] }, p0/Z, [x21, x23, LSL #2]\n"
            "ldr x21, [x10, #0x8]\n"
            "add x10, x10, #0x10\n"
            "incw x23\n"
            "cbz x20, 8f\n"
            "mov x20, x20\n"
            "3:"  // K loop: Main loop
            "whilelt p8.s, x14, %x[width]\n"
            "mov x13, #0x0\n"
            "cbz x15, 5f\n"
            "4:"  // K loop: Main loop: First: Loop
            ".inst 0x25316160  // psel p0.s, p8.s/Z, p11.s[w13]\n"
            ".inst 0x25316142  // psel p2.s, p8.s/Z, p10.s[w13]\n"
            ".inst 0x25716161  // psel p1.s, p8.s/Z, p11.s[w13, #1]\n"
            ".inst 0x25716143  // psel p3.s, p8.s/Z, p10.s[w13, #1]\n"
            ".inst 0xe0972388  // ld1w { za2h.s[x13] }, p0/Z, [x28, x23, LSL #2]\n"
            ".inst 0x25317120  // psel p0.s, p12.s/Z, p9.s[w13]\n"
            "ldr x28, [x11, #0x0]\n"
            ".inst 0xe0972b4c  // ld1w { za3h.s[x13] }, p2/Z, [x26, x23, LSL #2]\n"
            ".inst 0x25317122  // psel p2.s, p12.s/Z, p9.s[w13]\n"
            "ldr x26, [x10, #0x0]\n"
            ".inst 0xe0972709  // ld1w { za2h.s[x13, #1] }, p1/Z, [x24, x23, LSL #2]\n"
            ".inst 0x25717121  // psel p1.s, p12.s/Z, p9.s[w13, #1]\n"
            "ldr x24, [x11, #0x8]\n"
            "add x11, x11, #0x10\n"
            ".inst 0xe0972ead  // ld1w { za3h.s[x13, #1] }, p3/Z, [x21, x23, LSL #2]\n"
            "ldr x21, [x10, #0x8]\n"
            ".inst 0xe0bfa2c0  // st1w { za0v.s[x13] }, p0/Z, [x22, XZR, LSL #2]\n"
            ".inst 0x25717120  // psel p0.s, p12.s/Z, p9.s[w13, #1]\n"
            ".inst 0xe0b1aac4  // st1w { za1v.s[x13] }, p2/Z, [x22, x17, LSL #2]\n"
            "add x10, x10, #0x10\n"
            ".inst 0xe0a9a6c1  // st1w { za0v.s[x13, #1] }, p1/Z, [x22, x9, LSL #2]\n"
            ".inst 0xe0bba2c5  // st1w { za1v.s[x13, #1] }, p0/Z, [x22, x27, LSL #2]\n"
            "add x13, x13, #0x2\n"
            "addvl x22, x22, #4\n"
            "cmp x13, x15\n"
            "blt 4b\n"
            "5:"  // K loop: Main loop: First: Tail
            ".inst 0x25316160  // psel p0.s, p8.s/Z, p11.s[w13]\n"
            ".inst 0x25316142  // psel p2.s, p8.s/Z, p10.s[w13]\n"
            ".inst 0x25716161  // psel p1.s, p8.s/Z, p11.s[w13, #1]\n"
            ".inst 0x25716143  // psel p3.s, p8.s/Z, p10.s[w13, #1]\n"
            "mov x11, %x[in]\n"
            "add x10, %x[in], x17, LSL #3\n"
            ".inst 0xe0972388  // ld1w { za2h.s[x13] }, p0/Z, [x28, x23, LSL #2]\n"
            ".inst 0x25317120  // psel p0.s, p12.s/Z, p9.s[w13]\n"
            "ldr x28, [x11, #0x0]\n"
            "mov x12, #0x0\n"
            ".inst 0xe0972b4c  // ld1w { za3h.s[x13] }, p2/Z, [x26, x23, LSL #2]\n"
            ".inst 0x25317122  // psel p2.s, p12.s/Z, p9.s[w13]\n"
            "ldr x26, [x10, #0x0]\n"
            ".inst 0xe0972709  // ld1w { za2h.s[x13, #1] }, p1/Z, [x24, x23, LSL #2]\n"
            ".inst 0x25717121  // psel p1.s, p12.s/Z, p9.s[w13, #1]\n"
            "ldr x24, [x11, #0x8]\n"
            "add x11, x11, #0x10\n"
            ".inst 0xe0972ead  // ld1w { za3h.s[x13, #1] }, p3/Z, [x21, x23, LSL #2]\n"
            "ldr x21, [x10, #0x8]\n"
            ".inst 0xe0bfa2c0  // st1w { za0v.s[x13] }, p0/Z, [x22, XZR, LSL #2]\n"
            ".inst 0x25717120  // psel p0.s, p12.s/Z, p9.s[w13, #1]\n"
            ".inst 0xe0b1aac4  // st1w { za1v.s[x13] }, p2/Z, [x22, x17, LSL #2]\n"
            "whilelt p9.s, x14, %x[width]\n"
            "incw x14\n"
            ".inst 0xe0a9a6c1  // st1w { za0v.s[x13, #1] }, p1/Z, [x22, x9, LSL #2]\n"
            "add x10, x10, #0x10\n"
            "incw x23\n"
            ".inst 0xe0bba2c5  // st1w { za1v.s[x13, #1] }, p0/Z, [x22, x27, LSL #2]\n"
            "addvl x22, x22, #4\n"
            "whilelt p8.s, x14, %x[width]\n"
            "cbz x15, 7f\n"
            "6:"  // K loop: Main loop: Second: Loop
            ".inst 0x25306160  // psel p0.s, p8.s/Z, p11.s[w12]\n"
            ".inst 0x25306142  // psel p2.s, p8.s/Z, p10.s[w12]\n"
            ".inst 0x25706161  // psel p1.s, p8.s/Z, p11.s[w12, #1]\n"
            ".inst 0x25706143  // psel p3.s, p8.s/Z, p10.s[w12, #1]\n"
            ".inst 0xe0970380  // ld1w { za0h.s[x12] }, p0/Z, [x28, x23, LSL #2]\n"
            ".inst 0x25307120  // psel p0.s, p12.s/Z, p9.s[w12]\n"
            "ldr x28, [x11, #0x0]\n"
            ".inst 0xe0970b44  // ld1w { za1h.s[x12] }, p2/Z, [x26, x23, LSL #2]\n"
            ".inst 0x25307122  // psel p2.s, p12.s/Z, p9.s[w12]\n"
            "ldr x26, [x10, #0x0]\n"
            ".inst 0xe0970701  // ld1w { za0h.s[x12, #1] }, p1/Z, [x24, x23, LSL #2]\n"
            ".inst 0x25707121  // psel p1.s, p12.s/Z, p9.s[w12, #1]\n"
            "ldr x24, [x11, #0x8]\n"
            "add x11, x11, #0x10\n"
            ".inst 0xe0970ea5  // ld1w { za1h.s[x12, #1] }, p3/Z, [x21, x23, LSL #2]\n"
            "ldr x21, [x10, #0x8]\n"
            ".inst 0xe0bf82c8  // st1w { za2v.s[x12] }, p0/Z, [x22, XZR, LSL #2]\n"
            ".inst 0x25707120  // psel p0.s, p12.s/Z, p9.s[w12, #1]\n"
            ".inst 0xe0b18acc  // st1w { za3v.s[x12] }, p2/Z, [x22, x17, LSL #2]\n"
            "add x10, x10, #0x10\n"
            ".inst 0xe0a986c9  // st1w { za2v.s[x12, #1] }, p1/Z, [x22, x9, LSL #2]\n"
            ".inst 0xe0bb82cd  // st1w { za3v.s[x12, #1] }, p0/Z, [x22, x27, LSL #2]\n"
            "add x12, x12, #0x2\n"
            "addvl x22, x22, #4\n"
            "cmp x12, x15\n"
            "blt 6b\n"
            "7:"  // K loop: Main loop: Second: Tail
            ".inst 0x25306160  // psel p0.s, p8.s/Z, p11.s[w12]\n"
            ".inst 0x25306142  // psel p2.s, p8.s/Z, p10.s[w12]\n"
            ".inst 0x25706161  // psel p1.s, p8.s/Z, p11.s[w12, #1]\n"
            ".inst 0x25706143  // psel p3.s, p8.s/Z, p10.s[w12, #1]\n"
            "mov x11, %x[in]\n"
            "add x10, %x[in], x17, LSL #3\n"
            ".inst 0xe0970380  // ld1w { za0h.s[x12] }, p0/Z, [x28, x23, LSL #2]\n"
            ".inst 0x25307120  // psel p0.s, p12.s/Z, p9.s[w12]\n"
            "ldr x28, [x11, #0x0]\n"
            ".inst 0xe0970b44  // ld1w { za1h.s[x12] }, p2/Z, [x26, x23, LSL #2]\n"
            ".inst 0x25307122  // psel p2.s, p12.s/Z, p9.s[w12]\n"
            "ldr x26, [x10, #0x0]\n"
            ".inst 0xe0970701  // ld1w { za0h.s[x12, #1] }, p1/Z, [x24, x23, LSL #2]\n"
            ".inst 0x25707121  // psel p1.s, p12.s/Z, p9.s[w12, #1]\n"
            "ldr x24, [x11, #0x8]\n"
            "add x11, x11, #0x10\n"
            ".inst 0xe0970ea5  // ld1w { za1h.s[x12, #1] }, p3/Z, [x21, x23, LSL #2]\n"
            "ldr x21, [x10, #0x8]\n"
            ".inst 0xe0bf82c8  // st1w { za2v.s[x12] }, p0/Z, [x22, XZR, LSL #2]\n"
            ".inst 0x25707120  // psel p0.s, p12.s/Z, p9.s[w12, #1]\n"
            ".inst 0xe0b18acc  // st1w { za3v.s[x12] }, p2/Z, [x22, x17, LSL #2]\n"
            "whilelt p9.s, x14, %x[width]\n"
            "subs x20, x20, #0x1\n"
            ".inst 0xe0a986c9  // st1w { za2v.s[x12, #1] }, p1/Z, [x22, x9, LSL #2]\n"
            "add x10, x10, #0x10\n"
            "incw x14\n"
            ".inst 0xe0bb82cd  // st1w { za3v.s[x12, #1] }, p0/Z, [x22, x27, LSL #2]\n"
            "addvl x22, x22, #4\n"
            "incw x23\n"
            "bgt 3b\n"
            "8:"  // K loop: Tails
            "cbnz x25, 11f\n"
            "mov x11, %x[in]\n"
            "whilelt p8.s, x14, %x[width]\n"
            "mov x12, #0x0\n"
            "9:"  // K loop: Tails: Even: First
            ".inst 0x25307123  // psel p3.s, p12.s/Z, p9.s[w12]\n"
            ".inst 0x25307122  // psel p2.s, p12.s/Z, p9.s[w12]\n"
            ".inst 0x25306161  // psel p1.s, p8.s/Z, p11.s[w12]\n"
            ".inst 0x25306140  // psel p0.s, p8.s/Z, p10.s[w12]\n"
            ".inst 0xe0bf8ec0  // st1w { za0v.s[x12] }, p3/Z, [x22, XZR, LSL #2]\n"
            ".inst 0xe0b18ac4  // st1w { za1v.s[x12] }, p2/Z, [x22, x17, LSL #2]\n"
            "addvl x22, x22, #2\n"
            "ldr x21, [x11, #0x0]\n"
            "ldr x20, [x11, x17, LSL #0x3]\n"
            "add x11, x11, #0x8\n"
            ".inst 0xe09706a8  // ld1w { za2h.s[x12] }, p1/Z, [x21, x23, LSL #2]\n"
            ".inst 0xe097028c  // ld1w { za3h.s[x12] }, p0/Z, [x20, x23, LSL #2]\n"
            "add x12, x12, #0x1\n"
            "cmp x12, x17\n"
            "blt 9b\n"
            "whilelt p9.s, x14, %x[width]\n"
            "whilelt p8.s, x14, %x[width]\n"
            "mov x12, #0x0\n"
            "10:"  // K loop: Tails: Even: Second
            ".inst 0x25307121  // psel p1.s, p12.s/Z, p9.s[w12]\n"
            ".inst 0x25307120  // psel p0.s, p12.s/Z, p9.s[w12]\n"
            ".inst 0xe0bf86c8  // st1w { za2v.s[x12] }, p1/Z, [x22, XZR, LSL #2]\n"
            ".inst 0xe0b182cc  // st1w { za3v.s[x12] }, p0/Z, [x22, x17, LSL #2]\n"
            "add x12, x12, #0x1\n"
            "addvl x22, x22, #2\n"
            "cmp x12, x16\n"
            "blt 10b\n"
            "whilelt p8.s, x14, %x[width]\n"
            "b 13f\n"
            "11:"  // K loop: Tails: Odd
            "mov x12, #0x0\n"
            "12:"  // K loop: Tails: Odd: Loop
            ".inst 0x25307121  // psel p1.s, p12.s/Z, p9.s[w12]\n"
            ".inst 0x25307120  // psel p0.s, p12.s/Z, p9.s[w12]\n"
            ".inst 0xe0bf86c0  // st1w { za0v.s[x12] }, p1/Z, [x22, XZR, LSL #2]\n"
            ".inst 0xe0b182c4  // st1w { za1v.s[x12] }, p0/Z, [x22, x17, LSL #2]\n"
            "add x12, x12, #0x1\n"
            "addvl x22, x22, #2\n"
            "cmp x12, x16\n"
            "blt 12b\n"
            "13:"  // K loop: End
            "mov %x[out], x22\n"
            ".inst 0xd503467f  // SMSTOP\n"
            : [out] "+&r"(out)
            : [height] "r"(height), [in] "r"(in), [row_offset] "r"(row_offset), [width] "r"(width)
            : "cc", "memory", "p0", "p1", "p2", "p3", "p4", "p5", "p6", "p7", "p8", "p9", "p10", "p11", "p12", "p13",
              "p14", "p15", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x20", "x21", "x22", "x23",
              "x24", "x25", "x26", "x27", "x28", "z0", "z1", "z2", "z3", "z4", "z5", "z6", "z7", "z8", "z9", "z10",
              "z11", "z12", "z13", "z14", "z15", "z16", "z17", "z18", "z19", "z20", "z21", "z22", "z23", "z24", "z25",
              "z26", "z27", "z28", "z29", "z30", "z31");
    }
}

#endif  // Architectural features check.
