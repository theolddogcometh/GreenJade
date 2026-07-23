/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7157: sem_open oflag validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sem_flags_ok_u_7157(uint32_t flags);
 *     - Return 1 if oflag uses only O_CREAT|O_EXCL and O_EXCL implies
 *       O_CREAT; else 0.
 *   uint32_t __gj_sem_flags_ok_u_7157  (alias)
 *   __libcgj_batch7157_marker = "libcgj-batch7157"
 *
 * Exclusive continuum CREATE-ONLY (7151-7160: sem_open flag stubs —
 * creat_id, excl_id, known_mask_id, has_creat, has_excl, needs_mode,
 * flags_ok, flags_pack, flags_errorish, batch_id_7160).
 * Unique gj_sem_flags_ok_u_7157 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7157_marker[] = "libcgj-batch7157";

/*
 * Known sem_open oflag mask:
 *   O_CREAT (0x40) | O_EXCL (0x80) = 0xC0
 * POSIX: O_EXCL is only meaningful with O_CREAT.
 */
#define B7157_O_CREAT              ((uint32_t)0x40u)
#define B7157_O_EXCL               ((uint32_t)0x80u)
#define B7157_SEM_OFLAG_KNOWN_MASK ((uint32_t)0xC0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7157_sem_flags_ok(uint32_t u32Flags)
{
	if ((u32Flags & ~B7157_SEM_OFLAG_KNOWN_MASK) != 0u)
		return 0u;
	if ((u32Flags & B7157_O_EXCL) != 0u &&
	    (u32Flags & B7157_O_CREAT) == 0u)
		return 0u;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sem_flags_ok_u_7157 - 1 if oflag is a valid soft sem_open mask.
 *
 * flags: sem_open(3) oflag argument
 *
 * Returns 1 when only bits in {0x40, 0x80} are set and O_EXCL is not
 * alone without O_CREAT (including oflag 0). Soft catalog check; does
 * not call sem_open. No parent wires.
 */
uint32_t
gj_sem_flags_ok_u_7157(uint32_t u32Flags)
{
	(void)NULL;
	return b7157_sem_flags_ok(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sem_flags_ok_u_7157(uint32_t u32Flags)
    __attribute__((alias("gj_sem_flags_ok_u_7157")));
