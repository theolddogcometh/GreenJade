/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6963: mlock2 flags known-mask soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mlock2_flags_mask_ok_6963(uint32_t flags);
 *     - Return 1 when flags has only the known soft bit MLOCK_ONFAULT
 *       (bit 0), including zero; 0 if any higher bit is set.
 *   uint32_t __gj_mlock2_flags_mask_ok_6963  (alias)
 *   __libcgj_batch6963_marker = "libcgj-batch6963"
 *
 * Exclusive continuum CREATE-ONLY (6961-6970: mlock2 flags stubs —
 * onfault, flags_zero_ok, flags_mask_ok, mcl_current, mcl_future,
 * mcl_onfault, mcl_pack, mcl_mask_ok, flags_ready, batch_id_6970).
 * Unique gj_mlock2_flags_mask_ok_6963 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6963_marker[] = "libcgj-batch6963";

/* Soft known mlock2 flags bits (MLOCK_ONFAULT only). */
#define B6963_MLOCK2_KNOWN  ((uint32_t)0x1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6963_flags_mask_ok(uint32_t u32Flags)
{
	if ((u32Flags & ~B6963_MLOCK2_KNOWN) != 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mlock2_flags_mask_ok_6963 - soft known-mlock2-bits validity.
 *
 * flags: raw mlock2 flags word
 *
 * Returns 1 when only bit 0 may be set (MLOCK_ONFAULT or 0), else 0.
 * Pure integer. No parent wires.
 */
uint32_t
gj_mlock2_flags_mask_ok_6963(uint32_t flags)
{
	(void)NULL;
	return b6963_flags_mask_ok(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mlock2_flags_mask_ok_6963(uint32_t flags)
    __attribute__((alias("gj_mlock2_flags_mask_ok_6963")));
