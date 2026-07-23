/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7444: umask soft full mode-bits mask id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_umask_full_mask_id_7444(void);
 *     - Return soft full mode mask (07777 / 0xFFF).
 *   uint32_t __gj_umask_full_mask_id_7444  (alias)
 *   __libcgj_batch7444_marker = "libcgj-batch7444"
 *
 * Exclusive continuum CREATE-ONLY (7441-7450: umask soft mask stubs —
 * perm_mask_id, default_id, sbits_mask_id, full_mask_id, clamp_u,
 * apply_u, ok_u, blocks_u, pack_u, batch_id_7450).
 * Unique gj_umask_full_mask_id_7444 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7444_marker[] = "libcgj-batch7444";

/* Soft full mode mask: permission + special bits (07777). */
#define B7444_FULL_MASK ((uint32_t)0xFFFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7444_full_mask_id(void)
{
	return B7444_FULL_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_umask_full_mask_id_7444 - soft full file-mode mask constant.
 *
 * Always returns 0xFFF (07777). Catalog id only; does not call umask.
 * No parent wires.
 */
uint32_t
gj_umask_full_mask_id_7444(void)
{
	(void)NULL;
	return b7444_full_mask_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_umask_full_mask_id_7444(void)
    __attribute__((alias("gj_umask_full_mask_id_7444")));
