/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7447: umask soft mask validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_umask_ok_u_7447(uint32_t mask);
 *     - Return 1 if mask uses only permission bits (0777), else 0.
 *   uint32_t __gj_umask_ok_u_7447  (alias)
 *   __libcgj_batch7447_marker = "libcgj-batch7447"
 *
 * Exclusive continuum CREATE-ONLY (7441-7450: umask soft mask stubs —
 * perm_mask_id, default_id, sbits_mask_id, full_mask_id, clamp_u,
 * apply_u, ok_u, blocks_u, pack_u, batch_id_7450).
 * Unique gj_umask_ok_u_7447 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7447_marker[] = "libcgj-batch7447";

/* Soft rwxrwxrwx permission mask (0777). */
#define B7447_PERM_MASK ((uint32_t)0x1FFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7447_ok(uint32_t u32Mask)
{
	return ((u32Mask & ~B7447_PERM_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_umask_ok_u_7447 - 1 if mask has no bits outside 0777.
 *
 * mask: soft umask word
 *
 * Returns 1 when only permission bits are set (including 0), else 0.
 * Soft catalog check; does not call umask. No parent wires.
 */
uint32_t
gj_umask_ok_u_7447(uint32_t u32Mask)
{
	(void)NULL;
	return b7447_ok(u32Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_umask_ok_u_7447(uint32_t u32Mask)
    __attribute__((alias("gj_umask_ok_u_7447")));
