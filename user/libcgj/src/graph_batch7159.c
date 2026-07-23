/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7159: sem_open oflag errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sem_flags_errorish_u_7159(uint32_t flags);
 *     - Return 1 if oflag has unknown bits or O_EXCL without O_CREAT,
 *       else 0.
 *   uint32_t __gj_sem_flags_errorish_u_7159  (alias)
 *   __libcgj_batch7159_marker = "libcgj-batch7159"
 *
 * Exclusive continuum CREATE-ONLY (7151-7160: sem_open flag stubs —
 * creat_id, excl_id, known_mask_id, has_creat, has_excl, needs_mode,
 * flags_ok, flags_pack, flags_errorish, batch_id_7160).
 * Unique gj_sem_flags_errorish_u_7159 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7159_marker[] = "libcgj-batch7159";

/*
 * Known sem_open oflag mask:
 *   O_CREAT | O_EXCL = 0xC0
 * Soft policy: O_EXCL alone is errorish (not paired with O_CREAT).
 */
#define B7159_O_CREAT              ((uint32_t)0x40u)
#define B7159_O_EXCL               ((uint32_t)0x80u)
#define B7159_SEM_OFLAG_KNOWN_MASK ((uint32_t)0xC0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7159_sem_flags_errorish(uint32_t u32Flags)
{
	if ((u32Flags & ~B7159_SEM_OFLAG_KNOWN_MASK) != 0u)
		return 1u;
	if ((u32Flags & B7159_O_EXCL) != 0u &&
	    (u32Flags & B7159_O_CREAT) == 0u)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sem_flags_errorish_u_7159 - 1 if oflag looks invalid for sem_open.
 *
 * flags: sem_open(3) oflag argument
 *
 * Returns 1 when any bit outside {0x40, 0x80} is set, or when O_EXCL
 * is set without O_CREAT; else 0. Soft inverse of flags_ok; does not
 * call sem_open. No parent wires.
 */
uint32_t
gj_sem_flags_errorish_u_7159(uint32_t u32Flags)
{
	(void)NULL;
	return b7159_sem_flags_errorish(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sem_flags_errorish_u_7159(uint32_t u32Flags)
    __attribute__((alias("gj_sem_flags_errorish_u_7159")));
