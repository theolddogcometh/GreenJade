/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7277: accept4 flags validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_accept4_flags_ok_u_7277(uint32_t flags);
 *     - Return 1 if flags uses only known accept4 bits
 *       (SOCK_CLOEXEC|SOCK_NONBLOCK = 0x80800), else 0.
 *   uint32_t __gj_accept4_flags_ok_u_7277  (alias)
 *   __libcgj_batch7277_marker = "libcgj-batch7277"
 *
 * Exclusive continuum CREATE-ONLY (7271-7280: accept4 flag stubs —
 * cloexec_id, nonblock_id, known_mask_id, has_cloexec, has_nonblock,
 * is_zero, flags_ok, flags_pack, flags_errorish, batch_id_7280).
 * Unique gj_accept4_flags_ok_u_7277 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7277_marker[] = "libcgj-batch7277";

/*
 * Known accept4 flags mask:
 *   SOCK_CLOEXEC (0x80000) | SOCK_NONBLOCK (0x800) = 0x80800
 */
#define B7277_ACCEPT4_KNOWN_MASK ((uint32_t)0x80800u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7277_accept4_flags_ok(uint32_t u32Flags)
{
	return ((u32Flags & ~B7277_ACCEPT4_KNOWN_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_accept4_flags_ok_u_7277 - 1 if flags has no unknown bits.
 *
 * flags: accept4(2) flags argument
 *
 * Returns 1 when only bits in {0x80000, 0x800} are set (including 0),
 * else 0. Soft catalog check; does not call accept4. No parent wires.
 */
uint32_t
gj_accept4_flags_ok_u_7277(uint32_t u32Flags)
{
	(void)NULL;
	return b7277_accept4_flags_ok(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_accept4_flags_ok_u_7277(uint32_t u32Flags)
    __attribute__((alias("gj_accept4_flags_ok_u_7277")));
