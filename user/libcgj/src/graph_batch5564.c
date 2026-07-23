/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5564: NVMe SMART critical-warning predicate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_smart_crit_u(uint32_t crit);
 *     - Return 1 if any of SMART Critical Warning bits [0..4] are set
 *       ((crit & 0x1f) != 0), else 0.
 *   uint32_t __gj_nvme_smart_crit_u  (alias)
 *   __libcgj_batch5564_marker = "libcgj-batch5564"
 *
 * Exclusive continuum CREATE-ONLY (5561-5570: storage unique —
 * nvme_smart_temp_u, nvme_smart_spare_u, nvme_smart_used_u,
 * nvme_smart_crit_u, stor_free_pct_u, stor_free_ok_u,
 * lib_drive_mount_ok_u, lib_drive_rw_u, bar3_stor_gate_u,
 * batch_id_5570). Unique gj_nvme_smart_crit_u surface only; no
 * multi-def. Distinct from gj_nvme_status_ok_u (batch5258).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5564_marker[] = "libcgj-batch5564";

/* NVMe SMART Critical Warning defined bits (0..4). */
#define B5564_CRIT_MASK  0x1fu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5564_crit(uint32_t u32Crit)
{
	if ((u32Crit & B5564_CRIT_MASK) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_smart_crit_u - 1 if SMART Critical Warning bits present.
 *
 * crit: raw Critical Warning byte/field
 *
 * Returns 1 when (crit & 0x1f) != 0, else 0. Stub only; no device
 * access. No parent wires.
 */
uint32_t
gj_nvme_smart_crit_u(uint32_t u32Crit)
{
	(void)NULL;
	return b5564_crit(u32Crit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_smart_crit_u(uint32_t u32Crit)
    __attribute__((alias("gj_nvme_smart_crit_u")));
