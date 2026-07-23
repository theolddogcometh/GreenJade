/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9797: dyn smoke soft gate (wave 9800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_9800(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 9800
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_9800  (alias)
 *   __libcgj_batch9797_marker = "libcgj-batch9797"
 *
 * Milestone 9800 exclusive continuum CREATE-ONLY (9791-9800). Unique
 * gj_dyn_soft_9800 surface only; no multi-def. Distinct from
 * gj_dyn_soft_9700 (batch9697), gj_dyn_soft_9600 (batch9597),
 * gj_dyn_soft_9500 (batch9497), and gj_smoke_soft_9800
 * (batch9796 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9797_marker[] = "libcgj-batch9797";

/* Dyn soft lamp for wave 9800 (always soft-ready). */
#define B9797_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9797_soft(void)
{
	return B9797_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_9800 - report dyn soft lamp for wave 9800.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch9796) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_9800(void)
{
	(void)NULL;
	return b9797_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_9800(void)
    __attribute__((alias("gj_dyn_soft_9800")));
