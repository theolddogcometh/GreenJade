/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1708: Adler-32 prime modulus constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_adler32_mod(void);
 *     — Always returns 65521 (largest prime less than 2^16; the Adler-32
 *       BASE modulus used by zlib / ISO 3309).
 *   uint32_t __gj_adler32_mod  (alias)
 *   __libcgj_batch1708_marker = "libcgj-batch1708"
 *
 * Constant-only surface. Does NOT redefine adler32 / adler32_z /
 * gj_adler32_init / gj_adler32_update / gj_adler32_final.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1708_marker[] = "libcgj-batch1708";

/* Adler-32 BASE (largest prime < 65536). */
#define B1708_ADLER32_MOD 65521u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1708_mod(void)
{
	return B1708_ADLER32_MOD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_adler32_mod — Adler-32 prime modulus constant.
 *
 * Always returns 65521u.
 */
uint32_t
gj_adler32_mod(void)
{
	return b1708_mod();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_adler32_mod(void)
    __attribute__((alias("gj_adler32_mod")));
