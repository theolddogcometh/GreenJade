/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6962: mlock2 flags-zero (default/eager) soft-ok.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mlock2_flags_zero_ok_6962(uint32_t flags);
 *     - Return 1 when flags is 0 (default mlock2: lock pages now, like
 *       mlock); else 0.
 *   uint32_t __gj_mlock2_flags_zero_ok_6962  (alias)
 *   __libcgj_batch6962_marker = "libcgj-batch6962"
 *
 * Exclusive continuum CREATE-ONLY (6961-6970: mlock2 flags stubs —
 * onfault, flags_zero_ok, flags_mask_ok, mcl_current, mcl_future,
 * mcl_onfault, mcl_pack, mcl_mask_ok, flags_ready, batch_id_6970).
 * Unique gj_mlock2_flags_zero_ok_6962 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6962_marker[] = "libcgj-batch6962";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6962_flags_zero_ok(uint32_t u32Flags)
{
	if (u32Flags == 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mlock2_flags_zero_ok_6962 - soft default mlock2 flags gate.
 *
 * flags: raw mlock2 flags word
 *
 * Returns 1 when flags is exactly 0 (eager lock-all-now mode), else 0.
 * Pure integer. No parent wires.
 */
uint32_t
gj_mlock2_flags_zero_ok_6962(uint32_t flags)
{
	(void)NULL;
	return b6962_flags_zero_ok(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mlock2_flags_zero_ok_6962(uint32_t flags)
    __attribute__((alias("gj_mlock2_flags_zero_ok_6962")));
