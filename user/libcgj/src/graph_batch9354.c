/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9354: soft sd-bus ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sd_bus_ok_u_9354(void);
 *     - Returns 1 (sd-bus soft-id continuum ok). Pure-data product
 *       tag; does not open or talk on the system bus.
 *   uint32_t __gj_sd_bus_ok_u_9354  (alias)
 *   __libcgj_batch9354_marker = "libcgj-batch9354"
 *
 * Exclusive continuum CREATE-ONLY (9351-9360: systemd soft id stubs —
 * sd_unit_active_id_9351, sd_unit_inactive_id_9352,
 * sd_unit_failed_id_9353, sd_bus_ok_u_9354, sd_journal_ok_u_9355,
 * sd_notify_ok_u_9356, sd_listen_fds_ok_u_9357, sd_booted_ok_u_9358,
 * systemd_ready_u_9359, batch_id_9360).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9354_marker[] = "libcgj-batch9354";

#define B9354_SD_BUS_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9354_bus_ok(void)
{
	return B9354_SD_BUS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sd_bus_ok_u_9354 - sd-bus soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not open buses
 * or call libc. No parent wires.
 */
uint32_t
gj_sd_bus_ok_u_9354(void)
{
	(void)NULL;
	return b9354_bus_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sd_bus_ok_u_9354(void)
    __attribute__((alias("gj_sd_bus_ok_u_9354")));
