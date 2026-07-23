/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9970: install soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9970(void);
 *     - Returns the compile-time graph batch number for this TU (9970).
 *   uint32_t __gj_batch_id_9970  (alias)
 *   __libcgj_batch9970_marker = "libcgj-batch9970"
 *
 * Exclusive continuum CREATE-ONLY (9961-9970: real-hw install soft stubs
 * bar3 path — install_disk_ok_u_9961, install_partition_ok_u_9962,
 * install_esp_ok_u_9963, install_rootfs_ok_u_9964,
 * install_bootloader_ok_u_9965, install_kernel_ok_u_9966,
 * install_initrd_ok_u_9967, install_config_ok_u_9968,
 * install_soft_ready_u_9969, batch_id_9970). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9970_marker[] = "libcgj-batch9970";

#define B9970_BATCH_ID  9970u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9970_id(void)
{
	return B9970_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9970 - report this TU's graph batch number.
 *
 * Always returns 9970. Link-time presence tags the exclusive real-hw
 * install soft stub (bar3 path) continuum wave end. No parent wires.
 */
uint32_t
gj_batch_id_9970(void)
{
	(void)NULL;
	return b9970_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9970(void)
    __attribute__((alias("gj_batch_id_9970")));
