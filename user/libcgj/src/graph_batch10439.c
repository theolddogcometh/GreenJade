/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10439: IM soft ready aggregate tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_im_soft_ready_u_10439(void);
 *     - Returns 1 (soft compile-time product tag: IM soft continuum
 *       wave 10431-10440 is present/ready; not a runtime hard probe of
 *       any input-method stack).
 *   uint32_t __gj_im_soft_ready_u_10439  (alias)
 *   __libcgj_batch10439_marker = "libcgj-batch10439"
 *
 * Exclusive continuum CREATE-ONLY (10431-10440: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→10440). Unique
 * gj_im_soft_ready_u_10439 surface only; no multi-def. Distinct from
 * per-slot ok_u soft stubs (10431-10438, all→0) and gj_batch_id_10440.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10439_marker[] = "libcgj-batch10439";

/* Soft IM continuum ready lamp (wave present). */
#define B10439_IM_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10439_im_soft_ready(void)
{
	return B10439_IM_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_im_soft_ready_u_10439 - soft IM continuum ready tag.
 *
 * Always returns 1 (IM soft wave ready/present). Does not call libc or
 * hard-probe any IM slot. No parent wires.
 */
uint32_t
gj_im_soft_ready_u_10439(void)
{
	(void)NULL;
	return b10439_im_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_im_soft_ready_u_10439(void)
    __attribute__((alias("gj_im_soft_ready_u_10439")));
