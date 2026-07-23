/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5566: free-space threshold OK predicate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_stor_free_ok_u(uint32_t free_pct, uint32_t min_pct);
 *     - Return 1 if free_pct >= min_pct, else 0.
 *   uint32_t __gj_stor_free_ok_u  (alias)
 *   __libcgj_batch5566_marker = "libcgj-batch5566"
 *
 * Exclusive continuum CREATE-ONLY (5561-5570: storage unique —
 * nvme_smart_temp_u, nvme_smart_spare_u, nvme_smart_used_u,
 * nvme_smart_crit_u, stor_free_pct_u, stor_free_ok_u,
 * lib_drive_mount_ok_u, lib_drive_rw_u, bar3_stor_gate_u,
 * batch_id_5570). Unique gj_stor_free_ok_u surface only; no multi-def.
 * Complements gj_stor_free_pct_u without parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5566_marker[] = "libcgj-batch5566";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5566_free_ok(uint32_t u32FreePct, uint32_t u32MinPct)
{
	if (u32FreePct >= u32MinPct) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stor_free_ok_u - 1 if free percent meets minimum threshold.
 *
 * free_pct: free space percent [0..100]-style
 * min_pct: required minimum free percent
 *
 * Returns 1 when free_pct >= min_pct, else 0. Self-contained; does not
 * call gj_stor_free_pct_u. No parent wires.
 */
uint32_t
gj_stor_free_ok_u(uint32_t u32FreePct, uint32_t u32MinPct)
{
	(void)NULL;
	return b5566_free_ok(u32FreePct, u32MinPct);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_stor_free_ok_u(uint32_t u32FreePct, uint32_t u32MinPct)
    __attribute__((alias("gj_stor_free_ok_u")));
