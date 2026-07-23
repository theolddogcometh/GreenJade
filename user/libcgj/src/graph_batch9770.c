/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9770: smartmon soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9770(void);
 *     - Returns the compile-time graph batch number for this TU (9770).
 *   uint32_t __gj_batch_id_9770  (alias)
 *   __libcgj_batch9770_marker = "libcgj-batch9770"
 *
 * Exclusive continuum CREATE-ONLY (9761-9770: smartmon soft id
 * stubs — smart_ok_u_9761, smart_temp_ok_u_9762,
 * smart_health_ok_u_9763, smart_attr_ok_u_9764,
 * smart_selftest_ok_u_9765, nvme_smart_ok_u_9766,
 * ata_smart_ok_u_9767, scsi_smart_ok_u_9768, smartmon_ready_u_9769,
 * batch_id_9770). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9770_marker[] = "libcgj-batch9770";

#define B9770_BATCH_ID  9770u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9770_id(void)
{
	return B9770_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9770 - report this TU's graph batch number.
 *
 * Always returns 9770.
 */
uint32_t
gj_batch_id_9770(void)
{
	(void)NULL;
	return b9770_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9770(void)
    __attribute__((alias("gj_batch_id_9770")));
