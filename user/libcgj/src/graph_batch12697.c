/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12697: dyn smoke soft gate (wave 12700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_12700(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 12700
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_12700  (alias)
 *   __libcgj_batch12697_marker = "libcgj-batch12697"
 *
 * Milestone 12700 exclusive continuum CREATE-ONLY (12691-12700). Unique
 * gj_dyn_soft_12700 surface only; no multi-def. Distinct from
 * gj_dyn_soft_12600 (batch12597), gj_dyn_soft_12500 (batch12497),
 * gj_dyn_soft_12400 (batch12397), and gj_smoke_soft_12700
 * (batch12696 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12697_marker[] = "libcgj-batch12697";

/* Dyn soft lamp for wave 12700 (always soft-ready). */
#define B12697_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12697_soft(void)
{
	return B12697_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_12700 - report dyn soft lamp for wave 12700.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch12696) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_12700(void)
{
	(void)NULL;
	return b12697_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_12700(void)
    __attribute__((alias("gj_dyn_soft_12700")));
