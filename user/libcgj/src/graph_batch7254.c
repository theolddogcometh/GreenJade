/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7254: sendmsg MSG_DONTWAIT presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_msg_has_dontwait_u_7254(uint32_t flags);
 *     - Return 1 if flags has MSG_DONTWAIT (0x40) set, else 0.
 *   uint32_t __gj_msg_has_dontwait_u_7254  (alias)
 *   __libcgj_batch7254_marker = "libcgj-batch7254"
 *
 * Exclusive continuum CREATE-ONLY (7251-7260: sendmsg flag stubs —
 * dontwait_id, nosignal_id, more_id, has_dontwait, has_nosignal,
 * has_more, flags_ok, flags_pack, flags_errorish, batch_id_7260).
 * Unique gj_msg_has_dontwait_u_7254 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7254_marker[] = "libcgj-batch7254";

/* MSG_DONTWAIT bit (0x40). */
#define B7254_MSG_DONTWAIT ((uint32_t)0x40u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7254_msg_has_dontwait(uint32_t u32Flags)
{
	return ((u32Flags & B7254_MSG_DONTWAIT) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_msg_has_dontwait_u_7254 - 1 if flags includes MSG_DONTWAIT.
 *
 * flags: sendmsg(2) flags argument
 *
 * Returns 1 when bit 0x40 is set, else 0. Soft bit test only; does not
 * call sendmsg. No parent wires.
 */
uint32_t
gj_msg_has_dontwait_u_7254(uint32_t u32Flags)
{
	(void)NULL;
	return b7254_msg_has_dontwait(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_msg_has_dontwait_u_7254(uint32_t u32Flags)
    __attribute__((alias("gj_msg_has_dontwait_u_7254")));
