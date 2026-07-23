/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5563: NVMe SMART percentage-used stub (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_smart_used_u(uint32_t used);
 *     - Clamp SMART Percentage Used field to [0, 100] for gate math.
 *       (Real drives may report >100; this stub saturates at 100.)
 *   uint32_t __gj_nvme_smart_used_u  (alias)
 *   __libcgj_batch5563_marker = "libcgj-batch5563"
 *
 * Exclusive continuum CREATE-ONLY (5561-5570: storage unique —
 * nvme_smart_temp_u, nvme_smart_spare_u, nvme_smart_used_u,
 * nvme_smart_crit_u, stor_free_pct_u, stor_free_ok_u,
 * lib_drive_mount_ok_u, lib_drive_rw_u, bar3_stor_gate_u,
 * batch_id_5570). Unique gj_nvme_smart_used_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5563_marker[] = "libcgj-batch5563";

/* Percent ceiling for Percentage Used gate math. */
#define B5563_PCT_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5563_used(uint32_t u32Used)
{
	if (u32Used > B5563_PCT_MAX) {
		return B5563_PCT_MAX;
	}
	return u32Used;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_smart_used_u - clamp NVMe SMART Percentage Used.
 *
 * used: raw Percentage Used field
 *
 * Returns used clamped to [0, 100]. Stub only; no device access.
 * No parent wires.
 */
uint32_t
gj_nvme_smart_used_u(uint32_t u32Used)
{
	(void)NULL;
	return b5563_used(u32Used);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_smart_used_u(uint32_t u32Used)
    __attribute__((alias("gj_nvme_smart_used_u")));
