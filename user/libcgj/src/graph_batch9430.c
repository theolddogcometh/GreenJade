/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9430: bluez soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9430(void);
 *     - Returns the compile-time graph batch number for this TU (9430).
 *   uint32_t __gj_batch_id_9430  (alias)
 *   __libcgj_batch9430_marker = "libcgj-batch9430"
 *
 * Exclusive continuum CREATE-ONLY (9421-9430: bluez soft id stubs —
 * bt_adapter_ok_u_9421, bt_device_ok_u_9422, bt_gatt_ok_u_9423,
 * bt_a2dp_ok_u_9424, bt_hfp_ok_u_9425, bt_scan_ok_u_9426,
 * bt_pair_ok_u_9427, bt_connect_ok_u_9428, bluez_ready_u_9429,
 * batch_id_9430). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9430_marker[] = "libcgj-batch9430";

#define B9430_BATCH_ID  9430u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9430_id(void)
{
	return B9430_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9430 - report this TU's graph batch number.
 *
 * Always returns 9430.
 */
uint32_t
gj_batch_id_9430(void)
{
	(void)NULL;
	return b9430_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9430(void)
    __attribute__((alias("gj_batch_id_9430")));
