/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9353: soft systemd unit ActiveState "failed" id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sd_unit_failed_id_9353(void);
 *     - Returns 3 (soft ActiveState catalog id for "failed"). Soft
 *       continuum constant only; no D-Bus, no systemd IPC.
 *   uint32_t __gj_sd_unit_failed_id_9353  (alias)
 *   __libcgj_batch9353_marker = "libcgj-batch9353"
 *
 * Exclusive continuum CREATE-ONLY (9351-9360: systemd soft id stubs —
 * sd_unit_active_id_9351, sd_unit_inactive_id_9352,
 * sd_unit_failed_id_9353, sd_bus_ok_u_9354, sd_journal_ok_u_9355,
 * sd_notify_ok_u_9356, sd_listen_fds_ok_u_9357, sd_booted_ok_u_9358,
 * systemd_ready_u_9359, batch_id_9360).
 * Unique surface only; no multi-def. Distinct from
 * gj_sd_unit_is_failed_6164. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9353_marker[] = "libcgj-batch9353";

/* Soft ActiveState catalog id: failed (integer stub, not D-Bus string). */
#define B9353_UNIT_FAILED_ID  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9353_failed_id(void)
{
	return B9353_UNIT_FAILED_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sd_unit_failed_id_9353 - soft unit ActiveState "failed" catalog id.
 *
 * Always returns 3. Soft continuum constant; does not query systemd.
 * No parent wires.
 */
uint32_t
gj_sd_unit_failed_id_9353(void)
{
	(void)NULL;
	return b9353_failed_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sd_unit_failed_id_9353(void)
    __attribute__((alias("gj_sd_unit_failed_id_9353")));
