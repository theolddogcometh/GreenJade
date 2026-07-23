/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6965: mlockall MCL_FUTURE flag bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mlockall_future_u_6965(uint32_t flags);
 *     - Return 1 when MCL_FUTURE (0x2) is set in flags; else 0.
 *   uint32_t __gj_mlockall_future_u_6965  (alias)
 *   __libcgj_batch6965_marker = "libcgj-batch6965"
 *
 * Exclusive continuum CREATE-ONLY (6961-6970: mlock2 flags stubs —
 * onfault, flags_zero_ok, flags_mask_ok, mcl_current, mcl_future,
 * mcl_onfault, mcl_pack, mcl_mask_ok, flags_ready, batch_id_6970).
 * Unique gj_mlockall_future_u_6965 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6965_marker[] = "libcgj-batch6965";

/* Soft MCL_FUTURE (lock future mappings). */
#define B6965_MCL_FUTURE  ((uint32_t)0x2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6965_mcl_future(uint32_t u32Flags)
{
	if ((u32Flags & B6965_MCL_FUTURE) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mlockall_future_u_6965 - soft MCL_FUTURE presence test.
 *
 * flags: raw mlockall flags word
 *
 * Returns 1 when bit 1 (MCL_FUTURE) is set, else 0. Pure mask.
 * No parent wires.
 */
uint32_t
gj_mlockall_future_u_6965(uint32_t flags)
{
	(void)NULL;
	return b6965_mcl_future(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mlockall_future_u_6965(uint32_t flags)
    __attribute__((alias("gj_mlockall_future_u_6965")));
