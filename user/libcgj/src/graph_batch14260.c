/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14260: udev soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14260(void);
 *     - Returns the compile-time graph batch number for this TU (14260).
 *   uint32_t __gj_batch_id_14260  (alias)
 *   __libcgj_batch14260_marker = "libcgj-batch14260"
 *
 * Exclusive continuum CREATE-ONLY (14251-14260: udev soft id stubs —
 * udev_action_add_id_14251, udev_action_remove_id_14252,
 * udev_action_change_id_14253, udev_subsystem_ok_u_14254,
 * udev_devnode_ok_u_14255, udev_syspath_ok_u_14256,
 * udev_monitor_ok_u_14257, udev_enumerate_ok_u_14258,
 * udev_ready_u_14259, batch_id_14260).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128. No udev implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14260_marker[] = "libcgj-batch14260";

#define B14260_BATCH_ID  14260u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14260_id(void)
{
	return B14260_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14260 - report this TU's graph batch number.
 *
 * Always returns 14260. Link-time presence tags the exclusive
 * udev soft-id continuum wave end. No parent wires.
 */
uint32_t
gj_batch_id_14260(void)
{
	(void)NULL;
	return b14260_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14260(void)
    __attribute__((alias("gj_batch_id_14260")));
