/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7274: accept4 SOCK_CLOEXEC presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_accept4_has_cloexec_u_7274(uint32_t flags);
 *     - Return 1 if flags has SOCK_CLOEXEC (0x80000) set, else 0.
 *   uint32_t __gj_accept4_has_cloexec_u_7274  (alias)
 *   __libcgj_batch7274_marker = "libcgj-batch7274"
 *
 * Exclusive continuum CREATE-ONLY (7271-7280: accept4 flag stubs —
 * cloexec_id, nonblock_id, known_mask_id, has_cloexec, has_nonblock,
 * is_zero, flags_ok, flags_pack, flags_errorish, batch_id_7280).
 * Unique gj_accept4_has_cloexec_u_7274 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7274_marker[] = "libcgj-batch7274";

/* SOCK_CLOEXEC bit (0x80000). */
#define B7274_SOCK_CLOEXEC ((uint32_t)0x80000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7274_accept4_has_cloexec(uint32_t u32Flags)
{
	return ((u32Flags & B7274_SOCK_CLOEXEC) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_accept4_has_cloexec_u_7274 - 1 if flags includes SOCK_CLOEXEC.
 *
 * flags: accept4(2) flags argument
 *
 * Returns 1 when bit 0x80000 is set, else 0. Soft bit test only; does
 * not call accept4. No parent wires.
 */
uint32_t
gj_accept4_has_cloexec_u_7274(uint32_t u32Flags)
{
	(void)NULL;
	return b7274_accept4_has_cloexec(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_accept4_has_cloexec_u_7274(uint32_t u32Flags)
    __attribute__((alias("gj_accept4_has_cloexec_u_7274")));
