/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9767: ATA smartmon ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ata_smart_ok_u_9767(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       ATA/SATA SMART status probe.
 *   uint32_t __gj_ata_smart_ok_u_9767  (alias)
 *   __libcgj_batch9767_marker = "libcgj-batch9767"
 *
 * Exclusive continuum CREATE-ONLY (9761-9770: smartmon soft id
 * stubs — smart_ok_u_9761, smart_temp_ok_u_9762,
 * smart_health_ok_u_9763, smart_attr_ok_u_9764,
 * smart_selftest_ok_u_9765, nvme_smart_ok_u_9766,
 * ata_smart_ok_u_9767, scsi_smart_ok_u_9768, smartmon_ready_u_9769,
 * batch_id_9770). Unique surface only; no multi-def. No parent wires.
 * No __int128. No smartmon/ATA implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9767_marker[] = "libcgj-batch9767";

/* Soft ata-smart-ok lamp: always off (not a real ATA SMART probe). */
#define B9767_ATA_SMART_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9767_ata_smart_ok(void)
{
	return B9767_ATA_SMART_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ata_smart_ok_u_9767 - ATA smartmon ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not issue ATA SMART
 * commands or call libc. No parent wires.
 */
uint32_t
gj_ata_smart_ok_u_9767(void)
{
	(void)NULL;
	return b9767_ata_smart_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ata_smart_ok_u_9767(void)
    __attribute__((alias("gj_ata_smart_ok_u_9767")));
