/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9351: soft systemd unit ActiveState "active" id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sd_unit_active_id_9351(void);
 *     - Returns 1 (soft ActiveState catalog id for "active"). Soft
 *       continuum constant only; no D-Bus, no systemd IPC.
 *   uint32_t __gj_sd_unit_active_id_9351  (alias)
 *   __libcgj_batch9351_marker = "libcgj-batch9351"
 *
 * Exclusive continuum CREATE-ONLY (9351-9360: systemd soft id stubs —
 * sd_unit_active_id_9351, sd_unit_inactive_id_9352,
 * sd_unit_failed_id_9353, sd_bus_ok_u_9354, sd_journal_ok_u_9355,
 * sd_notify_ok_u_9356, sd_listen_fds_ok_u_9357, sd_booted_ok_u_9358,
 * systemd_ready_u_9359, batch_id_9360).
 * Unique surface only; no multi-def. Distinct from
 * gj_sd_unit_is_active_6163. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9351_marker[] = "libcgj-batch9351";

/* Soft ActiveState catalog id: active (integer stub, not D-Bus string). */
#define B9351_UNIT_ACTIVE_ID  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9351_active_id(void)
{
	return B9351_UNIT_ACTIVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sd_unit_active_id_9351 - soft unit ActiveState "active" catalog id.
 *
 * Always returns 1. Soft continuum constant; does not query systemd.
 * No parent wires.
 */
uint32_t
gj_sd_unit_active_id_9351(void)
{
	(void)NULL;
	return b9351_active_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sd_unit_active_id_9351(void)
    __attribute__((alias("gj_sd_unit_active_id_9351")));
