/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9643: qemu cpu ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_qemu_cpu_ok_u_9643(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       QEMU CPU model / vCPU validity probe.
 *   uint32_t __gj_qemu_cpu_ok_u_9643  (alias)
 *   __libcgj_batch9643_marker = "libcgj-batch9643"
 *
 * Exclusive continuum CREATE-ONLY (9641-9650: qemu soft id stubs —
 * qemu_ok_u_9641, qemu_machine_ok_u_9642, qemu_cpu_ok_u_9643,
 * qemu_drive_ok_u_9644, qemu_net_ok_u_9645, qemu_display_ok_u_9646,
 * qemu_snapshot_ok_u_9647, qemu_monitor_ok_u_9648, qemu_ready_u_9649,
 * batch_id_9650). Unique surface only; no multi-def. No parent wires.
 * No __int128. No QEMU implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9643_marker[] = "libcgj-batch9643";

/* Soft qemu cpu-ok lamp: always off (not a real CPU/vCPU probe). */
#define B9643_QEMU_CPU_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9643_qemu_cpu_ok(void)
{
	return B9643_QEMU_CPU_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_qemu_cpu_ok_u_9643 - qemu cpu ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not touch QEMU CPU
 * models or call libc. No parent wires.
 */
uint32_t
gj_qemu_cpu_ok_u_9643(void)
{
	(void)NULL;
	return b9643_qemu_cpu_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_qemu_cpu_ok_u_9643(void)
    __attribute__((alias("gj_qemu_cpu_ok_u_9643")));
