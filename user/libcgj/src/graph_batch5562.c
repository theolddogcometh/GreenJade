/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5562: NVMe SMART available-spare percent stub (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_smart_spare_u(uint32_t spare);
 *     - Clamp SMART Available Spare field to [0, 100] percent.
 *   uint32_t __gj_nvme_smart_spare_u  (alias)
 *   __libcgj_batch5562_marker = "libcgj-batch5562"
 *
 * Exclusive continuum CREATE-ONLY (5561-5570: storage unique —
 * nvme_smart_temp_u, nvme_smart_spare_u, nvme_smart_used_u,
 * nvme_smart_crit_u, stor_free_pct_u, stor_free_ok_u,
 * lib_drive_mount_ok_u, lib_drive_rw_u, bar3_stor_gate_u,
 * batch_id_5570). Unique gj_nvme_smart_spare_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5562_marker[] = "libcgj-batch5562";

/* Percent ceiling for Available Spare. */
#define B5562_PCT_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5562_spare(uint32_t u32Spare)
{
	if (u32Spare > B5562_PCT_MAX) {
		return B5562_PCT_MAX;
	}
	return u32Spare;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_smart_spare_u - clamp NVMe SMART Available Spare percent.
 *
 * spare: raw Available Spare field (percent-style)
 *
 * Returns spare clamped to [0, 100]. Stub only; no device access.
 * No parent wires.
 */
uint32_t
gj_nvme_smart_spare_u(uint32_t u32Spare)
{
	(void)NULL;
	return b5562_spare(u32Spare);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_smart_spare_u(uint32_t u32Spare)
    __attribute__((alias("gj_nvme_smart_spare_u")));
