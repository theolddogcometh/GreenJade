/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9650: qemu soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9650(void);
 *     - Returns the compile-time graph batch number for this TU (9650).
 *   uint32_t __gj_batch_id_9650  (alias)
 *   __libcgj_batch9650_marker = "libcgj-batch9650"
 *
 * Exclusive continuum CREATE-ONLY (9641-9650: qemu soft id stubs —
 * qemu_ok_u_9641, qemu_machine_ok_u_9642, qemu_cpu_ok_u_9643,
 * qemu_drive_ok_u_9644, qemu_net_ok_u_9645, qemu_display_ok_u_9646,
 * qemu_snapshot_ok_u_9647, qemu_monitor_ok_u_9648, qemu_ready_u_9649,
 * batch_id_9650). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9650_marker[] = "libcgj-batch9650";

#define B9650_BATCH_ID  9650u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9650_id(void)
{
	return B9650_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9650 - report this TU's graph batch number.
 *
 * Always returns 9650.
 */
uint32_t
gj_batch_id_9650(void)
{
	(void)NULL;
	return b9650_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9650(void)
    __attribute__((alias("gj_batch_id_9650")));
