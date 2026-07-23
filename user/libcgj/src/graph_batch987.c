/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch987: cache-line size constant query.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cacheline_size(void);
 *     — Return the assumed CPU cache-line size in bytes (64).
 *   uint32_t __gj_cacheline_size  (alias)
 *   __libcgj_batch987_marker = "libcgj-batch987"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch987_marker[] = "libcgj-batch987";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cacheline_size — report assumed cache-line width.
 *
 * Always returns 64 (x86_64 / common modern default).
 */
uint32_t
gj_cacheline_size(void)
{
	return 64u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cacheline_size(void)
    __attribute__((alias("gj_cacheline_size")));
