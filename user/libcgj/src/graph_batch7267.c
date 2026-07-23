/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7267: recvmsg flags validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_msg_flags_ok_u_7267(uint32_t flags);
 *     - Return 1 if flags uses only known MSG_* bits
 *       (0x2|0x40|0x100), else 0.
 *   uint32_t __gj_msg_flags_ok_u_7267  (alias)
 *   __libcgj_batch7267_marker = "libcgj-batch7267"
 *
 * Exclusive continuum CREATE-ONLY (7261-7270: recvmsg flag stubs —
 * peek_id, waitall_id, dontwait_id, has_peek, has_waitall, has_dontwait,
 * flags_ok, flags_pack, flags_errorish, batch_id_7270).
 * Unique gj_msg_flags_ok_u_7267 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7267_marker[] = "libcgj-batch7267";

/*
 * Known recvmsg flag bits for this continuum:
 *   MSG_PEEK (0x2) | MSG_DONTWAIT (0x40) | MSG_WAITALL (0x100) = 0x142
 */
#define B7267_MSG_KNOWN_MASK ((uint32_t)0x142u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7267_msg_flags_ok(uint32_t u32Flags)
{
	return ((u32Flags & ~B7267_MSG_KNOWN_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_msg_flags_ok_u_7267 - 1 if flags has no unknown bits.
 *
 * flags: recvmsg(2) / recv(2) flags argument
 *
 * Returns 1 when only bits in {0x2, 0x40, 0x100} are set (including 0),
 * else 0. Soft catalog check; does not call recvmsg. No parent wires.
 */
uint32_t
gj_msg_flags_ok_u_7267(uint32_t u32Flags)
{
	(void)NULL;
	return b7267_msg_flags_ok(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_msg_flags_ok_u_7267(uint32_t u32Flags)
    __attribute__((alias("gj_msg_flags_ok_u_7267")));
