/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5565: free-space percent from free/total (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_stor_free_pct_u(uint32_t free_u, uint32_t total_u);
 *     - Return floor((free_u * 100) / total_u) when total_u != 0,
 *       else 0. Result is at most 100 when free_u <= total_u.
 *   uint32_t __gj_stor_free_pct_u  (alias)
 *   __libcgj_batch5565_marker = "libcgj-batch5565"
 *
 * Exclusive continuum CREATE-ONLY (5561-5570: storage unique —
 * nvme_smart_temp_u, nvme_smart_spare_u, nvme_smart_used_u,
 * nvme_smart_crit_u, stor_free_pct_u, stor_free_ok_u,
 * lib_drive_mount_ok_u, lib_drive_rw_u, bar3_stor_gate_u,
 * batch_id_5570). Unique gj_stor_free_pct_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5565_marker[] = "libcgj-batch5565";

#define B5565_PCT_SCALE  100u
#define B5565_PCT_MAX    100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5565_free_pct(uint32_t u32Free, uint32_t u32Total)
{
	uint32_t u32Pct;

	if (u32Total == 0u) {
		return 0u;
	}
	/* Cap free at total so product stays in-range for uint32 percent. */
	if (u32Free > u32Total) {
		u32Free = u32Total;
	}
	u32Pct = (u32Free * B5565_PCT_SCALE) / u32Total;
	if (u32Pct > B5565_PCT_MAX) {
		return B5565_PCT_MAX;
	}
	return u32Pct;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stor_free_pct_u - free space as a percent of total.
 *
 * free_u: free capacity units (same unit as total_u)
 * total_u: total capacity units
 *
 * Returns floor((free * 100) / total), or 0 when total is 0. When
 * free > total, free is treated as total (pct = 100). Integer only.
 * No parent wires.
 */
uint32_t
gj_stor_free_pct_u(uint32_t u32Free, uint32_t u32Total)
{
	(void)NULL;
	return b5565_free_pct(u32Free, u32Total);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_stor_free_pct_u(uint32_t u32Free, uint32_t u32Total)
    __attribute__((alias("gj_stor_free_pct_u")));
