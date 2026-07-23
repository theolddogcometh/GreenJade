/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5609: mesa version pack (compare / baseline).
 *
 * Surface (unique symbols):
 *   uint32_t gj_mesa_version_baseline_5609(void);
 *     - Returns packed 24.0.0 soft Deck mesa baseline (0x00180000).
 *   uint32_t gj_mesa_version_ge_5609(uint32_t a, uint32_t b);
 *     - Return 1 when packed a >= b, else 0.
 *   uint32_t gj_mesa_version_eq_5609(uint32_t a, uint32_t b);
 *     - Return 1 when packed a == b, else 0.
 *   uint32_t gj_mesa_version_ok_5609(uint32_t packed);
 *     - Return 1 when packed >= baseline, else 0.
 *   uint32_t __gj_mesa_version_baseline_5609  (alias)
 *   uint32_t __gj_mesa_version_ge_5609  (alias)
 *   uint32_t __gj_mesa_version_eq_5609  (alias)
 *   uint32_t __gj_mesa_version_ok_5609  (alias)
 *   __libcgj_batch5609_marker = "libcgj-batch5609"
 *
 * Exclusive continuum CREATE-ONLY (5601-5610). Mesa version compare;
 * distinct from gj_mesa_version_*_5608. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5609_marker[] = "libcgj-batch5609";

/* Soft baseline: major=24, minor=0, patch=0 -> (24<<16)|(0<<8)|0 */
#define B5609_BASELINE  0x00180000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5609_ge(uint32_t u32A, uint32_t u32B)
{
	return (u32A >= u32B) ? 1u : 0u;
}

static uint32_t
b5609_eq(uint32_t u32A, uint32_t u32B)
{
	return (u32A == u32B) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mesa_version_baseline_5609 - soft Deck mesa baseline pack.
 *
 * Always returns 0x00180000 (24.0.0). Product-path constant only.
 */
uint32_t
gj_mesa_version_baseline_5609(void)
{
	(void)NULL;
	return B5609_BASELINE;
}

/*
 * gj_mesa_version_ge_5609 - soft packed version greater-or-equal.
 *
 * a, b: packed mesa versions
 *
 * Returns 1 when a >= b, else 0.
 */
uint32_t
gj_mesa_version_ge_5609(uint32_t u32A, uint32_t u32B)
{
	return b5609_ge(u32A, u32B);
}

/*
 * gj_mesa_version_eq_5609 - soft packed version equality.
 *
 * Returns 1 when a == b, else 0.
 */
uint32_t
gj_mesa_version_eq_5609(uint32_t u32A, uint32_t u32B)
{
	return b5609_eq(u32A, u32B);
}

/*
 * gj_mesa_version_ok_5609 - soft check packed version meets baseline.
 *
 * packed: candidate packed mesa version
 *
 * Returns 1 when packed >= baseline (24.0.0), else 0.
 */
uint32_t
gj_mesa_version_ok_5609(uint32_t u32Packed)
{
	return b5609_ge(u32Packed, B5609_BASELINE);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_mesa_version_baseline_5609(void)
    __attribute__((alias("gj_mesa_version_baseline_5609")));

uint32_t __gj_mesa_version_ge_5609(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_mesa_version_ge_5609")));

uint32_t __gj_mesa_version_eq_5609(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_mesa_version_eq_5609")));

uint32_t __gj_mesa_version_ok_5609(uint32_t u32Packed)
    __attribute__((alias("gj_mesa_version_ok_5609")));
