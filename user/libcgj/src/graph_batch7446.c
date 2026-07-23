/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7446: umask soft apply-to-mode stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_umask_apply_u_7446(uint32_t mode, uint32_t mask);
 *     - Return (mode & 0777) & ~(mask & 0777).
 *   uint32_t __gj_umask_apply_u_7446  (alias)
 *   __libcgj_batch7446_marker = "libcgj-batch7446"
 *
 * Exclusive continuum CREATE-ONLY (7441-7450: umask soft mask stubs —
 * perm_mask_id, default_id, sbits_mask_id, full_mask_id, clamp_u,
 * apply_u, ok_u, blocks_u, pack_u, batch_id_7450).
 * Unique gj_umask_apply_u_7446 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7446_marker[] = "libcgj-batch7446";

/* Soft rwxrwxrwx permission mask (0777). */
#define B7446_PERM_MASK ((uint32_t)0x1FFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7446_apply(uint32_t u32Mode, uint32_t u32Mask)
{
	uint32_t u32M;
	uint32_t u32U;

	u32M = u32Mode & B7446_PERM_MASK;
	u32U = u32Mask & B7446_PERM_MASK;
	return u32M & ~u32U;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_umask_apply_u_7446 - soft-apply umask to a mode word.
 *
 * mode: soft requested permission bits
 * mask: soft umask word
 *
 * Returns (mode & 0777) & ~(mask & 0777). Pure integer apply; does not
 * call umask or chmod. No parent wires.
 */
uint32_t
gj_umask_apply_u_7446(uint32_t u32Mode, uint32_t u32Mask)
{
	(void)NULL;
	return b7446_apply(u32Mode, u32Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_umask_apply_u_7446(uint32_t u32Mode, uint32_t u32Mask)
    __attribute__((alias("gj_umask_apply_u_7446")));
