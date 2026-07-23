/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7449: umask soft pack-from-class-denies stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_umask_pack_u_7449(uint32_t deny_u,
 *                                 uint32_t deny_g,
 *                                 uint32_t deny_o);
 *     - Pack soft user/group/other deny nibbles (0..7) into a umask.
 *   uint32_t __gj_umask_pack_u_7449  (alias)
 *   __libcgj_batch7449_marker = "libcgj-batch7449"
 *
 * Exclusive continuum CREATE-ONLY (7441-7450: umask soft mask stubs —
 * perm_mask_id, default_id, sbits_mask_id, full_mask_id, clamp_u,
 * apply_u, ok_u, blocks_u, pack_u, batch_id_7450).
 * Unique gj_umask_pack_u_7449 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7449_marker[] = "libcgj-batch7449";

/* Soft 3-bit rwx nibble. */
#define B7449_RWX_NIB ((uint32_t)0x7u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7449_pack(uint32_t u32DenyU, uint32_t u32DenyG, uint32_t u32DenyO)
{
	uint32_t u32Pack;

	u32Pack = (u32DenyU & B7449_RWX_NIB) << 6;
	u32Pack |= (u32DenyG & B7449_RWX_NIB) << 3;
	u32Pack |= (u32DenyO & B7449_RWX_NIB);
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_umask_pack_u_7449 - soft-pack class deny nibbles into a umask.
 *
 * deny_u: user class deny bits (low 3: rwx), others ignored
 * deny_g: group class deny bits (low 3: rwx), others ignored
 * deny_o: other class deny bits (low 3: rwx), others ignored
 *
 * Returns ((deny_u&7)<<6) | ((deny_g&7)<<3) | (deny_o&7). Pure integer
 * pack; does not call umask. No parent wires.
 */
uint32_t
gj_umask_pack_u_7449(uint32_t u32DenyU, uint32_t u32DenyG, uint32_t u32DenyO)
{
	(void)NULL;
	return b7449_pack(u32DenyU, u32DenyG, u32DenyO);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_umask_pack_u_7449(uint32_t u32DenyU,
                                uint32_t u32DenyG,
                                uint32_t u32DenyO)
    __attribute__((alias("gj_umask_pack_u_7449")));
