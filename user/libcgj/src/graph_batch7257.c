/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7257: sendmsg flags validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_msg_flags_ok_u_7257(uint32_t flags);
 *     - Return 1 if flags uses only known soft MSG_* bits
 *       (0x40|0x4000|0x8000), else 0.
 *   uint32_t __gj_msg_flags_ok_u_7257  (alias)
 *   __libcgj_batch7257_marker = "libcgj-batch7257"
 *
 * Exclusive continuum CREATE-ONLY (7251-7260: sendmsg flag stubs —
 * dontwait_id, nosignal_id, more_id, has_dontwait, has_nosignal,
 * has_more, flags_ok, flags_pack, flags_errorish, batch_id_7260).
 * Unique gj_msg_flags_ok_u_7257 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7257_marker[] = "libcgj-batch7257";

/*
 * Known soft sendmsg flags mask:
 *   MSG_DONTWAIT (0x40) | MSG_NOSIGNAL (0x4000) | MSG_MORE (0x8000)
 *   = 0xC040
 */
#define B7257_MSG_KNOWN_MASK ((uint32_t)0xC040u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7257_msg_flags_ok(uint32_t u32Flags)
{
	return ((u32Flags & ~B7257_MSG_KNOWN_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_msg_flags_ok_u_7257 - 1 if flags has no unknown bits.
 *
 * flags: sendmsg(2) flags argument
 *
 * Returns 1 when only bits in {0x40, 0x4000, 0x8000} are set
 * (including 0), else 0. Soft catalog check; does not call sendmsg.
 * No parent wires.
 */
uint32_t
gj_msg_flags_ok_u_7257(uint32_t u32Flags)
{
	(void)NULL;
	return b7257_msg_flags_ok(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_msg_flags_ok_u_7257(uint32_t u32Flags)
    __attribute__((alias("gj_msg_flags_ok_u_7257")));
