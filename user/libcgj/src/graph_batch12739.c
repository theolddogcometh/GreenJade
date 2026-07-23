/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12739: IM soft ready aggregate tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_im_soft_ready_u_12739(void);
 *     - Returns 1 (soft compile-time product tag: IM soft continuum
 *       wave 12731-12740 is present/ready; not a runtime hard probe of
 *       any input-method stack).
 *   uint32_t __gj_im_soft_ready_u_12739  (alias)
 *   __libcgj_batch12739_marker = "libcgj-batch12739"
 *
 * Exclusive continuum CREATE-ONLY (12731-12740: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→12740). Unique
 * gj_im_soft_ready_u_12739 surface only; no multi-def. Distinct from
 * per-slot ok_u soft stubs (12731-12738, all→0) and gj_batch_id_12740.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12739_marker[] = "libcgj-batch12739";

/* Soft IM continuum ready lamp (wave present). */
#define B12739_IM_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12739_im_soft_ready(void)
{
	return B12739_IM_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_im_soft_ready_u_12739 - soft IM continuum ready tag.
 *
 * Always returns 1 (IM soft wave ready/present). Does not call libc or
 * hard-probe any IM slot. No parent wires.
 */
uint32_t
gj_im_soft_ready_u_12739(void)
{
	(void)NULL;
	return b12739_im_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_im_soft_ready_u_12739(void)
    __attribute__((alias("gj_im_soft_ready_u_12739")));
