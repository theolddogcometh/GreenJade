/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7137: mq_open flags validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mq_flags_ok_u_7137(uint32_t flags);
 *     - Return 1 if flags uses only known mq oflag bits
 *       (O_CREAT|O_EXCL|O_NONBLOCK = 0x8C0), else 0.
 *   uint32_t __gj_mq_flags_ok_u_7137  (alias)
 *   __libcgj_batch7137_marker = "libcgj-batch7137"
 *
 * Exclusive continuum CREATE-ONLY (7131-7140: mq_open flag stubs —
 * creat_id, excl_id, nonblock_id, has_creat, has_excl, has_nonblock,
 * flags_ok, flags_pack, flags_errorish, batch_id_7140).
 * Unique gj_mq_flags_ok_u_7137 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7137_marker[] = "libcgj-batch7137";

/*
 * Known mq_open oflag modifier mask (access mode bits catalogued
 * separately elsewhere):
 *   O_CREAT (0x40) | O_EXCL (0x80) | O_NONBLOCK (0x800) = 0x8C0
 */
#define B7137_MQ_KNOWN_MASK ((uint32_t)0x8C0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7137_mq_flags_ok(uint32_t u32Flags)
{
	return ((u32Flags & ~B7137_MQ_KNOWN_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mq_flags_ok_u_7137 - 1 if flags has no unknown bits.
 *
 * flags: mq_open(3) oflag argument (modifier bits only in this soft
 * catalog; access-mode O_RDONLY/O_WRONLY/O_RDWR are not in the mask)
 *
 * Returns 1 when only bits in {0x40, 0x80, 0x800} are set (including 0),
 * else 0. Soft catalog check; does not call mq_open. No parent wires.
 */
uint32_t
gj_mq_flags_ok_u_7137(uint32_t u32Flags)
{
	(void)NULL;
	return b7137_mq_flags_ok(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mq_flags_ok_u_7137(uint32_t u32Flags)
    __attribute__((alias("gj_mq_flags_ok_u_7137")));
