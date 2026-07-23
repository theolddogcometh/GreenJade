/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7445: umask soft clamp-to-perm-bits stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_umask_clamp_u_7445(uint32_t mask);
 *     - Return mask & 0777 (soft permission clamp).
 *   uint32_t __gj_umask_clamp_u_7445  (alias)
 *   __libcgj_batch7445_marker = "libcgj-batch7445"
 *
 * Exclusive continuum CREATE-ONLY (7441-7450: umask soft mask stubs —
 * perm_mask_id, default_id, sbits_mask_id, full_mask_id, clamp_u,
 * apply_u, ok_u, blocks_u, pack_u, batch_id_7450).
 * Unique gj_umask_clamp_u_7445 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7445_marker[] = "libcgj-batch7445";

/* Soft rwxrwxrwx permission mask (0777). */
#define B7445_PERM_MASK ((uint32_t)0x1FFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7445_clamp(uint32_t u32Mask)
{
	return u32Mask & B7445_PERM_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_umask_clamp_u_7445 - soft-clamp umask to permission bits.
 *
 * mask: soft umask word
 *
 * Returns mask & 0x1FF (0777). Pure integer; does not call umask.
 * No parent wires.
 */
uint32_t
gj_umask_clamp_u_7445(uint32_t u32Mask)
{
	(void)NULL;
	return b7445_clamp(u32Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_umask_clamp_u_7445(uint32_t u32Mask)
    __attribute__((alias("gj_umask_clamp_u_7445")));
