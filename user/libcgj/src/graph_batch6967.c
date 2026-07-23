/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6967: mlockall flags pack soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mlockall_pack_u_6967(uint32_t want_current,
 *                                    uint32_t want_future,
 *                                    uint32_t want_onfault);
 *     - Soft pack: non-zero want_current → MCL_CURRENT, want_future →
 *       MCL_FUTURE, want_onfault → MCL_ONFAULT; OR the selected bits.
 *   uint32_t __gj_mlockall_pack_u_6967  (alias)
 *   __libcgj_batch6967_marker = "libcgj-batch6967"
 *
 * Exclusive continuum CREATE-ONLY (6961-6970: mlock2 flags stubs —
 * onfault, flags_zero_ok, flags_mask_ok, mcl_current, mcl_future,
 * mcl_onfault, mcl_pack, mcl_mask_ok, flags_ready, batch_id_6970).
 * Unique gj_mlockall_pack_u_6967 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6967_marker[] = "libcgj-batch6967";

#define B6967_MCL_CURRENT  ((uint32_t)0x1u)
#define B6967_MCL_FUTURE   ((uint32_t)0x2u)
#define B6967_MCL_ONFAULT  ((uint32_t)0x4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6967_mcl_pack(uint32_t u32Cur, uint32_t u32Fut, uint32_t u32Onf)
{
	uint32_t u32Out = 0u;

	if (u32Cur != 0u) {
		u32Out |= B6967_MCL_CURRENT;
	}
	if (u32Fut != 0u) {
		u32Out |= B6967_MCL_FUTURE;
	}
	if (u32Onf != 0u) {
		u32Out |= B6967_MCL_ONFAULT;
	}
	return u32Out;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mlockall_pack_u_6967 - soft pack of mlockall request bits into flags.
 *
 * want_current: non-zero to include MCL_CURRENT
 * want_future:  non-zero to include MCL_FUTURE
 * want_onfault: non-zero to include MCL_ONFAULT
 *
 * Returns packed flags mask (0..7 low bits). Pure integer. No parent wires.
 */
uint32_t
gj_mlockall_pack_u_6967(uint32_t want_current, uint32_t want_future,
    uint32_t want_onfault)
{
	(void)NULL;
	return b6967_mcl_pack(want_current, want_future, want_onfault);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mlockall_pack_u_6967(uint32_t want_current, uint32_t want_future,
    uint32_t want_onfault)
    __attribute__((alias("gj_mlockall_pack_u_6967")));
