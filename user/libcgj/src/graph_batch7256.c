/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7256: sendmsg MSG_MORE presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_msg_has_more_u_7256(uint32_t flags);
 *     - Return 1 if flags has MSG_MORE (0x8000) set, else 0.
 *   uint32_t __gj_msg_has_more_u_7256  (alias)
 *   __libcgj_batch7256_marker = "libcgj-batch7256"
 *
 * Exclusive continuum CREATE-ONLY (7251-7260: sendmsg flag stubs —
 * dontwait_id, nosignal_id, more_id, has_dontwait, has_nosignal,
 * has_more, flags_ok, flags_pack, flags_errorish, batch_id_7260).
 * Unique gj_msg_has_more_u_7256 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7256_marker[] = "libcgj-batch7256";

/* MSG_MORE bit (0x8000). */
#define B7256_MSG_MORE ((uint32_t)0x8000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7256_msg_has_more(uint32_t u32Flags)
{
	return ((u32Flags & B7256_MSG_MORE) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_msg_has_more_u_7256 - 1 if flags includes MSG_MORE.
 *
 * flags: sendmsg(2) flags argument
 *
 * Returns 1 when bit 0x8000 is set, else 0. Soft bit test only; does
 * not call sendmsg. No parent wires.
 */
uint32_t
gj_msg_has_more_u_7256(uint32_t u32Flags)
{
	(void)NULL;
	return b7256_msg_has_more(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_msg_has_more_u_7256(uint32_t u32Flags)
    __attribute__((alias("gj_msg_has_more_u_7256")));
