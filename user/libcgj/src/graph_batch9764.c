/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9764: smartmon attribute ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smart_attr_ok_u_9764(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       SMART attribute threshold / pre-fail probe.
 *   uint32_t __gj_smart_attr_ok_u_9764  (alias)
 *   __libcgj_batch9764_marker = "libcgj-batch9764"
 *
 * Exclusive continuum CREATE-ONLY (9761-9770: smartmon soft id
 * stubs — smart_ok_u_9761, smart_temp_ok_u_9762,
 * smart_health_ok_u_9763, smart_attr_ok_u_9764,
 * smart_selftest_ok_u_9765, nvme_smart_ok_u_9766,
 * ata_smart_ok_u_9767, scsi_smart_ok_u_9768, smartmon_ready_u_9769,
 * batch_id_9770). Unique surface only; no multi-def. No parent wires.
 * No __int128. No smartmon/smartctl implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9764_marker[] = "libcgj-batch9764";

/* Soft smart-attr-ok lamp: always off (not a real attribute probe). */
#define B9764_SMART_ATTR_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9764_smart_attr_ok(void)
{
	return B9764_SMART_ATTR_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smart_attr_ok_u_9764 - smartmon attribute ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not read SMART attributes
 * or call libc. No parent wires.
 */
uint32_t
gj_smart_attr_ok_u_9764(void)
{
	(void)NULL;
	return b9764_smart_attr_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smart_attr_ok_u_9764(void)
    __attribute__((alias("gj_smart_attr_ok_u_9764")));
