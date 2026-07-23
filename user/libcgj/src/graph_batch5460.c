/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5460: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5460(void);
 *     - Returns the compile-time graph batch number for this TU (5460).
 *   uint32_t __gj_batch_id_5460  (alias)
 *   __libcgj_batch5460_marker = "libcgj-batch5460"
 *
 * Exclusive continuum CREATE-ONLY (5451-5460: deck_tdp_mw_5451,
 * deck_battery_pct_5452, deck_fan_rpm_5453, deck_refresh_hz_5454,
 * deck_hdr_capable_5455, deck_vrr_capable_5456, deck_brightness_pct_5457,
 * deck_ambient_lux_5458, deck_thermal_ok_5459, batch_id_5460). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols —
 * avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5460_marker[] = "libcgj-batch5460";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5460_id(void)
{
	return 5460u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5460 - report this TU's graph batch number.
 *
 * Always returns 5460. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5460(void)
{
	(void)NULL;
	return b5460_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5460(void)
    __attribute__((alias("gj_batch_id_5460")));
