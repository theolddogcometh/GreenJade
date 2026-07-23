/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7255: sendmsg MSG_NOSIGNAL presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_msg_has_nosignal_u_7255(uint32_t flags);
 *     - Return 1 if flags has MSG_NOSIGNAL (0x4000) set, else 0.
 *   uint32_t __gj_msg_has_nosignal_u_7255  (alias)
 *   __libcgj_batch7255_marker = "libcgj-batch7255"
 *
 * Exclusive continuum CREATE-ONLY (7251-7260: sendmsg flag stubs —
 * dontwait_id, nosignal_id, more_id, has_dontwait, has_nosignal,
 * has_more, flags_ok, flags_pack, flags_errorish, batch_id_7260).
 * Unique gj_msg_has_nosignal_u_7255 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7255_marker[] = "libcgj-batch7255";

/* MSG_NOSIGNAL bit (0x4000). */
#define B7255_MSG_NOSIGNAL ((uint32_t)0x4000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7255_msg_has_nosignal(uint32_t u32Flags)
{
	return ((u32Flags & B7255_MSG_NOSIGNAL) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_msg_has_nosignal_u_7255 - 1 if flags includes MSG_NOSIGNAL.
 *
 * flags: sendmsg(2) flags argument
 *
 * Returns 1 when bit 0x4000 is set, else 0. Soft bit test only; does
 * not call sendmsg. No parent wires.
 */
uint32_t
gj_msg_has_nosignal_u_7255(uint32_t u32Flags)
{
	(void)NULL;
	return b7255_msg_has_nosignal(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_msg_has_nosignal_u_7255(uint32_t u32Flags)
    __attribute__((alias("gj_msg_has_nosignal_u_7255")));
