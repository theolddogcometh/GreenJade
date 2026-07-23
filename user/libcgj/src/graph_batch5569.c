/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5569: bar3 storage readiness gate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_stor_gate_u(uint32_t free_pct, uint32_t crit);
 *     - Return 1 when free_pct >= 10 and SMART critical warning is
 *       clear (crit == 0), else 0. Soft bar3 storage gate stub.
 *   uint32_t __gj_bar3_stor_gate_u  (alias)
 *   __libcgj_batch5569_marker = "libcgj-batch5569"
 *
 * Exclusive continuum CREATE-ONLY (5561-5570: storage unique —
 * nvme_smart_temp_u, nvme_smart_spare_u, nvme_smart_used_u,
 * nvme_smart_crit_u, stor_free_pct_u, stor_free_ok_u,
 * lib_drive_mount_ok_u, lib_drive_rw_u, bar3_stor_gate_u,
 * batch_id_5570). Unique gj_bar3_stor_gate_u surface only; no multi-def.
 * Distinct from gj_bar3_ready_hint (batch2299), gj_bar3_steam_bit
 * (batch2498), gj_bar3_deck_bit (batch2499). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5569_marker[] = "libcgj-batch5569";

/* Minimum free percent for bar3 storage gate pass. */
#define B5569_MIN_FREE_PCT  10u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5569_stor_gate(uint32_t u32FreePct, uint32_t u32Crit)
{
	if (u32Crit != 0u) {
		return 0u;
	}
	if (u32FreePct < B5569_MIN_FREE_PCT) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_stor_gate_u - bar3 storage gate (free space + SMART clear).
 *
 * free_pct: free space percent [0..100]-style
 * crit: SMART critical-warning presence (0 = clear)
 *
 * Returns 1 when free_pct >= 10 and crit == 0, else 0. Self-contained;
 * does not call stor/smart helpers. No parent wires.
 */
uint32_t
gj_bar3_stor_gate_u(uint32_t u32FreePct, uint32_t u32Crit)
{
	(void)NULL;
	return b5569_stor_gate(u32FreePct, u32Crit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_stor_gate_u(uint32_t u32FreePct, uint32_t u32Crit)
    __attribute__((alias("gj_bar3_stor_gate_u")));
