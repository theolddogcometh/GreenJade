/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7149: shm_open flags errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shm_flags_errorish_u_7149(uint32_t flags);
 *     - Return 1 if flags has any unknown bits outside known shm_open
 *       mask (0xC2), else 0.
 *   uint32_t __gj_shm_flags_errorish_u_7149  (alias)
 *   __libcgj_batch7149_marker = "libcgj-batch7149"
 *
 * Exclusive continuum CREATE-ONLY (7141-7150: shm_open flag stubs —
 * o_rdwr_id, o_creat_id, o_excl_id, has_rdwr, has_creat, has_excl,
 * flags_ok, flags_pack, flags_errorish, batch_id_7150).
 * Unique gj_shm_flags_errorish_u_7149 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7149_marker[] = "libcgj-batch7149";

/*
 * Known soft shm_open oflag mask:
 *   O_RDWR | O_CREAT | O_EXCL = 0xC2
 */
#define B7149_SHM_KNOWN_MASK ((uint32_t)0xC2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7149_shm_flags_errorish(uint32_t u32Flags)
{
	return ((u32Flags & ~B7149_SHM_KNOWN_MASK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shm_flags_errorish_u_7149 - 1 if flags has unknown bits.
 *
 * flags: shm_open(3) oflag argument
 *
 * Returns 1 when any bit outside {0x2, 0x40, 0x80} is set, else 0.
 * Soft inverse of flags_ok; does not call shm_open. No parent wires.
 */
uint32_t
gj_shm_flags_errorish_u_7149(uint32_t u32Flags)
{
	(void)NULL;
	return b7149_shm_flags_errorish(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shm_flags_errorish_u_7149(uint32_t u32Flags)
    __attribute__((alias("gj_shm_flags_errorish_u_7149")));
