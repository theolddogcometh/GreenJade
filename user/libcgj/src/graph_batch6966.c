/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6966: mlockall MCL_ONFAULT flag bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mlockall_onfault_u_6966(uint32_t flags);
 *     - Return 1 when MCL_ONFAULT (0x4) is set in flags; else 0.
 *   uint32_t __gj_mlockall_onfault_u_6966  (alias)
 *   __libcgj_batch6966_marker = "libcgj-batch6966"
 *
 * Exclusive continuum CREATE-ONLY (6961-6970: mlock2 flags stubs —
 * onfault, flags_zero_ok, flags_mask_ok, mcl_current, mcl_future,
 * mcl_onfault, mcl_pack, mcl_mask_ok, flags_ready, batch_id_6970).
 * Unique gj_mlockall_onfault_u_6966 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6966_marker[] = "libcgj-batch6966";

/* Soft MCL_ONFAULT (lock pages only when faulted in). */
#define B6966_MCL_ONFAULT  ((uint32_t)0x4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6966_mcl_onfault(uint32_t u32Flags)
{
	if ((u32Flags & B6966_MCL_ONFAULT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mlockall_onfault_u_6966 - soft MCL_ONFAULT presence test.
 *
 * flags: raw mlockall flags word
 *
 * Returns 1 when bit 2 (MCL_ONFAULT) is set, else 0. Pure mask.
 * No parent wires.
 */
uint32_t
gj_mlockall_onfault_u_6966(uint32_t flags)
{
	(void)NULL;
	return b6966_mcl_onfault(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mlockall_onfault_u_6966(uint32_t flags)
    __attribute__((alias("gj_mlockall_onfault_u_6966")));
