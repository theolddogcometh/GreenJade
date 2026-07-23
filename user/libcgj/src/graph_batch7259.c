/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7259: sendmsg flags errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_msg_flags_errorish_u_7259(uint32_t flags);
 *     - Return 1 if flags has any unknown bits outside known soft
 *       MSG_* mask (0xC040), else 0.
 *   uint32_t __gj_msg_flags_errorish_u_7259  (alias)
 *   __libcgj_batch7259_marker = "libcgj-batch7259"
 *
 * Exclusive continuum CREATE-ONLY (7251-7260: sendmsg flag stubs —
 * dontwait_id, nosignal_id, more_id, has_dontwait, has_nosignal,
 * has_more, flags_ok, flags_pack, flags_errorish, batch_id_7260).
 * Unique gj_msg_flags_errorish_u_7259 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7259_marker[] = "libcgj-batch7259";

/*
 * Known soft sendmsg flags mask:
 *   MSG_DONTWAIT | MSG_NOSIGNAL | MSG_MORE = 0xC040
 */
#define B7259_MSG_KNOWN_MASK ((uint32_t)0xC040u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7259_msg_flags_errorish(uint32_t u32Flags)
{
	return ((u32Flags & ~B7259_MSG_KNOWN_MASK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_msg_flags_errorish_u_7259 - 1 if flags has unknown bits.
 *
 * flags: sendmsg(2) flags argument
 *
 * Returns 1 when any bit outside {0x40, 0x4000, 0x8000} is set, else 0.
 * Soft inverse of flags_ok; does not call sendmsg. No parent wires.
 */
uint32_t
gj_msg_flags_errorish_u_7259(uint32_t u32Flags)
{
	(void)NULL;
	return b7259_msg_flags_errorish(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_msg_flags_errorish_u_7259(uint32_t u32Flags)
    __attribute__((alias("gj_msg_flags_errorish_u_7259")));
