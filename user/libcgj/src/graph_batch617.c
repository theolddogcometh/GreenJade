/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch617: Classic Fletcher-16 surface audit gate.
 *
 * Surface (requested; already wired — NOT redefined here to avoid multi-def):
 *   uint16_t gj_fletcher16(const void *data, size_t n);
 *     — Classic Fletcher-16 over n bytes (8-bit blocks, modulus 255).
 *         sum1 = (sum1 + byte) mod 255
 *         sum2 = (sum2 + sum1) mod 255
 *       Result = (sum2 << 8) | sum1. Empty/NULL → 0.
 *       KAT: "\x01" → 0x0101; "abc" → 0x4c27.
 *   uint16_t __gj_fletcher16  (alias)
 *   __libcgj_batch617_marker = "libcgj-batch617"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   gj_fletcher16 / __gj_fletcher16  → graph_batch183.c
 *   gj_fletcher32 / __gj_fletcher32  → graph_batch183.c
 *   gj_fletcher32_fold / __gj_fletcher32_fold → graph_batch453.c
 *
 * Freestanding pure C marker TU (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Dyn/smoke gates for gj_fletcher16
 * remain owned by batch183.
 */

#include <stddef.h>

const char __libcgj_batch617_marker[] = "libcgj-batch617";
