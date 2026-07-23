/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9426: BlueZ scan ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bt_scan_ok_u_9426(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       BlueZ discovery/scan readiness probe.
 *   uint32_t __gj_bt_scan_ok_u_9426  (alias)
 *   __libcgj_batch9426_marker = "libcgj-batch9426"
 *
 * Exclusive continuum CREATE-ONLY (9421-9430: bluez soft id stubs —
 * bt_adapter_ok_u_9421, bt_device_ok_u_9422, bt_gatt_ok_u_9423,
 * bt_a2dp_ok_u_9424, bt_hfp_ok_u_9425, bt_scan_ok_u_9426,
 * bt_pair_ok_u_9427, bt_connect_ok_u_9428, bluez_ready_u_9429,
 * batch_id_9430). Unique surface only; no multi-def. No parent wires.
 * No __int128. No BlueZ/D-Bus implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9426_marker[] = "libcgj-batch9426";

/* Soft BlueZ scan-ok lamp: always off (not a real discovery probe). */
#define B9426_BT_SCAN_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9426_bt_scan_ok(void)
{
	return B9426_BT_SCAN_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bt_scan_ok_u_9426 - BlueZ scan ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not start discovery or
 * call BlueZ/libc. No parent wires.
 */
uint32_t
gj_bt_scan_ok_u_9426(void)
{
	(void)NULL;
	return b9426_bt_scan_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bt_scan_ok_u_9426(void)
    __attribute__((alias("gj_bt_scan_ok_u_9426")));
