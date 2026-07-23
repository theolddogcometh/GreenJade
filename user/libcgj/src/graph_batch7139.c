/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7139: mq_open flags errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mq_flags_errorish_u_7139(uint32_t flags);
 *     - Return 1 if flags has any unknown bits outside known mq oflag
 *       mask (0x8C0), else 0.
 *   uint32_t __gj_mq_flags_errorish_u_7139  (alias)
 *   __libcgj_batch7139_marker = "libcgj-batch7139"
 *
 * Exclusive continuum CREATE-ONLY (7131-7140: mq_open flag stubs —
 * creat_id, excl_id, nonblock_id, has_creat, has_excl, has_nonblock,
 * flags_ok, flags_pack, flags_errorish, batch_id_7140).
 * Unique gj_mq_flags_errorish_u_7139 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7139_marker[] = "libcgj-batch7139";

/*
 * Known mq_open oflag modifier mask:
 *   O_CREAT | O_EXCL | O_NONBLOCK = 0x8C0
 */
#define B7139_MQ_KNOWN_MASK ((uint32_t)0x8C0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7139_mq_flags_errorish(uint32_t u32Flags)
{
	return ((u32Flags & ~B7139_MQ_KNOWN_MASK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mq_flags_errorish_u_7139 - 1 if flags has unknown bits.
 *
 * flags: mq_open(3) oflag argument (modifier-bit soft catalog)
 *
 * Returns 1 when any bit outside {0x40, 0x80, 0x800} is set, else 0.
 * Soft inverse of flags_ok; does not call mq_open. No parent wires.
 */
uint32_t
gj_mq_flags_errorish_u_7139(uint32_t u32Flags)
{
	(void)NULL;
	return b7139_mq_flags_errorish(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mq_flags_errorish_u_7139(uint32_t u32Flags)
    __attribute__((alias("gj_mq_flags_errorish_u_7139")));
