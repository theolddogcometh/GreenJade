/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6160: network manager stub states wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6160(void);
 *     - Returns the compile-time graph batch number for this TU (6160).
 *   uint32_t __gj_batch_id_6160  (alias)
 *   __libcgj_batch6160_marker = "libcgj-batch6160"
 *
 * Exclusive continuum CREATE-ONLY network manager stub states wave
 * (6151-6160: nm_mgr_state_unknown_6151, nm_mgr_state_asleep_6152,
 * nm_mgr_state_disconnected_6153, nm_mgr_state_connecting_6154,
 * nm_mgr_state_connected_local_6155, nm_mgr_state_connected_global_6156,
 * nm_dev_state_activated_6157, nm_connectivity_full_6158,
 * nm_mgr_is_connected_6159, batch_id_6160). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols —
 * avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6160_marker[] = "libcgj-batch6160";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6160_id(void)
{
	return 6160u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6160 - report this TU's graph batch number.
 *
 * Always returns 6160. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_6160(void)
{
	(void)NULL;
	return b6160_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_6160(void)
    __attribute__((alias("gj_batch_id_6160")));
