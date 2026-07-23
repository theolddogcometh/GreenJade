/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7278: accept4 flags pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_accept4_flags_pack_u_7278(uint32_t want_cloexec,
 *                                         uint32_t want_nonblock);
 *     - Pack soft SOCK_CLOEXEC / SOCK_NONBLOCK wants into a flags mask.
 *   uint32_t __gj_accept4_flags_pack_u_7278  (alias)
 *   __libcgj_batch7278_marker = "libcgj-batch7278"
 *
 * Exclusive continuum CREATE-ONLY (7271-7280: accept4 flag stubs —
 * cloexec_id, nonblock_id, known_mask_id, has_cloexec, has_nonblock,
 * is_zero, flags_ok, flags_pack, flags_errorish, batch_id_7280).
 * Unique gj_accept4_flags_pack_u_7278 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7278_marker[] = "libcgj-batch7278";

/* Soft pack bits matching Linux accept4 SOCK_* flag layout. */
#define B7278_SOCK_CLOEXEC  ((uint32_t)0x80000u)
#define B7278_SOCK_NONBLOCK ((uint32_t)0x800u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7278_accept4_flags_pack(uint32_t u32WantCloexec, uint32_t u32WantNonblock)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantCloexec != 0u)
		u32Pack |= B7278_SOCK_CLOEXEC;
	if (u32WantNonblock != 0u)
		u32Pack |= B7278_SOCK_NONBLOCK;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_accept4_flags_pack_u_7278 - soft-pack accept4 flag wants.
 *
 * want_cloexec:  non-zero to set SOCK_CLOEXEC (0x80000)
 * want_nonblock: non-zero to set SOCK_NONBLOCK (0x800)
 *
 * Returns bitmask of requested SOCK_* accept4 flag bits. Pure integer
 * pack; does not call accept4. No parent wires.
 */
uint32_t
gj_accept4_flags_pack_u_7278(uint32_t u32WantCloexec, uint32_t u32WantNonblock)
{
	(void)NULL;
	return b7278_accept4_flags_pack(u32WantCloexec, u32WantNonblock);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_accept4_flags_pack_u_7278(uint32_t u32WantCloexec,
                                        uint32_t u32WantNonblock)
    __attribute__((alias("gj_accept4_flags_pack_u_7278")));
