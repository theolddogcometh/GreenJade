/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4972: PE IMAGE_NT_SIGNATURE constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pe_nt_sig_u(void);
 *     - Return the PE signature dword IMAGE_NT_SIGNATURE 0x00004550
 *       ("PE\0\0" little-endian).
 *   uint32_t __gj_pe_nt_sig_u  (alias)
 *   __libcgj_batch4972_marker = "libcgj-batch4972"
 *
 * Exclusive continuum CREATE-ONLY PE unique wave (4971-4980). Distinct
 * from gj_pe_is_pe / gj_pe_is_pe_u (sniff, not constant) — unique
 * gj_pe_nt_sig_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4972_marker[] = "libcgj-batch4972";

/* IMAGE_NT_SIGNATURE: 'P' | ('E' << 8) | (0 << 16) | (0 << 24). */
#define B4972_NT_SIG  0x00004550u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4972_nt_sig(void)
{
	return B4972_NT_SIG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pe_nt_sig_u - report IMAGE_NT_SIGNATURE (0x00004550).
 *
 * Always returns 0x00004550 ("PE\0\0"). Soft compile-time PE product
 * constant. Does not parse an image. No parent wires.
 */
uint32_t
gj_pe_nt_sig_u(void)
{
	(void)NULL;
	return b4972_nt_sig();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pe_nt_sig_u(void)
    __attribute__((alias("gj_pe_nt_sig_u")));
