/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7147: shm_open flags validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shm_flags_ok_u_7147(uint32_t flags);
 *     - Return 1 if flags uses only known shm_open bits
 *       (O_RDWR|O_CREAT|O_EXCL = 0xC2), else 0.
 *   uint32_t __gj_shm_flags_ok_u_7147  (alias)
 *   __libcgj_batch7147_marker = "libcgj-batch7147"
 *
 * Exclusive continuum CREATE-ONLY (7141-7150: shm_open flag stubs —
 * o_rdwr_id, o_creat_id, o_excl_id, has_rdwr, has_creat, has_excl,
 * flags_ok, flags_pack, flags_errorish, batch_id_7150).
 * Unique gj_shm_flags_ok_u_7147 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7147_marker[] = "libcgj-batch7147";

/*
 * Known soft shm_open oflag mask:
 *   O_RDWR (0x2) | O_CREAT (0x40) | O_EXCL (0x80) = 0xC2
 * O_RDONLY is 0 and needs no bit. Soft catalog only.
 */
#define B7147_SHM_KNOWN_MASK ((uint32_t)0xC2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7147_shm_flags_ok(uint32_t u32Flags)
{
	return ((u32Flags & ~B7147_SHM_KNOWN_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shm_flags_ok_u_7147 - 1 if flags has no unknown bits.
 *
 * flags: shm_open(3) oflag argument
 *
 * Returns 1 when only bits in {0x2, 0x40, 0x80} are set (including 0),
 * else 0. Soft catalog check; does not call shm_open. No parent wires.
 */
uint32_t
gj_shm_flags_ok_u_7147(uint32_t u32Flags)
{
	(void)NULL;
	return b7147_shm_flags_ok(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shm_flags_ok_u_7147(uint32_t u32Flags)
    __attribute__((alias("gj_shm_flags_ok_u_7147")));
