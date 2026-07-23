/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7441: umask soft permission-bit mask id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_umask_perm_mask_id_7441(void);
 *     - Return soft permission-bits mask (0777 / 0x1FF).
 *   uint32_t __gj_umask_perm_mask_id_7441  (alias)
 *   __libcgj_batch7441_marker = "libcgj-batch7441"
 *
 * Exclusive continuum CREATE-ONLY (7441-7450: umask soft mask stubs —
 * perm_mask_id, default_id, sbits_mask_id, full_mask_id, clamp_u,
 * apply_u, ok_u, blocks_u, pack_u, batch_id_7450).
 * Unique gj_umask_perm_mask_id_7441 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7441_marker[] = "libcgj-batch7441";

/* Soft rwxrwxrwx permission mask (0777). */
#define B7441_PERM_MASK ((uint32_t)0x1FFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7441_perm_mask_id(void)
{
	return B7441_PERM_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_umask_perm_mask_id_7441 - soft umask permission-bits mask constant.
 *
 * Always returns 0x1FF (0777). Catalog id only; does not call umask.
 * No parent wires.
 */
uint32_t
gj_umask_perm_mask_id_7441(void)
{
	(void)NULL;
	return b7441_perm_mask_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_umask_perm_mask_id_7441(void)
    __attribute__((alias("gj_umask_perm_mask_id_7441")));
