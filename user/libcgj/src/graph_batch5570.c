/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5570: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5570(void);
 *     - Returns the compile-time graph batch number for this TU (5570).
 *   uint32_t __gj_batch_id_5570  (alias)
 *   __libcgj_batch5570_marker = "libcgj-batch5570"
 *
 * Exclusive continuum CREATE-ONLY (5561-5570: storage unique —
 * nvme_smart_temp_u, nvme_smart_spare_u, nvme_smart_used_u,
 * nvme_smart_crit_u, stor_free_pct_u, stor_free_ok_u,
 * lib_drive_mount_ok_u, lib_drive_rw_u, bar3_stor_gate_u,
 * batch_id_5570). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5570_marker[] = "libcgj-batch5570";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5570_id(void)
{
	return 5570u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5570 - report this TU's graph batch number.
 *
 * Always returns 5570. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5570(void)
{
	(void)NULL;
	return b5570_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5570(void)
    __attribute__((alias("gj_batch_id_5570")));
