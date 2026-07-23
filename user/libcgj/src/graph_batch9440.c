/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9440: networkmanager soft id stubs wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9440(void);
 *     - Returns the compile-time graph batch number for this TU (9440).
 *   uint32_t __gj_batch_id_9440  (alias)
 *   __libcgj_batch9440_marker = "libcgj-batch9440"
 *
 * Exclusive continuum CREATE-ONLY networkmanager soft id stubs wave
 * (9431-9440: nm_device_wifi_id_9431, nm_device_eth_id_9432,
 * nm_device_vpn_id_9433, nm_state_connected_id_9434,
 * nm_state_disconnected_id_9435, nm_ap_ok_u_9436, nm_conn_ok_u_9437,
 * nm_settings_ok_u_9438, nm_ready_u_9439, batch_id_9440). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols —
 * avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9440_marker[] = "libcgj-batch9440";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9440_id(void)
{
	return 9440u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9440 - report this TU's graph batch number.
 *
 * Always returns 9440. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_9440(void)
{
	(void)NULL;
	return b9440_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9440(void)
    __attribute__((alias("gj_batch_id_9440")));
